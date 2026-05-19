"""Object Model standalone runtime-library acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import run_probe

from ..paths import ROOT

_EXPORTED_CASE_NAMES = ["check_runtime_library_case"]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_runtime_library_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "runtime-library"
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_library_probe.cpp"
    exe_path = case_dir / "runtime_library_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    run_probe(exe_path)
    dispatch_expectations_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "dispatch_expectations_support_test.cpp"
    )
    dispatch_expectations_exe = case_dir / "dispatch_expectations_support_test.exe"
    compile_probe(clangxx, dispatch_expectations_probe, dispatch_expectations_exe, [])
    run_probe(dispatch_expectations_exe)
    strict_dispatch_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "strict_dispatch_error_status_probe.cpp"
    )
    strict_dispatch_exe = case_dir / "strict_dispatch_error_status_probe.exe"
    compile_probe(clangxx, strict_dispatch_probe, strict_dispatch_exe, [])
    run_probe(strict_dispatch_exe)
    typed_dispatch_probe = (
        ROOT / "tests" / "tooling" / "runtime" / "typed_dispatch_abi_probe.cpp"
    )
    typed_dispatch_exe = case_dir / "typed_dispatch_abi_probe.exe"
    compile_probe(clangxx, typed_dispatch_probe, typed_dispatch_exe, [])
    run_probe(typed_dispatch_exe)
    return CaseResult(
        case_id="runtime-library",
        probe="tests/tooling/runtime/runtime_library_probe.cpp",
        fixture=None,
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "kind": "standalone-runtime-probe",
            "dispatch_expectations_drift_probe": (
                "tests/tooling/runtime/dispatch_expectations_support_test.cpp"
            ),
            "strict_dispatch_error_status_probe": (
                "tests/tooling/runtime/strict_dispatch_error_status_probe.cpp"
            ),
            "typed_dispatch_abi_probe": (
                "tests/tooling/runtime/typed_dispatch_abi_probe.cpp"
            ),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
