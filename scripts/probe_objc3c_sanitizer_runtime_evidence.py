#!/usr/bin/env python3
"""Probe a packaged sanitizer runtime with executable native evidence."""

from __future__ import annotations

import argparse
import hashlib
import os
import re
import shutil
import stat
import sys
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.llvm_discovery import find_llvm_tool_path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import command_text, run_completed
from scripts.objc3c_package_channels.sanitizer_contracts import (
    runtime_package_variant_contract,
)

SUMMARY_CONTRACT_ID = "objc3c.security.hardening.sanitizer.runtime-evidence.probe.v1"
DEFAULT_FIXTURE_GLOB = "*assignment_basic_counter.objc3"
WINDOWS_TARGET_PLATFORM = "windows-x64"
DEFAULT_MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"
REPORT_ROOT = ROOT / "tmp" / "reports" / "sanitizer-runtime-evidence"
PACKAGE_ROOT_BASE = (
    ROOT
    / "tmp"
    / "sre"
    / "pkg"
)
SAFE_RUN_ID_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]{0,127}$")


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def output_tail(result: Any, *, max_chars: int = 4000) -> str:
    output_parts = []
    for stream_name in ("stdout", "stderr"):
        stream = getattr(result, stream_name, "")
        if stream:
            output_parts.append(f"{stream_name}:\n{stream[-max_chars:]}")
    return "\n".join(output_parts)


def is_under_root(path: Path, root: Path) -> bool:
    try:
        relative = path.resolve().relative_to(root.resolve())
    except ValueError:
        return False
    return bool(relative.parts)


def is_reparse_point(path: Path) -> bool:
    try:
        if path.is_symlink():
            return True
        is_junction = getattr(path, "is_junction", None)
        if callable(is_junction) and is_junction():
            return True
        attributes = getattr(path.lstat(), "st_file_attributes", 0)
    except OSError:
        return False
    return bool(attributes & getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0))


def expect_plain_existing_ancestors(path: Path, owned_root: Path, label: str) -> None:
    owned_root = owned_root.resolve()
    path = path.resolve()
    try:
        relative = path.relative_to(owned_root)
    except ValueError as exc:
        raise RuntimeError(f"{label} must stay under owned root {repo_rel(owned_root)}: {path}") from exc

    current = owned_root
    if current.exists():
        expect(not is_reparse_point(current), f"{label} owned root is a reparse point: {current}")
    for segment in relative.parts[:-1]:
        current = current / segment
        if current.exists():
            expect(not is_reparse_point(current), f"{label} ancestor is a reparse point: {current}")


def expect_no_reparse_points(path: Path, label: str) -> None:
    if not path.exists():
        return
    expect(not is_reparse_point(path), f"{label} is a reparse point: {path}")
    for current, dir_names, file_names in os.walk(path):
        current_path = Path(current)
        expect(not is_reparse_point(current_path), f"{label} contains reparse point: {current_path}")
        for child_name in [*dir_names, *file_names]:
            child = current_path / child_name
            expect(not is_reparse_point(child), f"{label} contains reparse point: {child}")


def resolve_owned_path(raw_path: str, default_path: Path, owned_root: Path, label: str) -> Path:
    path = Path(raw_path) if raw_path else default_path
    if not path.is_absolute():
        path = ROOT / path
    resolved = path.resolve()
    expect(
        is_under_root(resolved, owned_root),
        f"{label} must stay under owned root {repo_rel(owned_root)}: {resolved}",
    )
    expect_plain_existing_ancestors(resolved, owned_root, label)
    return resolved


def prepare_owned_directory(path: Path, owned_root: Path, label: str) -> Path:
    resolved = resolve_owned_path(str(path), path, owned_root, label)
    if resolved.exists():
        expect(resolved.is_dir(), f"{label} exists but is not a directory: {resolved}")
        expect_no_reparse_points(resolved, label)
        shutil.rmtree(resolved)
    resolved.mkdir(parents=True, exist_ok=True)
    return resolved


def validate_run_id(raw_run_id: str, *, default_run_id: str) -> str:
    run_id = raw_run_id or default_run_id
    expect(
        SAFE_RUN_ID_RE.fullmatch(run_id) is not None,
        "sanitizer runtime evidence run id must be 1-128 chars of letters, digits, dot, underscore, or dash",
    )
    return run_id


def expect_default_relative_path(raw_path: str, expected_path: str, label: str) -> str:
    normalized = raw_path.replace("\\", "/")
    expect(normalized == expected_path, f"{label} is pinned to {expected_path}")
    return normalized


