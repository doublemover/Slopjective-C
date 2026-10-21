"""Compile and execute native fixture nodes against the sealed Linux SDK."""
from __future__ import annotations

from datetime import datetime, timezone
import json
import os
from pathlib import Path
import resource
import signal
import subprocess
import time


def run_linux_fixture_node(repo: Path, node: dict, output: Path, toolchain: dict) -> dict:
    from .fixture_nodes import _CASES
    suite, driver = node["suite"], node["driver"]
    if toolchain.get("platform") != "linux-x86_64" or driver not in {"clang", "objc3c-clang"}:
        raise ValueError("Linux fixture nodes require the sealed Linux clang or facade driver")
    if driver == "objc3c-clang" and suite not in {"match", "transfer"}:
        raise ValueError("Linux facade fixture driver is admitted for match and transfer suites")
    inputs = repo / "out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen/Inputs"
    output.mkdir(parents=True, exist_ok=True)
    report_path = output / "results.json"
    records, artifacts = [], {}
    report = {"status": "running", "platform": "linux-x86_64", "suite": suite,
              "client_driver": driver, "build_identity": toolchain["sdk_build_id"],
              "started_utc": datetime.now(timezone.utc).isoformat(), "commands": records}
    environment = dict(os.environ)
    environment.update(toolchain["environment"])
    timeout = float(node.get("timeout_seconds", 1200))
    base = ["--target=" + toolchain["target_triple"], "-fobjc-runtime=gnustep-2.0", "-fblocks",
            "-resource-dir", toolchain["resource_dir"], "-I", str(inputs)]
    for path in toolchain["runtime"]["include_dirs"]:
        base += ["-I", path]

    def persist() -> None:
        report_path.write_text(json.dumps(report, indent=2) + "\n")

    def no_core() -> None:
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))

    def execute(command: list[str], expected: int = 0) -> None:
        started = time.monotonic()
        result = subprocess.run(command, cwd=output, env=environment, text=True,
                                capture_output=True, timeout=timeout, preexec_fn=no_core)
        index = len(records)
        for channel, contents in (("stdout", result.stdout), ("stderr", result.stderr)):
            path = output / f"command-{index}.{channel}.log"
            path.write_text(contents)
            artifacts[f"command-{index}-{channel}"] = path
        records.append({"argv": command, "exit_code": result.returncode, "expected_exit_code": expected,
                        "elapsed_seconds": round(time.monotonic() - started, 6)})
        persist()
        if result.returncode != expected:
            raise RuntimeError(f"Linux {suite} command {index} exited {result.returncode}; expected {expected}; see {output}")

    def compile(source: Path, target: Path, language: str, opt: str, arc: bool = False, extension: bool = True) -> None:
        flags = base + ["-x", language, "-std=" + ("c++20" if "++" in language else "c17"),
                        "-O" + opt, "-fobjc-arc" if arc else "-fno-objc-arc"]
        if extension:
            flags += ["-fobjc-version=3"]
        if "++" in language:
            flags += ["-fexceptions", "-fcxx-exceptions", "-fobjc-arc-exceptions"]
        if driver == "objc3c-clang":
            command = [toolchain["tools"]["facade"], str(source), "--objc3", "--emit-object", str(target),
                       "--target", toolchain["target_triple"], "--runtime", "gnustep-2.0", "--", *flags]
            if "++" in language:
                command.insert(3, "--objcxx")
        else:
            command = [toolchain["tools"]["clang"], *flags, "-c", str(source), "-o", str(target)]
        execute(command)
        artifacts[target.name] = target

    def link(objects: list[Path], executable: Path) -> None:
        execute([toolchain["tools"]["clangxx"], "--driver-mode=g++", "-fuse-ld=lld",
                 "-B", str(Path(toolchain["tools"]["linker"]).parent),
                 *map(str, objects), toolchain["runtime"]["import_library"],
                 "-Wl,-rpath," + toolchain["runtime"]["lib_dir"], "-o", str(executable)])
        artifacts[executable.name] = executable

    count = 0
    try:
        persist()
        for opt in ("0", "2"):
            if suite == "typed-errors":
                support, mrc, main = [output / (name + "-O" + opt + ".o") for name in ("support", "mrc", "main")]
                compile(inputs / "typed-error-native-runtime-support.m", support, "objective-c", opt, extension=False)
                compile(inputs / "typed-error-mrc-runtime.mm", mrc, "objective-c++", opt)
                compile(inputs / "typed-error-out-runtime.mm", main, "objective-c++", opt, arc=True)
                executable = output / ("typed-errors-O" + opt)
                link([support, mrc, main], executable)
                execute([str(executable)])
                count += 1
                for trap in _CASES[suite][1:]:
                    execute([str(executable), trap.removeprefix("typed-error-").removesuffix("-trap")], expected=-signal.SIGILL)
                    count += 1
                continue
            for case in _CASES[suite]:
                executable = output / (case + "-O" + opt)
                objects = []
                if suite == "abi":
                    names = [("carrier-native-abi-runtime-provider.c", "objective-c"),
                             ("carrier-native-abi-runtime-main.c", "objective-c")] if case == "extension-provider-consumer" else [
                             ("carrier-native-abi-c-provider.c", "objective-c"), ("carrier-native-abi-c-client.c", "c")]
                elif suite == "arc-lifetime":
                    names = [("carrier-arc-local-runtime-support.m", "objective-c"),
                             ("carrier-arc-local-lifetime-runtime.mm", "objective-c++")]
                elif suite in {"match", "transfer"}:
                    names = [(str(repo / "tests/clang_foundation" / (case + (".mm" if suite == "match" or case.endswith("local-runtime") else ".cpp"))), "objective-c++")]
                else:
                    names = [(case + (".cpp" if suite == "cpp-lifetime" else ".c"),
                              "objective-c++" if suite == "cpp-lifetime" else "objective-c")]
                for index, (filename, language) in enumerate(names):
                    source = Path(filename) if Path(filename).is_absolute() else inputs / filename
                    obj = output / (case + "-O" + opt + "-" + str(index) + ".o")
                    compile(source, obj, language, opt, arc=suite == "arc-lifetime" and index == 1,
                            extension=language != "c" and not (suite == "arc-lifetime" and index == 0))
                    objects.append(obj)
                link(objects, executable)
                execute([str(executable)])
                count += 1
        report.update(status="passed", runtime_count=count)
    except Exception as error:
        report.update(status="failed", error=str(error))
        raise
    finally:
        report["finished_utc"] = datetime.now(timezone.utc).isoformat()
        persist()
    artifacts["report"] = report_path
    return {"artifacts": artifacts, "data": {"suite": suite, "driver": driver,
            "runtime_count": count, "count": count, "report_path": str(report_path),
            "build_identity": toolchain["sdk_build_id"], "exit_code": 0}}
