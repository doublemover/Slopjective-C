"""Execute native condition AST and optional public source/PCH matrices.

Public source matrices are opt-in; this gate does not qualify an installed compiler.
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
from scripts.objc3c_native_validation import defer_source, guard_source, optional_access_source, coalescing_source


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
    for name, facts in list(result.items()):
        if facts["kind"] == "guard" and not facts.get("ordered_bindings"):
            result[name.removesuffix("guard") + "stmtexpr-guard"] = {
                **facts, "source_form": "GNU statement expression",
            }
    for name, facts in list(result.items()):
        if facts["standard"] != "c17" and facts["ownership"] == "mrc" and not facts.get("source_form"):
            if not facts.get("ordered_bindings") or facts["payload"] == "cpp":
                prefix, kind = name.rsplit("-", 1)
                result[prefix + "-template-" + kind] = {**facts, "template_replay": True}
    for standard in ("c17", "c++20", "c++23"):
        for payload in ("object", "block"):
            for source_ownership in ("strong", "weak", "unsafe_unretained"):
                for source_form in ("direct", "tagged"):
                    for binding in ("let", "var"):
                        for kind in ("guard", "if"):
                            name = f"{standard}-arc-{payload}-{source_ownership}-{source_form}-{binding}-{kind}"
                            result[name] = {
                                "standard": standard, "ownership": "arc", "kind": kind,
                                "payload": payload, "source_ownership": source_ownership,
                                "source_form": source_form, "binding": binding,
                                "expected_objc_exception_scenarios": 1,
                            }
    for standard in ("c17", "c++20", "c++23"):
        result[f"{standard}-arc-prefix-defer"] = {
            "standard": standard, "ownership": "arc", "kind": "guard",
            "payload": "object", "ordered_bindings": 2,
            "defer_prefix_failure": True, "expected_objc_exception_scenarios": 2,
        }
    for standard in ("c17", "c++20", "c++23"):
        result[f"{standard}-arc-if-defer"] = {
            "standard": standard, "ownership": "arc", "kind": "if",
            "payload": "object", "ordered_bindings": 2,
            "defer_prefix_failure": True, "expected_objc_exception_scenarios": 2,
        }
    return result


def qualify(repo: Path, build: Path, sdk: Path, output: Path, *, public_guard: bool = False,
            public_optional_access: bool = False, public_coalescing: bool = False) -> dict:
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / "runtime.json"
    report = {
        "schema": "objc3c.condition-ast-runtime.v3", "status": "running",
        "compiler_binding": "admitted-development-native-ast",
        "source_syntax_admitted": False, "installed_compiler_qualified": False,
        "requested_source_matrices": ["defer",
            *(["guard"] if public_guard else []),
            *(["optional-access"] if public_optional_access else []),
            *(["coalescing"] if public_coalescing else [])],
        "started_utc": datetime.now(timezone.utc).isoformat(),
        "commands": [], "configurations": [],
    }
    started = time.monotonic()

    def persist():
        receipt.write_text(json.dumps(report, indent=2) + "\n")

    def run(name: str, command: list, *, environment=None, expected_exit_code=0):
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
            "name": name, "exit_code": code, "expected_exit_code": expected_exit_code,
            "timed_out": timed_out,
            "command": list(map(str, command)),
            "elapsed_seconds": round(time.monotonic() - command_started, 6),
            "log_sha256": digest(log),
        })
        persist()
        if code != expected_exit_code or timed_out:
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
                       Path(__file__).resolve(), *defer_source.INPUTS,
                       *(guard_source.INPUTS if public_guard else ()),
                       *(optional_access_source.INPUTS if public_optional_access else ()),
                       *(coalescing_source.INPUTS if public_coalescing else ()))}
            report.update(
                source_build_id=source["build_id"],
                source_tree_sha256=source["tree_sha256"],
                compiler_sha256=inputs[str(compiler)],
                ast_tests_sha256=inputs[str(tests)],
                foundation_report_sha256=inputs[str(foundation)],
                runtime_library_sha256=inputs[str(runtime)], runtime_sdk_seal=sdk.name,
                qualification_script_sha256=inputs[str(Path(__file__).resolve())],
                defer_source_inputs_sha256={
                    str(path.relative_to(repo)) if path.is_relative_to(repo) else str(path): inputs[str(path)]
                    for path in defer_source.INPUTS
                },
                executable_model="non-PIE private native AST modules",
            )
            with tempfile.TemporaryDirectory(prefix="objc3-condition-ast-") as temporary:
                directory = Path(temporary)
                environment = os.environ.copy()
                environment["OBJC3_CONDITION_TEST_IR_DIR"] = str(directory)
                run("native-ast-codegen", [tests, "--gtest_filter=ObjC3ConditionCodeGen.*"],
                    environment=environment)
                modules = {path.stem: path for path in directory.glob("*.ll")}
                expected = {
                    f"{name}-{optimization}": {**facts, "optimization": optimization,
                                               "frontend_optimization": optimization}
                    for name, facts in cases().items()
                    for optimization in ("O0", "O2")
                }
                if modules.keys() != expected.keys():
                    raise RuntimeError("AST gate did not export the exact expected module set")
                report["verified_modules"] = {
                    name: digest(path) for name, path in sorted(modules.items())
                }
                for name, module in sorted(modules.items()):
                    optimization = expected[name]["optimization"]
                    executable = directory / "application"
                    run(name + "-compile", [
                        compiler, "--driver-mode=g++", "--no-default-config",
                        "-x", "ir", module, "-" + optimization, "-fno-crash-diagnostics",
                        "-fuse-ld=lld", "-no-pie",
                        "-L" + str(sdk / "lib"), "-lobjc",
                        "-Wl,-rpath," + str(sdk / "lib"), "-o", executable,
                    ])
                    run(name + "-execute", [executable])
                    report["configurations"].append({
                        **expected[name], "module_sha256": report["verified_modules"][name],
                    })
                defer_source.qualify(run, directory, compiler, sdk, report)
                if public_guard:
                    report["guard_source_inputs_sha256"] = {
                        str(path.relative_to(repo)): inputs[str(path)]
                        for path in guard_source.INPUTS
                    }
                    guard_source.qualify(run, directory, compiler, sdk, report)
                if public_optional_access:
                    report["optional_access_source_inputs_sha256"] = {
                        str(path.relative_to(repo)): inputs[str(path)]
                        for path in optional_access_source.INPUTS
                    }
                    optional_access_source.qualify(run, directory, compiler, sdk, report)
                if public_coalescing:
                    report["coalescing_source_inputs_sha256"] = {
                        str(path.relative_to(repo)): inputs[str(path)]
                        for path in coalescing_source.INPUTS
                    }
                    coalescing_source.qualify(run, directory, compiler, sdk, report)
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
    parser.add_argument("--public-guard", action="store_true",
                        help="also execute the public guard source/PCH implementation matrix")
    parser.add_argument("--public-optional-access", action="store_true",
                        help="also execute reference optional member/send source/PCH matrices")
    parser.add_argument("--public-coalescing", action="store_true",
                        help="execute scalar and C++ coalescing against the real GNUstep runtime")
    options = parser.parse_args()
    repo = options.repo.resolve()
    result = qualify(
        repo, (options.build or repo / "out/toolchains/objc3-development/build-linux-x86_64").resolve(),
        (options.sdk or repo / "out/toolchains/linux-sdk/current").resolve(),
        (options.output or repo / "out/native-validation/guard-bindings-ast").resolve(),
        public_guard=options.public_guard, public_optional_access=options.public_optional_access,
        public_coalescing=options.public_coalescing,
    )
    print(f"{result['status']}: {len(result['configurations'])} native AST configurations, "
          f"{sum(len(result.get(key, {}).get('configurations', [])) for key in ('defer_source', 'guard_source', 'optional_access_source', 'coalescing_source'))} source/PCH configurations, "
          f"{len(result['commands'])} commands")


if __name__ == "__main__":
    main()
