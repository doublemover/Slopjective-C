"""Public native guard source/PCH execution matrix.

Implementation coverage is deliberately separate from feature admission. Callers
must bind compiler/runtime/input identities and hold the foundation lock before
using this helper; creating these fixtures does not qualify public guard syntax.
"""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "tests/clang_foundation/guard_bindings"
INPUTS = (Path(__file__).resolve(), FIXTURES / "runtime.h", FIXTURES / "main.m")


def qualify(run, directory: Path, compiler: Path, sdk: Path, report: dict,
            *, fixtures: Path | None = None) -> None:
    fixtures = fixtures or FIXTURES
    result = {
        "status": "running", "compiler_binding": "admitted-development-source",
        "profile_conformance_claimed": False,
        "configurations": [],
        "scope": ["ordered bindings and short-circuiting", "snapshot independence",
                  "let/var and source mutation", "guard/if lexical scope",
                  "loop transfers", "source defer ordering", "Block captures",
                  "GNU statement expressions", "C++ template replay",
                  "ARC object/Block snapshots", "weak source absence and Some(nil)"],
    }
    report["guard_source"] = result
    started_commands = len(report["commands"])
    for standard in ("c17", "c++20", "c++23"):
        language = "objective-c" if standard == "c17" else "objective-c++"
        for arc in (False, True):
            for optimization in ("O0", "O2"):
                common = [
                    compiler, "--driver-mode=g++", "--no-default-config",
                    "-std=" + standard, "-fobjc-version=3", "-fobjc-runtime=gnustep-2.2",
                    "-fexceptions", "-fobjc-exceptions", "-fobjc-arc-exceptions", "-fblocks",
                    "-fobjc-arc" if arc else "-fno-objc-arc", "-" + optimization,
                    "-g", "-fno-crash-diagnostics",
                ]
                for pch in (False, True):
                    name = f"guard-source-{standard}-{'arc' if arc else 'mrc'}-{optimization}-{'pch' if pch else 'ordinary'}"
                    executable = directory / "guard-application"
                    arguments = []
                    if pch:
                        cache = directory / "guard-runtime.pch"
                        run(name + "-header", [*common, "-x", language + "-header",
                                              fixtures / "runtime.h", "-o", cache])
                        arguments = ["-include-pch", cache, "-DGUARD_RUNTIME_PCH=1"]
                    run(name + "-compile", [
                        *common, "-x", language, *arguments, fixtures / "main.m",
                        "-fuse-ld=lld", "-no-pie", "-L" + str(sdk / "lib"), "-lobjc",
                        "-Wl,-rpath," + str(sdk / "lib"), "-o", executable,
                    ])
                    run(name + "-execute", [executable])
                    result["configurations"].append({
                        "standard": standard, "ownership": "ARC" if arc else "MRC",
                        "optimization": optimization, "input": "PCH" if pch else "ordinary",
                    })
    result["commands_passed"] = len(report["commands"]) - started_commands
    result["status"] = "PASS"