def package_relative_path(raw_path: object, label: str) -> str:
    normalized = str(raw_path or "").replace("\\", "/")
    path = PurePosixPath(normalized)
    expect(bool(normalized), f"{label} is required")
    expect(not path.is_absolute(), f"{label} must be package-relative: {normalized}")
    expect(".." not in path.parts and "." not in path.parts, f"{label} cannot contain traversal segments: {normalized}")
    expect(not (path.parts and ":" in path.parts[0]), f"{label} must not be drive-qualified: {normalized}")
    return path.as_posix()


def expected_runtime_library_artifacts(sanitizer_variant: str) -> set[str]:
    contract = runtime_package_variant_contract(sanitizer_variant)
    return {
        entry
        for entry in contract.runtime_library_payload_entries
        if entry != str(contract.runtime_library_manifest_path)
    }


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def pwsh_command() -> str:
    return shutil.which("pwsh") or shutil.which("powershell") or "pwsh"


def clang_command() -> str:
    configured = os.environ.get("OBJC3C_NATIVE_EXECUTION_CLANG_PATH")
    if configured:
        return configured
    discovered = find_llvm_tool_path("clang++")
    return str(discovered) if discovered else "clang++"


def runtime_dir(package_root: Path, sanitizer_variant: str) -> Path:
    leaf = "address" if sanitizer_variant == "address" else "undefined"
    path = package_root / "artifacts" / "runtime" / "sanitizer" / leaf
    expect(is_under_root(path, package_root), f"sanitizer runtime directory escaped package root: {path}")
    expect(path.is_dir(), f"missing sanitizer runtime directory: {path}")
    return path


def load_runtime_manifest(package_root: Path, sanitizer_variant: str) -> tuple[Path, dict[str, Any]]:
    contract = runtime_package_variant_contract(sanitizer_variant)
    manifest_path = package_root / str(contract.runtime_library_manifest_path).replace("/", os.sep)
    expect(manifest_path.is_file(), f"missing sanitizer runtime manifest: {manifest_path}")
    payload = load_json(manifest_path)
    expect(payload.get("contract_id") == "objc3c.sanitizer.runtime-library-manifest.v1", "runtime manifest contract drifted")
    expect(payload.get("sanitizer") == sanitizer_variant, "runtime manifest sanitizer drifted")
    expect(payload.get("target_platform_id") == WINDOWS_TARGET_PLATFORM, "runtime manifest platform drifted")
    expect(payload.get("runtime_library_ids") == list(contract.runtime_library_ids), "runtime manifest library ids drifted")
    expect(
        payload.get("missing_runtime_behavior") == "fail-closed-before-package-install",
        "runtime manifest missing-runtime behavior drifted",
    )
    expect(payload.get("support_truth") is False, "runtime manifest promoted support truth")
    expect(payload.get("native_execution_claimed") is False, "runtime manifest claimed native execution")
    artifacts = payload.get("runtime_library_artifacts")
    expect(isinstance(artifacts, list) and artifacts, "runtime manifest artifacts missing")
    expected_artifacts = expected_runtime_library_artifacts(sanitizer_variant)
    expected_runtime_dir = runtime_dir(package_root, sanitizer_variant).resolve()
    expected_runtime_library_id = "clang_rt.asan" if sanitizer_variant == "address" else "clang_rt.ubsan"
    seen_artifacts: set[str] = set()
    for artifact in artifacts:
        artifact_rel = package_relative_path(artifact.get("artifact"), "runtime manifest artifact")
        seen_artifacts.add(artifact_rel)
        expect(artifact_rel in expected_artifacts, f"runtime manifest unexpected artifact: {artifact_rel}")
        expect(
            artifact.get("runtime_library_id") == expected_runtime_library_id,
            f"runtime manifest library id drifted for {artifact_rel}",
        )
        artifact_path = package_root / artifact_rel.replace("/", os.sep)
        expect(
            artifact_path.resolve().parent == expected_runtime_dir,
            f"runtime artifact must live directly in packaged runtime link dir: {artifact_rel}",
        )
        expect(artifact_path.is_file(), f"runtime artifact missing: {artifact_path}")
        expect(artifact.get("sha256") == sha256_file(artifact_path), f"runtime artifact digest drifted: {artifact_path}")
    expect(seen_artifacts == expected_artifacts, "runtime manifest artifact set drifted from package contract")
    return manifest_path, payload


