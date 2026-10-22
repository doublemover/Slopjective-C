"""Execute verified private condition AST modules from an admitted Linux build.

This gate does not admit guard source syntax or qualify an installed compiler.
It binds the development compiler, AST test binary, source seal and runtime,
then compiles the exact modules exported by the maintained CodeGen tests.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import resource
import signal
import subprocess
import tempfile
import time

from scripts.objc3c_toolchain.build import foundation_lock
from scripts.objc3c_toolchain.core import verify_development


ROOT = Path(__file__).resolve().parents[2]


def digest(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def cases() -> dict[str, dict]:
    result = {}
    for standard in ("c17", "c++20", "c++23"):
        for ownership in ("arc", "mrc"):
            for kind in ("guard", "if"):
                result[f"{standard}-{ownership}-{kind}"] = {
                    "standard": standard, "ownership": ownership,
                    "kind": kind, "payload": "plain",
                }
    for standard in ("c++20", "c++23"):
        for kind in ("guard", "if"):
            result[f"{standard}-cpp-{kind}"] = {
                "standard": standard, "ownership": "mrc", "kind": kind,
                "payload": "cpp", "expected_exception_scenarios": 5,
            }
    for name, facts in list(result.items()):
        ordered = name.rsplit("-", 1)
        result[ordered[0] + "-ordered-" + ordered[1]] = {
            **facts, "ordered_bindings": 2,
            "mixed_boolean_condition": facts["kind"] == "guard",
            **({"expected_exception_scenarios": 4} if facts["payload"] == "cpp" else {}),
        }
    return result


def qualify(repo: Path, build: Path, sdk: Path, output: Path) -> dict:
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / "runtime.json"
    report = {
        "schema": "objc3c.condition-ast-runtime.v1", "status": "running",
        "compiler_binding": "admitted-development-native-ast",
        "source_syntax_admitted": False, "installed_compiler_qualified": False,
        "started_utc": datetime.now(timezone.utc).isoformat(),
        "commands": [], "configurations": [],
    }
    started = time.monotonic()

    def persist():
        receipt.write_text(json.dumps(report, indent=2) + "\n")

    def run(name: str, command: list, *, environment=None):
        log = output / f"{name}.log"
        command_started = time.monotonic()

        def no_core():
            resource.setrlimit(resource.RLIMIT_CORE, (0, 0))

        timed_out = False
        with log.open("w") as stream:
            process = subprocess.Popen(
                list(map(str, command)), stdout=stream,
                stderr=subprocess.STDOUT, env=environment,
                start_new_session=True, preexec_fn=no_core,
            )
            try:
                code = process.wait(timeout=120)
            except subprocess.TimeoutExpired:
                timed_out = True
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                code = process.wait()
            except BaseException:
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                process.wait()
                raise
        report["commands"].append({
            "name": name, "exit_code": code, "timed_out": timed_out,
            "command": list(map(str, command)),
            "elapsed_seconds": round(time.monotonic() - command_started, 6),
            "log_sha256": digest(log),
        })
        persist()
        if code or timed_out:
            raise RuntimeError(f"{name} failed (exit {code}); see {log}")

    try:
        persist()  # Replace any earlier PASS before checking admission.
        with foundation_lock(repo):
            source = verify_development(repo)
            foundation = build / "foundation-build-report.json"
            admission = json.loads(foundation.read_text())
            if admission.get("status") != "passed":
                raise RuntimeError("the foundation build has not passed")
            if admission.get("source_identity") != source["build_id"]:
                raise RuntimeError("the foundation report describes different inputs")
            compiler = (build / "bin/clang").resolve(strict=True)
            tests = build / "tools/clang/unittests/AST/ObjC3TypeContractsTests"
            runtime = (sdk / "lib/libobjc.so").resolve(strict=True)
            inputs = {str(path): digest(path) for path in
                      (foundation, compiler, tests, runtime, sdk / "objc3-sdk.json",
                       Path(__file__).resolve())}
            report.update(
                source_build_id=source["build_id"],
                source_tree_sha256=source["tree_sha256"],
                compiler_sha256=inputs[str(compiler)],
                ast_tests_sha256=inputs[str(tests)],
                foundation_report_sha256=inputs[str(foundation)],
                runtime_library_sha256=inputs[str(runtime)], runtime_sdk_seal=sdk.name,
                qualification_script_sha256=inputs[str(Path(__file__).resolve())],
                executable_model="non-PIE private native AST modules",
            )
            with tempfile.TemporaryDirectory(prefix="objc3-condition-ast-") as temporary:
                directory = Path(temporary)
                environment = os.environ.copy()
                environment["OBJC3_CONDITION_TEST_IR_DIR"] = str(directory)
                run("native-ast-codegen", [tests, "--gtest_filter=ObjC3ConditionCodeGen.*"],
                    environment=environment)
                modules = {path.stem: path for path in directory.glob("*.ll")}
                expected = cases()
                if modules.keys() != expected.keys():
                    raise RuntimeError("AST gate did not export the exact expected module set")
                report["verified_modules"] = {
                    name: digest(path) for name, path in sorted(modules.items())
                }
                for name, module in sorted(modules.items()):
                    for optimization in ("O0", "O2"):
                        key = f"{name}-{optimization}"
                        executable = directory / "application"
                        run(key + "-compile", [
                            compiler, "--driver-mode=g++", "--no-default-config",
                            "-x", "ir", module, "-" + optimization, "-fuse-ld=lld", "-no-pie",
                            "-L" + str(sdk / "lib"), "-lobjc",
                            "-Wl,-rpath," + str(sdk / "lib"), "-o", executable,
                        ])
                        run(key + "-execute", [executable])
                        report["configurations"].append({
                            **expected[name], "optimization": optimization,
                            "module_sha256": report["verified_modules"][name],
                        })
                if any(digest(path) != report["verified_modules"][name]
                       for name, path in modules.items()):
                    raise RuntimeError("a verified module changed during execution")
            report["temporary_artifacts_reclaimed"] = True
            if any(digest(Path(path)) != value for path, value in inputs.items()):
                raise RuntimeError("a bound compiler, test or runtime input changed")
            if verify_development(repo) != source:
                raise RuntimeError("the development source seal changed")
            report["status"] = "PASS"
    except BaseException as error:
        report.update(status="FAIL", error=str(error))
        raise
    finally:
        report["elapsed_seconds"] = round(time.monotonic() - started, 6)
        persist()
    return report


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=ROOT)
    parser.add_argument("--build", type=Path)
    parser.add_argument("--sdk", type=Path)
    parser.add_argument("--output", type=Path)
    options = parser.parse_args()
    repo = options.repo.resolve()
    result = qualify(
        repo, (options.build or repo / "out/toolchains/objc3-development/build-linux-x86_64").resolve(),
        (options.sdk or repo / "out/toolchains/linux-sdk/current").resolve(),
        (options.output or repo / "out/native-validation/guard-bindings-ast").resolve(),
    )
    print(f"{result['status']}: {len(result['configurations'])} configurations, "
          f"{len(result['commands'])} commands")


if __name__ == "__main__":
    main()
