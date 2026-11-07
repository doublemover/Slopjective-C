"""Build and install the pinned native Linux runtime without implicit downloads."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import json
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
if not __package__:
    sys.path.insert(0, str(ROOT))
from scripts.objc3c_toolchain_runtime.source_patches import prepare_source, verify_source


def checkout(path: Path, url: str, revision: str, tag: str, fetch: bool, *, allow_owned_changes=False) -> None:
    if not path.exists():
        if not fetch:
            raise ValueError(f"missing pinned source {path}; use --fetch to acquire it")
        path.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(["git", "clone", "--depth", "1", "--branch", tag, url, str(path)], check=True)
    actual = subprocess.check_output(["git", "-C", str(path), "rev-parse", "HEAD"], text=True).strip()
    if actual != revision:
        raise ValueError(f"source {path} is at {actual}; expected {revision}")
    if not allow_owned_changes and subprocess.check_output(["git", "-C", str(path), "status", "--porcelain"], text=True).strip():
        raise ValueError(f"pinned runtime dependency is dirty: {path}; preserving it")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=ROOT)
    parser.add_argument("--compiler-root", type=Path,
                        default=os.environ.get("OBJC3C_LLVM_ROOT") or os.environ.get("LLVM_ROOT"))
    parser.add_argument("--fetch", action="store_true")
    parser.add_argument("--jobs", type=int, default=2)
    parser.add_argument("--sanitizer", choices=("address,undefined", "thread"),
                        help="build a separate development runtime with visible ARC/Block synchronization")
    parser.add_argument("--report-file", type=Path,
                        help="write the structured result separately from subprocess diagnostics")
    args = parser.parse_args(argv)
    if sys.platform != "linux" or args.jobs < 1 or not args.compiler_root:
        parser.error("native Linux, a compiler root, and a positive job count are required")
    repo = args.repo.resolve()
    profile = json.loads((repo / "scripts/objc3c_toolchain_runtime/runtime-profile.json").read_text())["profiles"]["linux-x86_64"]
    source = repo / "out/toolchains/libobjc2-v2.3/src"
    robin = repo / f"out/toolchains/robin-map-{profile['robin_map_release']}/src"
    bin_dir = args.compiler_root.resolve() / "bin"
    with (bin_dir / 'clang').open('rb') as compiler:
        compiler_sha256 = hashlib.file_digest(compiler, 'sha256').hexdigest()
    configuration = {'compiler_root': str(args.compiler_root.resolve()), 'profile': profile, 'compiler_sha256': compiler_sha256,
                     'build_type': 'Release', 'oldabi': False, 'legacy_compat': False}
    if args.sanitizer:
        resource_root = Path(subprocess.check_output(
            [str(bin_dir / 'clang'), '-print-resource-dir'], text=True).strip()).resolve(strict=True)
        names = ('tsan',) if args.sanitizer == 'thread' else ('asan', 'ubsan')
        runtime_files = [path for path in resource_root.rglob('libclang_rt.*')
                         if path.is_file() and any(name in path.name for name in names)]
        if not runtime_files:
            parser.error("the selected compiler has no matching sanitizer runtimes")
        configuration.update(sanitizer=args.sanitizer, sanitizer_resource_root=str(resource_root),
            sanitizer_runtime_sha256={str(path.relative_to(resource_root)):
                hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(runtime_files)})
    build_key = hashlib.sha256(json.dumps(configuration, sort_keys=True).encode()).hexdigest()
    build = repo / "out/toolchains/libobjc2-v2.3/build-linux-x86_64" / build_key
    stage = build / "stage"
    report = {"status": "running", "profile": "linux-x86_64", "commands": [],
              "started_utc": datetime.now(timezone.utc).isoformat(),
              "configuration": configuration, "build_key": build_key, "source_commit": profile["runtime_commit"], "robin_map_commit": profile["robin_map_commit"]}
    try:
        checkout(source, profile["runtime_source"], profile["runtime_commit"], "v" + profile["runtime_release"], args.fetch,
                 allow_owned_changes=True)
        prepare_source(repo, source, profile)
        checkout(robin, profile["robin_map_source"], profile["robin_map_commit"], profile["robin_map_release"], args.fetch)
        build.mkdir(parents=True, exist_ok=True)
        configure = ["cmake", "-S", str(source), "-B", str(build), "-G", "Ninja",
                     "-DCMAKE_BUILD_TYPE=Release", f"-DCMAKE_INSTALL_PREFIX={stage}",
                     "-DCMAKE_INSTALL_LIBDIR=lib", f"-DFETCHCONTENT_SOURCE_DIR_ROBINMAP={robin}",
                     "-DFETCHCONTENT_FULLY_DISCONNECTED=ON", "-DTESTS=OFF", "-DOLDABI_COMPAT=OFF",
                     "-DLEGACY_COMPAT=OFF", "-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld"]
        for language, compiler in (("C", "clang"), ("CXX", "clang++"),
                                    ("OBJC", "clang"), ("OBJCXX", "clang++")):
            configure.append(f"-DCMAKE_{language}_COMPILER={bin_dir / compiler}")
        if args.sanitizer:
            flags = "-fsanitize=" + args.sanitizer + " -fno-omit-frame-pointer"
            configure += [f"-DCMAKE_{language}_FLAGS={flags}"
                          for language in ("C", "CXX", "OBJC", "OBJCXX")]
            configure += ["-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld -fsanitize=" + args.sanitizer]
        commands = [
            [sys.executable, "-B", str(repo / "scripts/objc3c_toolchain_runtime/verify_runtime_profile.py"),
             "--profile", "linux-x86_64", "--source", str(source),
             "--robin-map-source", str(robin), "--bin-dir", str(bin_dir)],
            configure, ["cmake", "--build", str(build), "--parallel", str(args.jobs)],
            ["cmake", "--install", str(build), "--prefix", str(stage)],
        ]
        for index, command in enumerate(commands):
            started = time.monotonic()
            with (build / f"step-{index}.log").open("w") as log:
                result = subprocess.run(command, stdout=log, stderr=subprocess.STDOUT)
            report["commands"].append({"argv": command, "exit_code": result.returncode,
                                       "elapsed_seconds": round(time.monotonic() - started, 6)})
            if result.returncode:
                raise ValueError(f"runtime step {index} exited {result.returncode}; see {build / f'step-{index}.log'}")
        if not (stage / "lib/libobjc.so").is_file():
            raise ValueError("runtime installation did not produce lib/libobjc.so")
        verify_source(repo, source, profile)
        checkout(robin, profile["robin_map_source"], profile["robin_map_commit"], profile["robin_map_release"], False)
        report.update(status="passed", stage=str(stage), robin_map_source=str(robin))
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        report.update(status="failed", error=str(error))
    finally:
        if build.is_dir():
            report["finished_utc"] = datetime.now(timezone.utc).isoformat()
            (build / "build-report.json").write_text(json.dumps(report, indent=2) + "\n")
    contents = json.dumps(report, indent=2) + "\n"
    if args.report_file:
        args.report_file.parent.mkdir(parents=True, exist_ok=True)
        args.report_file.write_text(contents)
    print(contents, end="")
    return 0 if report["status"] == "passed" else 2


if __name__ == "__main__":
    raise SystemExit(main())