def sanitizer_environment(package_root: Path, sanitizer_variant: str) -> dict[str, str]:
    env = os.environ.copy()
    native_executable = package_root / "artifacts" / "bin" / "objc3c-native.exe"
    expect(native_executable.is_file(), f"packaged native executable missing: {native_executable}")
    env["OBJC3C_NATIVE_EXECUTION_SANITIZER_VARIANT"] = sanitizer_variant
    env["OBJC3C_NATIVE_EXECUTABLE"] = str(native_executable)
    env["PATH"] = str(runtime_dir(package_root, sanitizer_variant)) + os.pathsep + env.get("PATH", "")
    if sanitizer_variant == "address":
        env.setdefault("ASAN_OPTIONS", "detect_leaks=0:halt_on_error=1:symbolize=1")
    else:
        env.setdefault("UBSAN_OPTIONS", "halt_on_error=1:print_stacktrace=1")
    return env


def run_packaged_smoke(
    *,
    package_root: Path,
    sanitizer_variant: str,
    fixture_glob: str,
    run_id: str,
) -> dict[str, Any]:
    script = package_root / "scripts" / "check_objc3c_native_execution_smoke.ps1"
    expect(script.is_file(), f"packaged execution smoke script missing: {script}")
    env = sanitizer_environment(package_root, sanitizer_variant)
    env["OBJC3C_NATIVE_EXECUTION_RUN_ID"] = run_id
    command = [
        pwsh_command(),
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(script),
        "-FixtureGlob",
        fixture_glob,
        "-Limit",
        "1",
    ]
    result = run_completed(command, cwd=package_root, env=env, capture_output=True)
    summary_path = (
        package_root
        / "tmp"
        / "artifacts"
        / "objc3c-native"
        / "execution-smoke"
        / run_id
        / "summary.json"
    )
    expect(
        result.returncode == 0,
        "\n".join(
            [
                f"packaged sanitizer smoke failed: {command_text(command)}",
                output_tail(result),
            ]
        ),
    )
    expect(summary_path.is_file(), f"packaged sanitizer smoke summary missing: {summary_path}")
    summary = load_json(summary_path)
    expect(summary.get("status") == "PASS", "packaged sanitizer smoke summary did not pass")
    expect(summary.get("sanitizer_variant") == sanitizer_variant, "packaged smoke sanitizer variant drifted")
    expected_runtime_dir = f"artifacts/runtime/sanitizer/{sanitizer_variant}"
    expect(summary.get("sanitizer_runtime_dir") == expected_runtime_dir, "packaged smoke runtime link directory drifted")
    sanitizer_environment_payload = summary.get("sanitizer_environment", {})
    expect(isinstance(sanitizer_environment_payload, dict), "packaged smoke sanitizer environment missing")
    path_prepend = Path(str(sanitizer_environment_payload.get("PATH_PREPEND", ""))).resolve()
    expect(
        path_prepend == runtime_dir(package_root, sanitizer_variant).resolve(),
        "packaged smoke PATH did not prefer packaged sanitizer runtime directory",
    )
    return {
        "command": command,
        "exit_code": result.returncode,
        "summary_path": repo_rel(summary_path),
        "summary_status": summary.get("status"),
        "fixture_glob": fixture_glob,
        "selected_positive": summary.get("selection", {}).get("selected_positive"),
        "selected_negative": summary.get("selection", {}).get("selected_negative"),
    }


def windows_link_args() -> list[str]:
    if os.name != "nt":
        return []
    return [
        "-fms-runtime-lib=dll",
        "-fuse-ld=lld",
        "-Xlinker",
        "/MANIFEST:EMBED",
        "-Xlinker",
        "/MANIFESTUAC:level='asInvoker' uiAccess='false'",
    ]


def packaged_runtime_link_args(runtime_library_dir: Path) -> list[str]:
    if os.name == "nt":
        return [
            "-Xlinker",
            f"/LIBPATH:{runtime_library_dir}",
        ]
    return ["-L", str(runtime_library_dir)]


def detection_source(sanitizer_variant: str) -> str:
    if sanitizer_variant == "address":
        return r'''
#include <cstdlib>
int main() {
  int* value = new int[1];
  value[0] = 7;
  delete[] value;
  volatile int observed = value[0];
  return observed;
}
'''.strip()
    return r'''
#include <limits>
int main() {
  volatile int value = std::numeric_limits<int>::max();
  volatile int observed = value + 1;
  return observed;
}
'''.strip()


