"""Malformed property/ivar layout runtime acceptance case."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from .storage_reflection_runtime_property_invalid_layout_assertions import (
    assert_property_invalid_layout_payload,
)
from .storage_reflection_runtime_property_invalid_layout_payload import (
    capture_property_invalid_layout_payload,
)
from .storage_reflection_runtime_property_invalid_layout_summary import (
    PROPERTY_INVALID_LAYOUT_CASE_ID,
    build_property_invalid_layout_summary,
)


PROPERTY_INVALID_LAYOUT_PROBE = (
    "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp"
)


def check_property_invalid_layout_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / PROPERTY_INVALID_LAYOUT_CASE_ID
    probe = ROOT / PROPERTY_INVALID_LAYOUT_PROBE
    exe_path = case_dir / "property_ivar_invalid_layout_probe.exe"
    compile_probe(clangxx, probe, exe_path, [])
    payload = parse_json_output(
        run_probe(exe_path), "property ivar invalid layout probe"
    )
    facts = capture_property_invalid_layout_payload(payload)
    assert_property_invalid_layout_payload(facts)

    return CaseResult(
        case_id=PROPERTY_INVALID_LAYOUT_CASE_ID,
        probe=PROPERTY_INVALID_LAYOUT_PROBE,
        fixture=None,
        claim_class="linked-runtime-negative-probe",
        passed=True,
        summary=build_property_invalid_layout_summary(facts),
    )


__all__ = ["check_property_invalid_layout_runtime_case"]
