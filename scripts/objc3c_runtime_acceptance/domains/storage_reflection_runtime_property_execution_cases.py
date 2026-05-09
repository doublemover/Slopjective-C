"""Storage/reflection property execution runtime acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT


def check_property_execution_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-execution"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_ivar_execution_matrix_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_ivar_execution_matrix_probe.cpp"
    exe_path = case_dir / "property_ivar_execution_matrix_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property execution probe")

    widget_entry = payload.get("widget_entry", {})
    registry_state = payload.get("registry_state", {})
    count_property = payload.get("count_property", {})
    enabled_property = payload.get("enabled_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})
    count_method = payload.get("count_method", {})
    enabled_method = payload.get("enabled_method", {})
    value_method = payload.get("value_method", {})
    token_method = payload.get("token_method", {})
    set_count_dispatch = payload.get("set_count_dispatch", {})
    count_dispatch = payload.get("count_dispatch", {})
    set_enabled_dispatch = payload.get("set_enabled_dispatch", {})
    enabled_dispatch = payload.get("enabled_dispatch", {})
    set_value_dispatch = payload.get("set_value_dispatch", {})
    value_dispatch = payload.get("value_dispatch", {})
    token_dispatch = payload.get("token_dispatch", {})

    expect(payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(enabled_property.get("has_runtime_getter") == 1 and enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(token_property.get("has_runtime_getter") == 1 and token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")
    expect(count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")
    expect(count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(count_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(enabled_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(value_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(token_property.get("base_identity") == widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")
    expect(registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")
    expect(count_method.get("resolved") == 1 and count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(enabled_method.get("resolved") == 1 and enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(value_method.get("resolved") == 1 and value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(token_method.get("resolved") == 1 and token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(count_method.get("resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(enabled_method.get("resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(value_method.get("resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(token_method.get("resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")
    expect(set_count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected setCount: to execute through live synthesized accessor resolution")
    expect(set_count_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCount: to execute through the runtime property-setter builtin")
    expect(set_count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected setCount: dispatch property name to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected setCount: dispatch base identity to match reflected property metadata")
    expect(set_count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected setCount: dispatch slot index to match reflected property metadata")
    expect(set_count_dispatch.get("last_selector") == count_property.get("effective_setter_selector"),
           "expected setCount: dispatch selector to match reflected property metadata")
    expect(set_count_dispatch.get("last_resolved_owner_identity") == count_property.get("setter_owner_identity"),
           "expected setCount: dispatch ownership to match reflected property metadata")
    expect(set_count_dispatch.get("last_used_builtin") == 1 and set_count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected setCount: to remain builtin-backed and runtime-dispatched")
    expect(set_count_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCount: dispatch to report one setter parameter")
    expect(count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected count getter to execute through live synthesized accessor resolution")
    expect(count_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected count getter to execute through the runtime property-getter builtin")
    expect(count_dispatch.get("last_property_name") == count_property.get("property_name"),
           "expected count getter dispatch property name to match reflected property metadata")
    expect(count_dispatch.get("last_property_base_identity") == count_property.get("base_identity"),
           "expected count getter dispatch base identity to match reflected property metadata")
    expect(count_dispatch.get("last_property_slot_index") == count_property.get("slot_index"),
           "expected count getter dispatch slot index to match reflected property metadata")
    expect(count_dispatch.get("last_selector") == count_property.get("effective_getter_selector"),
           "expected count getter dispatch selector to match reflected property metadata")
    expect(count_dispatch.get("last_resolved_owner_identity") == count_property.get("getter_owner_identity"),
           "expected count getter dispatch ownership to match reflected property metadata")
    expect(count_dispatch.get("last_used_builtin") == 1 and count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected count getter to remain builtin-backed and runtime-dispatched")
    expect(count_dispatch.get("last_resolved_parameter_count") == 0,
           "expected count getter dispatch to report zero getter parameters")
    expect(set_enabled_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setEnabled: to execute through the runtime property-setter builtin")
    expect(set_enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected setEnabled: dispatch property name to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected setEnabled: dispatch base identity to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected setEnabled: dispatch slot index to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_selector") == enabled_property.get("effective_setter_selector"),
           "expected setEnabled: dispatch selector to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("setter_owner_identity"),
           "expected setEnabled: dispatch ownership to match reflected property metadata")
    expect(set_enabled_dispatch.get("last_used_builtin") == 1 and set_enabled_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setEnabled: to remain builtin-backed and report one setter parameter")
    expect(enabled_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected enabled getter to execute through the runtime property-getter builtin")
    expect(enabled_dispatch.get("last_property_name") == enabled_property.get("property_name"),
           "expected enabled getter dispatch property name to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_base_identity") == enabled_property.get("base_identity"),
           "expected enabled getter dispatch base identity to match reflected property metadata")
    expect(enabled_dispatch.get("last_property_slot_index") == enabled_property.get("slot_index"),
           "expected enabled getter dispatch slot index to match reflected property metadata")
    expect(enabled_dispatch.get("last_selector") == enabled_property.get("effective_getter_selector"),
           "expected enabled getter dispatch selector to match reflected property metadata")
    expect(enabled_dispatch.get("last_resolved_owner_identity") == enabled_property.get("getter_owner_identity"),
           "expected enabled getter dispatch ownership to match reflected property metadata")
    expect(enabled_dispatch.get("last_used_builtin") == 1 and enabled_dispatch.get("last_resolved_parameter_count") == 0,
           "expected enabled getter to remain builtin-backed and report zero getter parameters")
    expect(set_value_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCurrentValue: to execute through the runtime property-setter builtin")
    expect(set_value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected setCurrentValue: dispatch property name to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected setCurrentValue: dispatch base identity to match reflected property metadata")
    expect(set_value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected setCurrentValue: dispatch slot index to match reflected property metadata")
    expect(set_value_dispatch.get("last_selector") == value_property.get("effective_setter_selector"),
           "expected setCurrentValue: dispatch selector to match reflected property metadata")
    expect(set_value_dispatch.get("last_resolved_owner_identity") == value_property.get("setter_owner_identity"),
           "expected setCurrentValue: dispatch ownership to match reflected property metadata")
    expect(set_value_dispatch.get("last_used_builtin") == 1 and set_value_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCurrentValue: to remain builtin-backed and report one setter parameter")
    expect(value_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected currentValue getter to execute through the runtime property-getter builtin")
    expect(value_dispatch.get("last_property_name") == value_property.get("property_name"),
           "expected currentValue getter dispatch property name to match reflected property metadata")
    expect(value_dispatch.get("last_property_base_identity") == value_property.get("base_identity"),
           "expected currentValue getter dispatch base identity to match reflected property metadata")
    expect(value_dispatch.get("last_property_slot_index") == value_property.get("slot_index"),
           "expected currentValue getter dispatch slot index to match reflected property metadata")
    expect(value_dispatch.get("last_selector") == value_property.get("effective_getter_selector"),
           "expected currentValue getter dispatch selector to match reflected property metadata")
    expect(value_dispatch.get("last_resolved_owner_identity") == value_property.get("getter_owner_identity"),
           "expected currentValue getter dispatch ownership to match reflected property metadata")
    expect(value_dispatch.get("last_used_builtin") == 1 and value_dispatch.get("last_resolved_parameter_count") == 0,
           "expected currentValue getter to remain builtin-backed and report zero getter parameters")
    expect(token_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected tokenValue getter to execute through the runtime property-getter builtin")
    expect(token_dispatch.get("last_property_name") == token_property.get("property_name"),
           "expected tokenValue getter dispatch property name to match reflected property metadata")
    expect(token_dispatch.get("last_property_base_identity") == token_property.get("base_identity"),
           "expected tokenValue getter dispatch base identity to match reflected property metadata")
    expect(token_dispatch.get("last_property_slot_index") == token_property.get("slot_index"),
           "expected tokenValue getter dispatch slot index to match reflected property metadata")
    expect(token_dispatch.get("last_selector") == token_property.get("effective_getter_selector"),
           "expected tokenValue getter dispatch selector to match reflected property metadata")
    expect(token_dispatch.get("last_resolved_owner_identity") == token_property.get("getter_owner_identity"),
           "expected tokenValue getter dispatch ownership to match reflected property metadata")
    expect(token_dispatch.get("last_used_builtin") == 1 and token_dispatch.get("last_resolved_parameter_count") == 0,
           "expected tokenValue getter to remain builtin-backed and report zero getter parameters")
    return CaseResult(
        case_id="property-execution",
        probe="tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
        fixture="tests/tooling/fixtures/native/property_ivar_execution_matrix_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "count_value": payload.get("count_value"),
            "enabled_value": payload.get("enabled_value"),
            "value_result": payload.get("value_result"),
            "runtime_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "slot_backed_property_count": registry_state.get("slot_backed_property_count"),
            "count_dispatch_kind": count_dispatch.get("last_implementation_kind"),
            "value_dispatch_kind": value_dispatch.get("last_implementation_kind"),
            "token_dispatch_kind": token_dispatch.get("last_implementation_kind"),
        },
    )


__all__ = ["check_property_execution_case"]