def run_detection_probe(
    *,
    package_root: Path,
    sanitizer_variant: str,
    work_dir: Path,
) -> dict[str, Any]:
    source_path = work_dir / f"{sanitizer_variant}_detection.cpp"
    exe_path = work_dir / f"{sanitizer_variant}_detection.exe"
    stdout_path = work_dir / f"{sanitizer_variant}_detection.stdout.txt"
    stderr_path = work_dir / f"{sanitizer_variant}_detection.stderr.txt"
    source_path.write_text(detection_source(sanitizer_variant) + "\n", encoding="utf-8")

    contract = runtime_package_variant_contract(sanitizer_variant)
    if sanitizer_variant == "address":
        sanitizer_flags = ["-fsanitize=address"]
    elif contract.trap_or_recover_mode == "trap":
        sanitizer_flags = [
            "-fsanitize=undefined",
            "-fsanitize-trap=undefined",
        ]
    else:
        sanitizer_flags = [
            "-fsanitize=undefined",
            "-fno-sanitize-recover=undefined",
        ]
    runtime_library_dir = runtime_dir(package_root, sanitizer_variant)
    command = [
        clang_command(),
        "-std=c++20",
        "-O0",
        *windows_link_args(),
        *packaged_runtime_link_args(runtime_library_dir),
        *sanitizer_flags,
        "-fno-omit-frame-pointer",
        str(source_path),
        "-o",
        str(exe_path),
        "-fno-color-diagnostics",
    ]
    env = sanitizer_environment(package_root, sanitizer_variant)
    compile_result = run_completed(command, cwd=ROOT, env=env, capture_output=True)
    expect(
        compile_result.returncode == 0,
        "\n".join(
            [
                f"sanitizer detection probe compile failed: {command_text(command)}",
                output_tail(compile_result),
            ]
        ),
    )
    expect(exe_path.is_file(), f"sanitizer detection executable missing: {exe_path}")

    run_result = run_completed([str(exe_path)], cwd=work_dir, env=env, capture_output=True)
    stdout_path.write_text(run_result.stdout or "", encoding="utf-8")
    stderr_path.write_text(run_result.stderr or "", encoding="utf-8")
    combined = f"{run_result.stdout or ''}\n{run_result.stderr or ''}"
    expect(run_result.returncode != 0, "sanitizer detection probe unexpectedly exited 0")
    expected_tokens = ["AddressSanitizer"] if sanitizer_variant == "address" else []
    if sanitizer_variant == "undefined" and contract.trap_or_recover_mode != "trap":
        expected_tokens = ["UndefinedBehaviorSanitizer", "runtime error"]
    if expected_tokens:
        expect(
            any(token in combined for token in expected_tokens),
            f"sanitizer detection output missing expected tokens: {expected_tokens}",
        )
    return {
        "command": command,
        "compile_exit_code": compile_result.returncode,
        "run_command": [str(exe_path)],
        "run_exit_code": run_result.returncode,
        "expected_detection": sanitizer_variant,
        "trap_or_recover_mode": contract.trap_or_recover_mode or "recover",
        "packaged_runtime_link_dir": repo_rel(runtime_library_dir),
        "packaged_runtime_link_args": packaged_runtime_link_args(runtime_library_dir),
        "stdout": repo_rel(stdout_path),
        "stderr": repo_rel(stderr_path),
        "source": repo_rel(source_path),
        "executable": repo_rel(exe_path),
    }


