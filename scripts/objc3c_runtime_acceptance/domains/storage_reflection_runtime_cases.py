"""Storage/reflection runtime acceptance case helpers."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    ROOT,
    RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PUBLIC_HEADER_PATH,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

def check_property_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-reflection"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "property_metadata_reflection_positive.objc3"
    obj_path = compile_fixture(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_property_metadata_reflection_probe.cpp"
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
    expect(token_property.get("setter_available") == 0, "expected readonly token property to have no setter")
    expect(token_property.get("has_runtime_getter") == 1, "expected token property getter to be runtime-backed")
    expect(value_property.get("found") == 1, "expected value property to be reflectable")
    expect(value_property.get("setter_available") == 1, "expected value property to expose a setter")
    expect(value_property.get("has_runtime_getter") == 1 and value_property.get("has_runtime_setter") == 1,
           "expected value property getter/setter to be runtime-backed")
    expect(count_property.get("found") == 1, "expected count property to be reflectable")
    expect(count_property.get("has_runtime_getter") == 1 and count_property.get("has_runtime_setter") == 1,
           "expected count property getter/setter to be runtime-backed")
    expect(registry_after_count.get("slot_backed_property_count", 0) >= 3,
           "expected slot-backed property registry to include the three Widget properties")
    expect(missing_property.get("found") == 0, "expected missing property lookup to fail closed")
    expect(missing_class_property.get("found") == 0, "expected missing class property lookup to fail closed")

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


def check_storage_ownership_reflection_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "storage-ownership-reflection"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "runtime_backed_storage_ownership_reflection_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_backed_storage_ownership_reflection_probe.cpp"
    exe_path = case_dir / "runtime_backed_storage_ownership_reflection_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "storage ownership reflection probe")

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    box_entry = payload.get("box_entry", {})
    implementation_surface = payload.get("implementation_surface", {})
    manifest_implementation_surface = manifest.get(
        "runtime_property_ivar_accessor_reflection_implementation_surface", {}
    )

    expect(box_entry.get("found") == 1, "expected Box to be realized for storage ownership reflection")
    expect(box_entry.get("runtime_property_accessor_count", 0) >= 5,
           "expected Box to publish five runtime-backed storage accessors")
    expect(box_entry.get("runtime_instance_size_bytes", 0) >= 40,
           "expected Box instance layout to reserve five object-backed storage slots")
    expect(registration_manifest.get("property_descriptor_count") == 10,
           "expected storage ownership fixture to publish ten property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 5,
           "expected storage ownership fixture to publish five ivar layout descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 10,
           "expected compile-output truthfulness to certify ten property descriptors")
    expect(registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 5,
           "expected compile-output truthfulness to certify five ivar descriptors")
    expect(
        "; runtime_backed_object_ownership_attribute_surface = "
        "contract=objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime-backed object ownership attribute surface",
    )
    expect("property_attribute_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten property-attribute profiles")
    expect("ownership_lifetime_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten ownership lifetime profiles")
    expect("ownership_runtime_hook_profiles=6" in ll_text,
           "expected LLVM IR ownership surface to publish six runtime hook profiles")
    expect("accessor_ownership_profiles=10" in ll_text,
           "expected LLVM IR ownership surface to publish ten accessor ownership profiles")
    expected_manifest_implementation_surface = {
        "contract_id": RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "runtime_property_ivar_storage_accessor_source_surface_contract_id": (
            RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID
        ),
        "storage_accessor_runtime_abi_surface_contract_id": (
            RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID
        ),
        "property_metadata_reflection_contract_id": (
            "objc3c.runtime.property.metadata.reflection.v1"
        ),
        "runtime_backed_object_ownership_attribute_surface_contract_id": (
            "objc3c.runtime.backed.object.ownership.attribute.surface.v1"
        ),
        "public_header_path": RUNTIME_PUBLIC_HEADER_PATH,
        "internal_header_path": RUNTIME_BOOTSTRAP_INTERNAL_HEADER_PATH,
        "implementation_snapshot_symbol": (
            "objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing"
        ),
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_manifest_implementation_surface.items():
        expect(
            manifest_implementation_surface.get(field) == expected_value,
            f"expected property/accessor runtime implementation surface to preserve {field}",
        )
    expect(
        manifest_implementation_surface.get("requires_coupled_registration_manifest")
        is True,
        "expected property/accessor runtime implementation surface to require the coupled runtime registration manifest",
    )
    expect(
        manifest_implementation_surface.get("requires_real_compile_output") is True,
        "expected property/accessor runtime implementation surface to require real compile output",
    )
    expect(
        manifest_implementation_surface.get("requires_linked_runtime_probe") is True,
        "expected property/accessor runtime implementation surface to require a linked runtime probe",
    )
    expected_implementation_surface = {
        "property_registry_ready": 1,
        "runtime_accessor_dispatch_ready": 1,
        "runtime_layout_ready": 1,
        "reflection_query_ready": 1,
        "deterministic": 1,
        "property_registry_state_snapshot_symbol": (
            "objc3_runtime_copy_property_registry_state_for_testing"
        ),
        "property_entry_snapshot_symbol": "objc3_runtime_copy_property_entry_for_testing",
        "current_property_read_symbol": "objc3_runtime_read_current_property_i32",
        "current_property_write_symbol": "objc3_runtime_write_current_property_i32",
        "current_property_exchange_symbol": (
            "objc3_runtime_exchange_current_property_i32"
        ),
        "bind_current_property_context_symbol": (
            "objc3_runtime_bind_current_property_context_for_testing"
        ),
        "clear_current_property_context_symbol": (
            "objc3_runtime_clear_current_property_context_for_testing"
        ),
        "weak_current_property_load_symbol": (
            "objc3_runtime_load_weak_current_property_i32"
        ),
        "weak_current_property_store_symbol": (
            "objc3_runtime_store_weak_current_property_i32"
        ),
        "implementation_model": (
            "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation"
        ),
        "reflection_model": (
            "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts"
        ),
        "fail_closed_model": (
            "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis"
        ),
    }
    for field, expected_value in expected_implementation_surface.items():
        expect(
            implementation_surface.get(field) == expected_value,
            f"expected live storage/accessor implementation snapshot to preserve {field}",
        )

    expected_properties = {
        "current_value_property": {
            "property_name": "currentValue",
            "slot_index": 0,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=1;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,strong",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=currentValue;setter_available=1;setter=setCurrentValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "currentValue",
            "effective_setter_selector": "setCurrentValue:",
        },
        "copied_value_property": {
            "property_name": "copiedValue",
            "slot_index": 1,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=copy,nonatomic",
            "ownership_lifetime_profile": "strong-owned",
            "ownership_runtime_hook_profile": None,
            "accessor_ownership_profile": "getter=copiedValue;setter_available=1;setter=setCopiedValue:;ownership_lifetime=strong-owned;runtime_hook=",
            "effective_getter_selector": "copiedValue",
            "effective_setter_selector": "setCopiedValue:",
        },
        "weak_value_property": {
            "property_name": "weakValue",
            "slot_index": 2,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;strong=0;weak=1;unowned=0;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=nonatomic,weak",
            "ownership_lifetime_profile": "weak",
            "ownership_runtime_hook_profile": "objc-weak-side-table",
            "accessor_ownership_profile": "getter=weakValue;setter_available=1;setter=setWeakValue:;ownership_lifetime=weak;runtime_hook=objc-weak-side-table",
            "effective_getter_selector": "weakValue",
            "effective_setter_selector": "setWeakValue:",
        },
        "borrowed_value_property": {
            "property_name": "borrowedValue",
            "slot_index": 3,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=1;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=assign",
            "ownership_lifetime_profile": "unowned-unsafe",
            "ownership_runtime_hook_profile": "objc-unowned-unsafe-direct",
            "accessor_ownership_profile": "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct",
            "effective_getter_selector": "borrowedValue",
            "effective_setter_selector": "setBorrowedValue:",
        },
        "guarded_value_property": {
            "property_name": "guardedValue",
            "slot_index": 4,
            "property_attribute_profile": "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;strong=0;weak=0;unowned=1;unsafe_unretained=0;assign=0;nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;attributes=unowned",
            "ownership_lifetime_profile": "unowned-safe",
            "ownership_runtime_hook_profile": "objc-unowned-safe-guard",
            "accessor_ownership_profile": "getter=guardedValue;setter_available=1;setter=setGuardedValue:;ownership_lifetime=unowned-safe;runtime_hook=objc-unowned-safe-guard",
            "effective_getter_selector": "guardedValue",
            "effective_setter_selector": "setGuardedValue:",
        },
    }

    for payload_key, expected in expected_properties.items():
        prop = payload.get(payload_key, {})
        expect(prop.get("found") == 1, f"expected {expected['property_name']} to be reflectable")
        expect(prop.get("has_runtime_getter") == 1 and prop.get("has_runtime_setter") == 1,
               f"expected {expected['property_name']} to execute through runtime-backed accessors")
        expect(prop.get("base_identity") == box_entry.get("base_identity"),
               f"expected {expected['property_name']} to share Box base identity")
        expect(prop.get("slot_index") == expected["slot_index"],
               f"expected {expected['property_name']} to keep slot index {expected['slot_index']}")
        expect(prop.get("size_bytes") == 8 and prop.get("alignment_bytes") == 8,
               f"expected {expected['property_name']} to preserve 8-byte object storage layout")
        expect(prop.get("property_name") == expected["property_name"],
               f"expected runtime property name for {expected['property_name']}")
        expect(prop.get("effective_getter_selector") == expected["effective_getter_selector"],
               f"expected getter selector for {expected['property_name']}")
        expect(prop.get("effective_setter_selector") == expected["effective_setter_selector"],
               f"expected setter selector for {expected['property_name']}")
        expect(prop.get("property_attribute_profile") == expected["property_attribute_profile"],
               f"expected property attribute profile for {expected['property_name']}")
        expect(prop.get("ownership_lifetime_profile") == expected["ownership_lifetime_profile"],
               f"expected ownership lifetime profile for {expected['property_name']}")
        expected_runtime_hook = expected["ownership_runtime_hook_profile"]
        if expected_runtime_hook is None:
            expect(prop.get("ownership_runtime_hook_profile") in (None, ""),
                   f"expected no runtime hook profile for {expected['property_name']}")
        else:
            expect(prop.get("ownership_runtime_hook_profile") == expected_runtime_hook,
                   f"expected runtime hook profile for {expected['property_name']}")
        expect(prop.get("accessor_ownership_profile") == expected["accessor_ownership_profile"],
               f"expected accessor ownership profile for {expected['property_name']}")
        expect(prop.get("getter_owner_identity"), f"expected getter owner identity for {expected['property_name']}")
        expect(prop.get("setter_owner_identity"), f"expected setter owner identity for {expected['property_name']}")

    return CaseResult(
        case_id="storage-ownership-reflection",
        probe="tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "runtime_property_accessor_count": box_entry.get("runtime_property_accessor_count"),
            "runtime_instance_size_bytes": box_entry.get("runtime_instance_size_bytes"),
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
            "ivar_descriptor_count": registration_manifest.get("ivar_descriptor_count"),
            "implementation_surface_contract_id": manifest_implementation_surface.get(
                "contract_id"
            ),
            "implementation_snapshot_symbol": manifest_implementation_surface.get(
                "implementation_snapshot_symbol"
            ),
            "guarded_runtime_hook_profile": payload.get("guarded_value_property", {}).get("ownership_runtime_hook_profile"),
            "weak_runtime_hook_profile": payload.get("weak_value_property", {}).get("ownership_runtime_hook_profile"),
        },
    )


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


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-layout"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, _ = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "property_layout_runtime_probe.cpp"
    exe_path = case_dir / "property_layout_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "property layout runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime property/layout consumption surface",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected property-layout fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected property-layout fixture to publish property descriptor inventory")
    expect("ivar_layout_owner_entries=" in ll_text, "expected property-layout fixture to publish ivar layout owner inventory")

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(second_alloc > 0, "expected second alloc to materialize a positive Widget instance identity")
    expect(
        first_alloc != second_alloc,
        "expected property-layout runtime to allocate distinct Widget instance identities",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected count getter to observe the written value on the first alloc")
    expect(
        payload.get("count_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance count storage",
    )
    expect(payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        payload.get("enabled_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance enabled storage",
    )
    expect(payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        payload.get("value_result_second") == 0,
        "expected second alloc to observe zero-filled per-instance strong value storage",
    )

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected property-layout runtime to report at least one registered image")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected property-layout runtime to report a non-zero descriptor total")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected property-layout runtime to materialize the synthesized accessor selector surface")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        str(count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:count"),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )

    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        str(set_count_entry.get("resolved_owner_identity", "")).endswith("implementation:Widget::instance_method:setCount:"),
        "expected setCount setter cache entry to preserve the synthesized owner identity",
    )

    return CaseResult(
        case_id="property-layout",
        probe="tests/tooling/runtime/property_layout_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "count_value_first": payload["count_value_first"],
            "count_value_second": payload["count_value_second"],
            "selector_table_entry_count": selector_state.get("selector_table_entry_count"),
        },
    )


def check_instance_allocation_layout_runtime_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "instance-allocation-layout-runtime"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "instance_allocation_runtime_probe.cpp"
    exe_path = case_dir / "instance_allocation_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "instance allocation runtime probe")

    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_state = payload.get("registration_state", {})
    selector_state = payload.get("selector_table_state", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    count_entry = payload.get("count_entry", {})
    set_count_entry = payload.get("set_count_entry", {})

    expect(
        "; runtime_instance_allocation_layout_support = "
        "contract=objc3c.runtime.instance.allocation.layout.support.v1"
        in ll_text,
        "expected LLVM IR to publish the runtime instance allocation/layout support surface",
    )
    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in ll_text,
        "expected LLVM IR to preserve the property/layout consumption surface coupled to instance allocation",
    )
    expect("synthesized_accessor_entries=6" in ll_text, "expected instance allocation fixture to preserve six synthesized accessors")
    expect("property_descriptor_entries=" in ll_text, "expected instance allocation fixture to publish property descriptors")
    expect("ivar_layout_owner_entries=" in ll_text, "expected instance allocation fixture to publish ivar layout owners")

    runtime_surface = manifest.get("runtime_property_ivar_storage_accessor_source_surface", {})
    expect(
        runtime_surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected compile manifest to keep the source layout model coupled to runtime allocation",
    )
    synthesized_surface = manifest.get("executable_synthesized_accessor_property_lowering_surface", {})
    expect(
        synthesized_surface.get("storage_model")
        == "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "expected synthesized accessor lowering to route storage through runtime helpers",
    )

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))
    expect(first_alloc == 1048576, "expected first runtime instance identity to start at 1048576")
    expect(second_alloc == 1048577, "expected second runtime instance identity to increment deterministically")
    expect(first_alloc != second_alloc, "expected alloc to materialize distinct receiver identities")

    expect(payload.get("set_count_first") == 0, "expected first count setter dispatch to return zero")
    expect(payload.get("count_value_first") == 37, "expected first count getter to read its written value")
    expect(payload.get("count_value_second_before") == 0, "expected second count getter to start from zero-filled storage")
    expect(payload.get("set_enabled_first") == 0, "expected first enabled setter dispatch to return zero")
    expect(payload.get("enabled_value_first") == 1, "expected first enabled getter to read its written value")
    expect(payload.get("enabled_value_second") == 0, "expected second enabled getter to start from zero-filled storage")
    expect(payload.get("set_value_first") == 0, "expected first strong value setter dispatch to return zero")
    expect(payload.get("value_result_first") == 55, "expected first strong value getter to read its retained slot value")
    expect(payload.get("value_result_second_before") == 0, "expected second strong value getter to start from nil/zero storage")
    expect(payload.get("set_count_second") == 0, "expected second count setter dispatch to return zero")
    expect(payload.get("count_value_first_after_second") == 37, "expected second count write not to affect the first instance")
    expect(payload.get("count_value_second_after") == 9, "expected second count getter to read its own written value")
    expect(payload.get("set_value_second") == 0, "expected second strong value setter dispatch to return zero")
    expect(payload.get("value_result_first_after_second") == 55, "expected second value write not to affect the first instance")
    expect(payload.get("value_result_second_after") == 91, "expected second value getter to read its own written value")

    expect(registration_state.get("registered_image_count", 0) >= 1, "expected registered image state for instance allocation probe")
    expect(registration_state.get("registered_descriptor_total", 0) >= 1, "expected registered descriptors for instance allocation probe")
    expect(selector_state.get("selector_table_entry_count", 0) >= 6, "expected synthesized accessor selectors in the selector table")
    expect(selector_state.get("metadata_backed_selector_count", 0) >= 6, "expected selector table to distinguish metadata-backed selectors")

    expect(graph_state.get("realized_class_count") == 1, "expected one realized Widget class")
    expect(graph_state.get("root_class_count") == 1, "expected Widget to be realized as a root class")
    expect(graph_state.get("receiver_class_binding_count") == 1, "expected one class receiver binding")
    expect(graph_state.get("live_instance_count") == 2, "expected two live runtime instances")
    expect(graph_state.get("last_allocated_receiver_identity") == second_alloc, "expected graph state to record the last allocated receiver")
    expect(graph_state.get("last_allocated_base_identity") == 1024, "expected graph state to record the Widget class base identity")
    expect(graph_state.get("last_allocated_instance_size_bytes") == 16, "expected Widget instance storage size to remain 16 bytes")
    expect(graph_state.get("last_allocated_class_name") == "Widget", "expected graph state to record the allocated class name")

    expect(widget_entry.get("found") == 1, "expected Widget realized class entry to be queryable")
    expect(widget_entry.get("base_identity") == 1024, "expected Widget base identity to remain deterministic")
    expect(widget_entry.get("is_root_class") == 1, "expected Widget fixture to be a root class")
    expect(widget_entry.get("implementation_backed") == 1, "expected Widget entry to be implementation backed")
    expect(widget_entry.get("runtime_property_accessor_count") == 3, "expected Widget to publish three runtime-backed property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 16, "expected Widget entry to publish 16 bytes of instance storage")
    expect(widget_entry.get("class_owner_identity") == "class:Widget", "expected Widget class owner identity")
    expect(widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "expected Widget metaclass owner identity")

    expect(count_entry.get("found") == 1 and count_entry.get("resolved") == 1, "expected count getter cache entry to resolve")
    expect(count_entry.get("dispatch_family_is_class") == 0, "expected count getter dispatch to be instance-family")
    expect(count_entry.get("normalized_receiver_identity") == 1025, "expected instance dispatch to normalize to Widget instance identity")
    expect(count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:count",
        "expected count getter cache entry to preserve synthesized owner identity",
    )
    expect(set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1, "expected setCount setter cache entry to resolve")
    expect(set_count_entry.get("dispatch_family_is_class") == 0, "expected setCount setter dispatch to be instance-family")
    expect(set_count_entry.get("normalized_receiver_identity") == 1025, "expected setter dispatch to normalize to Widget instance identity")
    expect(set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        set_count_entry.get("resolved_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected setCount setter cache entry to preserve synthesized owner identity",
    )

    return CaseResult(
        case_id="instance-allocation-layout-runtime",
        probe="tests/tooling/runtime/instance_allocation_runtime_probe.cpp",
        fixture="tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "allocation_mode": "distinct-instance-runtime-storage",
            "first_alloc": first_alloc,
            "second_alloc": second_alloc,
            "live_instance_count": graph_state.get("live_instance_count"),
            "instance_size_bytes": graph_state.get("last_allocated_instance_size_bytes"),
            "widget_property_accessor_count": widget_entry.get("runtime_property_accessor_count"),
            "count_first_after_second": payload["count_value_first_after_second"],
            "count_second_after": payload["count_value_second_after"],
        },
    )
