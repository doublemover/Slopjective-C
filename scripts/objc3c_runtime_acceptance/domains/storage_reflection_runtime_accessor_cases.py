"""Storage/reflection accessor runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from .storage_reflection_runtime_accessor_assertions import (
    assert_synthesized_accessor_runtime_payload,
)
from .storage_reflection_runtime_accessor_payload import (
    capture_synthesized_accessor_runtime_payload,
)
from .storage_reflection_runtime_accessor_summary import (
    build_synthesized_accessor_runtime_summary,
)


SYNTHESIZED_ACCESSOR_RUNTIME_CASE_ID = "synthesized-accessor-runtime"
SYNTHESIZED_ACCESSOR_RUNTIME_FIXTURE = (
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
)
SYNTHESIZED_ACCESSOR_RUNTIME_PROBE = (
    "tests/tooling/runtime/synthesized_accessor_probe.cpp"
)


def check_synthesized_accessor_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / SYNTHESIZED_ACCESSOR_RUNTIME_CASE_ID
    fixture = ROOT / SYNTHESIZED_ACCESSOR_RUNTIME_FIXTURE
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / SYNTHESIZED_ACCESSOR_RUNTIME_PROBE
    exe_path = case_dir / "synthesized_accessor_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "synthesized accessor runtime probe")
    facts = capture_synthesized_accessor_runtime_payload(payload)
    assert_synthesized_accessor_runtime_payload(facts)

    return CaseResult(
        case_id=SYNTHESIZED_ACCESSOR_RUNTIME_CASE_ID,
        probe=SYNTHESIZED_ACCESSOR_RUNTIME_PROBE,
        fixture=SYNTHESIZED_ACCESSOR_RUNTIME_FIXTURE,
        claim_class="linked-runtime-probe",
        passed=True,
        summary=build_synthesized_accessor_runtime_summary(facts),
    )


__all__ = ["check_synthesized_accessor_runtime_case"]
