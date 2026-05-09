"""Storage/reflection property layout and instance allocation runtime cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import compile_fixture_outputs
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import ROOT


def check_property_layout_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "property-layout"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
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
    expect(
        "synthesized_accessor_entries=6" in ll_text,
        "expected property-layout fixture to preserve six synthesized accessors",
    )
    expect(
        "property_descriptor_entries=" in ll_text,
        "expected property-layout fixture to publish property descriptor inventory",
    )
    expect(
        "ivar_layout_owner_entries=" in ll_text,
        "expected property-layout fixture to publish ivar layout owner inventory",
    )

    first_alloc = int(payload.get("first_alloc", 0))
    second_alloc = int(payload.get("second_alloc", 0))

    expect(first_alloc > 0, "expected first alloc to materialize a positive Widget instance identity")
    expect(second_alloc > 0, "expected second alloc to materialize a positive Widget instance identity")
    expect(
        first_alloc != second_alloc,
        "expected property-layout runtime to allocate distinct Widget instance identities",
    )
    expect(payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(
        payload.get("count_value_first") == 37,
        "expected count getter to observe the written value on the first alloc",
    )
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

    expect(
        registration_state.get("registered_image_count", 0) >= 1,
        "expected property-layout runtime to report at least one registered image",
    )
    expect(
        registration_state.get("registered_descriptor_total", 0) >= 1,
        "expected property-layout runtime to report a non-zero descriptor total",
    )
    expect(
        selector_state.get("selector_table_entry_count", 0) >= 6,
        "expected property-layout runtime to materialize the synthesized accessor selector surface",
    )

    expect(
        count_entry.get("found") == 1 and count_entry.get("resolved") == 1,
        "expected count getter cache entry to resolve",
    )
    expect(
        count_entry.get("parameter_count") == 0,
        "expected count getter cache entry to preserve zero parameters",
    )
    expect(
        str(count_entry.get("resolved_owner_identity", "")).endswith(
            "implementation:Widget::instance_method:count"
        ),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )

    expect(
        set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1,
        "expected setCount setter cache entry to resolve",
    )
    expect(
        set_count_entry.get("parameter_count") == 1,
        "expected setCount setter cache entry to preserve one parameter",
    )
    expect(
        str(set_count_entry.get("resolved_owner_identity", "")).endswith(
            "implementation:Widget::instance_method:setCount:"
        ),
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


def check_instance_allocation_layout_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "instance-allocation-layout-runtime"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "synthesized_accessor_property_lowering_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        fixture, case_dir / "compile"
    )
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
    expect(
        "synthesized_accessor_entries=6" in ll_text,
        "expected instance allocation fixture to preserve six synthesized accessors",
    )
    expect(
        "property_descriptor_entries=" in ll_text,
        "expected instance allocation fixture to publish property descriptors",
    )
    expect(
        "ivar_layout_owner_entries=" in ll_text,
        "expected instance allocation fixture to publish ivar layout owners",
    )

    runtime_surface = manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface", {}
    )
    expect(
        runtime_surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected compile manifest to keep the source layout model coupled to runtime allocation",
    )
    synthesized_surface = manifest.get(
        "executable_synthesized_accessor_property_lowering_surface", {}
    )
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

    expect(
        registration_state.get("registered_image_count", 0) >= 1,
        "expected registered image state for instance allocation probe",
    )
    expect(
        registration_state.get("registered_descriptor_total", 0) >= 1,
        "expected registered descriptors for instance allocation probe",
    )
    expect(
        selector_state.get("selector_table_entry_count", 0) >= 6,
        "expected synthesized accessor selectors in the selector table",
    )
    expect(
        selector_state.get("metadata_backed_selector_count", 0) >= 6,
        "expected selector table to distinguish metadata-backed selectors",
    )

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
    expect(
        set_count_entry.get("found") == 1 and set_count_entry.get("resolved") == 1,
        "expected setCount setter cache entry to resolve",
    )
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


__all__ = [
    "check_property_layout_case",
    "check_instance_allocation_layout_runtime_case",
]
