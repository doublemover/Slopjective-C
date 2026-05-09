"""Storage/reflection property metadata runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT


def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "property_metadata_reflection_positive.objc3"
    )
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = (
        ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "runtime_property_metadata_reflection_probe.cpp"
    )
    exe_path = case_dir / "runtime_property_metadata_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property reflection probe")

    widget_entry = payload.get("widget_entry", {})
    token_property = payload.get("token_property", {})
    value_property = payload.get("value_property", {})
    count_property = payload.get("count_property", {})
    missing_property = payload.get("missing_property", {})
    missing_class_property = payload.get("missing_class_property", {})
    registry_after_count = payload.get("registry_state_after_count", {})

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be present")
    expect(token_property.get("found") == 1, "expected token property to be reflectable")
    expect(
        token_property.get("setter_available") == 0,
        "expected readonly token property to have no setter",
    )
    expect(
        token_property.get("has_runtime_getter") == 1,
        "expected token property getter to be runtime-backed",
    )
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(
        value_property.get("has_runtime_getter") == 1
        and value_property.get("has_runtime_setter") == 1,
        "expected value property getter/setter to be runtime-backed",
    )
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(
        count_property.get("has_runtime_getter") == 1
        and count_property.get("has_runtime_setter") == 1,
        "expected count property getter/setter to be runtime-backed",
    )
    expect(
        registry_after_count.get("slot_backed_property_count", 0) >= 3,
        "expected slot-backed property registry to include the three Widget properties",
    )
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(
        missing_class_property.get("found") == 0,
        "expected missing class property lookup to fail closed",
    )

    return CaseResult(
        case_id="property-reflection",
        probe="tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_metadata_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "reflectable_property_count": registry_after_count.get("reflectable_property_count"),
            "slot_backed_property_count": registry_after_count.get("slot_backed_property_count"),
            "value_property_setter_available": value_property.get("setter_available"),
            "count_property_runtime_setter": count_property.get("has_runtime_setter"),
        },
    )


__all__ = ["check_property_reflection_case"]
