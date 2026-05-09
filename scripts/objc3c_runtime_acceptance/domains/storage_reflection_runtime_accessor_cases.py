"""Storage/reflection accessor and layout runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT

def check_synthesized_accessor_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "synthesized-accessor-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "synthesized_accessor_probe.cpp"
    exe_path = case_dir / "synthesized_accessor_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "synthesized accessor runtime probe")

    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})
    enabled_entry = payload.get("enabled_entry", {})
    set_enabled_entry = payload.get("set_enabled_entry", {})
    value_entry = payload.get("value_entry", {})
    set_value_entry = payload.get("set_value_entry", {})

    expect(payload.get("widget_instance", 0) > 0, "expected synthesized-accessor runtime probe to allocate a positive Widget receiver")
    expect(payload.get("set_count_result") == 0, "expected synthesized-accessor count setter dispatch to return zero")
    expect(payload.get("count_value") == 37, "expected synthesized-accessor count getter to reload 37")
    expect(payload.get("set_enabled_result") == 0, "expected synthesized-accessor enabled setter dispatch to return zero")
    expect(payload.get("enabled_value") == 1, "expected synthesized-accessor enabled getter to reload 1")
    expect(payload.get("set_value_result") == 0, "expected synthesized-accessor value setter dispatch to return zero")
    expect(payload.get("value_result") == 55, "expected synthesized-accessor value getter to reload 55")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected synthesized-accessor runtime probe to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected synthesized-accessor runtime probe to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized-accessor runtime probe to materialize the accessor selector surface")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected synthesized-accessor runtime probe to preserve metadata-backed selectors")

    expected_entries = (
        (count_entry, "count", 0, "implementation:Widget::instance_method:count"),
        (set_count_entry, "setCount:", 1, "implementation:Widget::instance_method:setCount:"),
        (enabled_entry, "enabled", 0, "implementation:Widget::instance_method:enabled"),
        (set_enabled_entry, "setEnabled:", 1, "implementation:Widget::instance_method:setEnabled:"),
        (value_entry, "value", 0, "implementation:Widget::instance_method:value"),
        (set_value_entry, "setValue:", 1, "implementation:Widget::instance_method:setValue:"),
    )
    for entry, selector, parameter_count, owner_identity in expected_entries:
        expect(entry.get("found") == 1 and entry.get("resolved") == 1, f"expected {selector} cache entry to resolve live")
        expect(entry.get("selector") == selector, f"expected {selector} cache entry to preserve selector spelling")
        expect(entry.get("parameter_count") == parameter_count, f"expected {selector} cache entry to preserve parameter count {parameter_count}")
        expect(entry.get("resolved_class_name") == "Widget", f"expected {selector} cache entry to resolve against Widget")
        expect(entry.get("resolved_owner_identity") == owner_identity, f"expected {selector} cache entry to preserve owner identity")

    return CaseResult(
        case_id="synthesized-accessor-runtime",
        probe="tests/tooling/runtime/synthesized_accessor_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_instance": payload["widget_instance"],
            "count_value": payload["count_value"],
            "enabled_value": payload["enabled_value"],
            "value_result": payload["value_result"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


__all__ = ["check_synthesized_accessor_runtime_case"]
