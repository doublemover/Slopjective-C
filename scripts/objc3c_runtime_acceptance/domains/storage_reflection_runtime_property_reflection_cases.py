"""Storage/reflection property metadata runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from .storage_reflection_runtime_property_reflection_assertions import (
    assert_property_reflection_payload,
)
from .storage_reflection_runtime_property_reflection_payload import (
    capture_property_reflection_payload,
)
from .storage_reflection_runtime_property_reflection_summary import (
    build_property_reflection_summary,
)


PROPERTY_REFLECTION_CASE_ID = "property-reflection"
PROPERTY_REFLECTION_FIXTURE = (
    "tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3"
)
PROPERTY_REFLECTION_PROBE = (
    "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp"
)


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / PROPERTY_REFLECTION_CASE_ID
    fixture = ROOT / PROPERTY_REFLECTION_FIXTURE
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / PROPERTY_REFLECTION_PROBE
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")
    facts = capture_property_reflection_payload(payload)
    assert_property_reflection_payload(facts)

    return CaseResult(
        case_id=PROPERTY_REFLECTION_CASE_ID,
        probe=PROPERTY_REFLECTION_PROBE,
        fixture=PROPERTY_REFLECTION_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_property_reflection_summary(facts),
    )


__all__ = ["check_property_reflection_case"]
