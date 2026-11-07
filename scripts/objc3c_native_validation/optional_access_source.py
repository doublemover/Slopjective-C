"""Execute public optional member/send source and PCH witnesses.

This matrix records implementation evidence without promoting feature claims.
Callers bind compiler/runtime/input identities and hold the foundation lock.
"""
from __future__ import annotations
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "tests/clang_foundation/optional_access"
INPUTS = (Path(__file__).resolve(), FIXTURES / "runtime.h", FIXTURES / "main.m")


def qualify(run, directory: Path, compiler: Path, sdk: Path, report: dict,
            *, fixtures: Path | None = None) -> None:
    fixtures = fixtures or FIXTURES
    result = {"status": "running", "profile_conformance_claimed": False,
              "configurations": [],
              "scope": ["single receiver evaluation", "lazy message arguments",
                        "nil object/Block and void results", "chained getters",
                        "nullable coalescing", "ordinary conditional receivers",
                        "C++ template replay", "ARC weak snapshot ownership"]}
    report["optional_access_source"] = result
    started = len(report["commands"])
    for standard in ("c17", "c++20", "c++23"):
        language = "objective-c" if standard == "c17" else "objective-c++"
        for arc in (False, True):
            for optimization in ("O0", "O2"):
                common = [compiler, "--driver-mode=g++", "--no-default-config",
                          "-std=" + standard, "-fobjc-version=3",
                          "-fobjc-runtime=gnustep-2.2", "-fexceptions",
                          "-fobjc-exceptions", "-fobjc-arc-exceptions", "-fblocks",
                          "-fobjc-arc" if arc else "-fno-objc-arc",
                          "-" + optimization, "-g", "-fno-crash-diagnostics"]
                for pch in (False, True):
                    name = f"optional-access-{standard}-{'arc' if arc else 'mrc'}-{optimization}-{'pch' if pch else 'source'}"
                    executable = directory / "optional-access-application"
                    arguments = []
                    if pch:
                        cache = directory / "optional-access.pch"
                        run(name + "-header", [*common, "-x", language + "-header",
                                              fixtures / "runtime.h", "-o", cache])
                        arguments = ["-include-pch", cache,
                                     "-DOPTIONAL_ACCESS_RUNTIME_PCH=1"]
                    run(name + "-compile", [*common, "-x", language, *arguments,
                        fixtures / "main.m", "-fuse-ld=lld", "-no-pie",
                        "-L" + str(sdk / "lib"), "-lobjc",
                        "-Wl,-rpath," + str(sdk / "lib"), "-o", executable])
                    run(name + "-execute", [executable])
                    result["configurations"].append({"standard": standard,
                        "ownership": "ARC" if arc else "MRC",
                        "optimization": optimization,
                        "input": "PCH" if pch else "ordinary"})
    result["commands_passed"] = len(report["commands"]) - started
    result["status"] = "PASS"