def build_probe_summary(args: argparse.Namespace) -> dict[str, Any]:
    sanitizer_variant = str(args.sanitizer_variant)
    package_root = resolve_owned_path(
        str(args.package_root),
        PACKAGE_ROOT_BASE / "missing-package-root",
        PACKAGE_ROOT_BASE,
        "sanitizer runtime evidence package root",
    )
    expect(package_root.is_dir(), f"package root missing: {package_root}")
    contract = runtime_package_variant_contract(sanitizer_variant)
    manifest_relative_path = expect_default_relative_path(
        str(args.manifest_relative_path),
        DEFAULT_MANIFEST_RELATIVE_PATH,
        "sanitizer runtime evidence manifest relative path",
    )
    fixture_glob = expect_default_relative_path(
        str(args.fixture_glob),
        DEFAULT_FIXTURE_GLOB,
        "sanitizer runtime evidence fixture glob",
    )
    manifest_path = package_root / manifest_relative_path.replace("/", os.sep)
    expect(manifest_path.is_file(), f"package manifest missing: {manifest_path}")
    manifest = load_json(manifest_path)
    if "support_truth" in manifest:
        expect(manifest.get("support_truth") is False, "package manifest support truth must remain false")
    if "native_execution_claimed" in manifest:
        expect(manifest.get("native_execution_claimed") is False, "package manifest native execution claim must remain false")
    expect(manifest.get("runtime_variant") == contract.runtime_variant, "package manifest runtime variant drifted")
    sanitizer_package = manifest.get("sanitizer_package_variant", {})
    expect(isinstance(sanitizer_package, dict), "package manifest missing sanitizer package variant")
    expect(sanitizer_package.get("package_id") == contract.package_id, "package id drifted")
    expect(sanitizer_package.get("package_channel_id") == contract.package_channel_id, "package channel drifted")
    expect(sanitizer_package.get("target_platform_id") == WINDOWS_TARGET_PLATFORM, "package target platform drifted")
    expect(
        sanitizer_package.get("runtime_library_manifest_path") == contract.runtime_library_manifest_path,
        "package runtime library manifest path drifted",
    )
    runtime_artifacts = sanitizer_package.get("runtime_library_artifacts")
    expect(isinstance(runtime_artifacts, list), "package runtime library artifacts missing")
    expect(
        {
            str(artifact.get("artifact", ""))
            for artifact in runtime_artifacts
            if isinstance(artifact, dict)
        }
        == expected_runtime_library_artifacts(sanitizer_variant),
        "package runtime library artifacts drifted",
    )
    expect(sanitizer_package.get("default_release_channel_allowed") is False, "package allowed default release channel fallback")
    expect(sanitizer_package.get("release_runtime_mixing_allowed") is False, "package allowed release/sanitizer runtime mixing")
    expect(sanitizer_package.get("support_truth") is False, "sanitizer package variant promoted support truth")
    expect(sanitizer_package.get("native_execution_claimed") is False, "sanitizer package variant claimed native execution")
    if contract.trap_or_recover_mode is not None:
        expect(
            sanitizer_package.get("trap_or_recover_mode") == contract.trap_or_recover_mode,
            "package UBSan trap/recover mode drifted",
        )

    runtime_manifest_path, runtime_manifest = load_runtime_manifest(package_root, sanitizer_variant)
    run_id = validate_run_id(
        str(args.run_id),
        default_run_id=f"sanitizer-runtime-evidence-{sanitizer_variant}",
    )
    work_dir = prepare_owned_directory(
        resolve_owned_path(
            str(args.work_dir),
            REPORT_ROOT / sanitizer_variant / "probe-work",
            REPORT_ROOT,
            "sanitizer runtime evidence probe work directory",
        ),
        REPORT_ROOT,
        "sanitizer runtime evidence probe work directory",
    )

    packaged_smoke = run_packaged_smoke(
        package_root=package_root,
        sanitizer_variant=sanitizer_variant,
        fixture_glob=fixture_glob,
        run_id=run_id,
    )
    detection = run_detection_probe(
        package_root=package_root,
        sanitizer_variant=sanitizer_variant,
        work_dir=work_dir,
    )
    now = datetime.now(timezone.utc).isoformat()
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "generated_at": now,
        "sanitizer_variant": sanitizer_variant,
        "target_platform_id": WINDOWS_TARGET_PLATFORM,
        "package_root": repo_rel(package_root),
        "package_manifest": repo_rel(manifest_path),
        "package_id": contract.package_id,
        "package_channel_id": contract.package_channel_id,
        "runtime_library_manifest": repo_rel(runtime_manifest_path),
        "runtime_library_manifest_digest": "sha256:" + sha256_file(runtime_manifest_path),
        "runtime_library_artifacts": runtime_manifest.get("runtime_library_artifacts", []),
        "support_truth": False,
        "native_execution_claimed": False,
        "support_promotion_allowed": False,
        "packaged_execution_smoke": packaged_smoke,
        "expected_detection_record": detection,
    }


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sanitizer-variant", choices=("address", "undefined"), required=True)
    parser.add_argument("--package-root", required=True)
    parser.add_argument(
        "--manifest-relative-path",
        default=DEFAULT_MANIFEST_RELATIVE_PATH,
    )
    parser.add_argument("--summary-out", default="")
    parser.add_argument("--fixture-glob", default=DEFAULT_FIXTURE_GLOB)
    parser.add_argument("--run-id", default="")
    parser.add_argument("--work-dir", default="")
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    summary = build_probe_summary(args)
    out = resolve_owned_path(
        str(args.summary_out),
        REPORT_ROOT / args.sanitizer_variant / "probe.json",
        REPORT_ROOT,
        "sanitizer runtime evidence probe summary",
    )
    write_json_file(out, summary)
    print(f"summary_path: {repo_rel(out)}")
    print("objc3c-sanitizer-runtime-evidence-probe: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
