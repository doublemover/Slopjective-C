"""Source/PCH execution coverage for admitted deferred control flow.

This does not admit guard parsing, C++ cleanup CFGs or ARC release ordering.
"""
from __future__ import annotations

from pathlib import Path
import signal

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "tests/clang_foundation/defer_control_flow"
INPUTS = (Path(__file__).resolve(), FIXTURES / "runtime.h", FIXTURES / "main.m")


def qualify(run, directory: Path, compiler: Path, sdk: Path, report: dict) -> None:
    result = {
        "status": "running", "compiler_binding": "admitted-development-source",
        "defer_source_syntax_admitted": True, "guard_source_syntax_admitted": False,
        "arc_release_ordering_qualified": False, "configurations": [],
        "expected_objc_terminations": 0, "expected_cpp_terminations": 0,
    }
    report["defer_source"] = result
    started_commands = len(report["commands"])
    for standard in ("c17", "c++20", "c++23"):
        language = "objective-c" if standard == "c17" else "objective-c++"
        for arc in (False, True):
            for optimization in ("O0", "O2"):
                common = [
                    compiler, "--driver-mode=g++", "--no-default-config",
                    "-std=" + standard, "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
                    "-fexceptions", "-fobjc-exceptions", "-fobjc-arc-exceptions",
                    "-fobjc-arc" if arc else "-fno-objc-arc", "-" + optimization,
                    "-g", "-Wno-vla-cxx-extension", "-fno-crash-diagnostics",
                ]
                for pch in (False, True):
                    name = f"defer-source-{standard}-{'arc' if arc else 'mrc'}-{optimization}-{'pch' if pch else 'ordinary'}"
                    executable = directory / "defer-application"
                    arguments = []
                    if pch:
                        cache = directory / "defer-runtime.pch"
                        run(name + "-header", [*common, "-x", language + "-header",
                                              FIXTURES / "runtime.h", "-o", cache])
                        arguments = ["-include-pch", cache, "-DDEFER_RUNTIME_PCH=1"]
                    run(name + "-compile", [
                        *common, "-x", language, *arguments, FIXTURES / "main.m",
                        "-fuse-ld=lld", "-no-pie", "-L" + str(sdk / "lib"), "-lobjc",
                        "-Wl,-rpath," + str(sdk / "lib"), "-o", executable,
                    ])
                    run(name + "-execute", [executable])
                    run(name + "-objc-double-exception", [executable, "1"],
                        expected_exit_code=-signal.SIGABRT)
                    result["expected_objc_terminations"] += 1
                    if standard != "c17":
                        run(name + "-cpp-double-exception", [executable, "2"],
                            expected_exit_code=-signal.SIGABRT)
                        result["expected_cpp_terminations"] += 1
                    result["configurations"].append({
                        "standard": standard, "ownership": "ARC" if arc else "MRC",
                        "optimization": optimization, "input": "PCH" if pch else "ordinary",
                    })
    result["commands_passed"] = len(report["commands"]) - started_commands
    result["status"] = "PASS"
