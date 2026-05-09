"""Object Model metaclass and canonical sample linked-runtime cases."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.native_build import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    ROOT,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "check_metaclass_graph_root_class_case",
    "check_canonical_sample_set_case",
]


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


def check_metaclass_graph_root_class_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_id = "metaclass-graph-root-class"
    case_dir = run_dir / case_id
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "metaclass_graph_root_class_library.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "metaclass_graph_root_class_probe.cpp"
    exe_path = case_dir / "metaclass_graph_root_class_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "metaclass/root-class graph probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    graph_state = payload.get("graph_state", {})
    root_entry = payload.get("root_entry", {})
    widget_entry = payload.get("widget_entry", {})
    root_class_state = payload.get("root_class_state", {})
    widget_class_state = payload.get("widget_class_state", {})
    widget_known_class_state = payload.get("widget_known_class_state", {})
    widget_inherited_state = payload.get("widget_inherited_state", {})
    widget_own_state = payload.get("widget_own_state", {})
    root_shared_entry = payload.get("root_shared_entry", {})
    widget_shared_entry = payload.get("widget_shared_entry", {})
    widget_inherited_entry = payload.get("widget_inherited_entry", {})
    widget_own_entry = payload.get("widget_own_entry", {})
    surface = manifest.get("runtime_class_metaclass_protocol_realization_surface")

    expect(payload.get("root_class_value") == 19, "expected root class method dispatch to return 19")
    expect(payload.get("widget_class_value") == 19, "expected Widget class dispatch to inherit RootObject +shared")
    expect(payload.get("widget_known_class_value") == 19, "expected normalized Widget class receiver to reuse inherited class dispatch")
    expect(payload.get("widget_inherited_instance_value") == 17, "expected Widget instance dispatch to inherit RootObject -rootValue")
    expect(payload.get("widget_own_instance_value") == 23, "expected Widget instance dispatch to resolve Widget -widgetValue")
    expect(
        graph_state.get("realized_class_count") == 2
        and graph_state.get("root_class_count") == 1
        and graph_state.get("metaclass_edge_count") == 1
        and graph_state.get("receiver_class_binding_count") == 2
        and graph_state.get("last_realized_class_name") == "Widget"
        and graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget",
        "expected realized graph to publish RootObject as the sole root and Widget as the single class/metaclass edge",
    )
    expect(
        root_entry.get("found") == 1
        and root_entry.get("base_identity") == 1024
        and root_entry.get("is_root_class") == 1
        and root_entry.get("implementation_backed") == 1
        and root_entry.get("class_name") == "RootObject"
        and root_entry.get("class_owner_identity") == "class:RootObject"
        and root_entry.get("metaclass_owner_identity") == "metaclass:RootObject"
        and root_entry.get("super_class_owner_identity") is None
        and root_entry.get("super_metaclass_owner_identity") is None,
        "expected RootObject entry to realize as an implementation-backed root with null superclass and metaclass-super links",
    )
    expect(
        widget_entry.get("found") == 1
        and widget_entry.get("base_identity") == 1041
        and widget_entry.get("is_root_class") == 0
        and widget_entry.get("implementation_backed") == 1
        and widget_entry.get("class_name") == "Widget"
        and widget_entry.get("class_owner_identity") == "class:Widget"
        and widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and widget_entry.get("super_class_owner_identity") == "class:RootObject"
        and widget_entry.get("super_metaclass_owner_identity") == "metaclass:RootObject",
        "expected Widget entry to publish stable class/metaclass owner identities and RootObject superclass links",
    )
    expect(
        root_class_state.get("last_resolved_class_name") == "RootObject"
        and root_class_state.get("last_resolved_owner_identity") == "implementation:RootObject::class_method:shared"
        and root_class_state.get("last_dispatch_resolved_live_method") == 1,
        "expected RootObject class dispatch to resolve through the live metaclass method list",
    )
    expect(
        widget_class_state.get("last_resolved_class_name") == "RootObject"
        and widget_class_state.get("last_resolved_owner_identity") == "implementation:RootObject::class_method:shared"
        and widget_class_state.get("last_dispatch_resolved_live_method") == 1,
        "expected Widget class dispatch to walk the metaclass superclass chain",
    )
    expect(
        widget_known_class_state.get("last_dispatch_used_cache") == 1
        and widget_known_class_state.get("last_resolved_class_name") == "RootObject",
        "expected repeated Widget class dispatch to reuse the cache while preserving inherited RootObject resolution",
    )
    expect(
        widget_inherited_state.get("last_resolved_class_name") == "RootObject"
        and widget_inherited_state.get("last_resolved_owner_identity") == "implementation:RootObject::instance_method:rootValue",
        "expected Widget instance dispatch to walk the class superclass chain for RootObject -rootValue",
    )
    expect(
        widget_own_state.get("last_resolved_class_name") == "Widget"
        and widget_own_state.get("last_resolved_owner_identity") == "implementation:Widget::instance_method:widgetValue",
        "expected Widget instance dispatch to resolve its own method before walking superclasses",
    )
    for entry_name, entry, expected_owner, expected_class_dispatch in (
        ("root_shared_entry", root_shared_entry, "implementation:RootObject::class_method:shared", 1),
        ("widget_shared_entry", widget_shared_entry, "implementation:RootObject::class_method:shared", 1),
        ("widget_inherited_entry", widget_inherited_entry, "implementation:RootObject::instance_method:rootValue", 0),
        ("widget_own_entry", widget_own_entry, "implementation:Widget::instance_method:widgetValue", 0),
    ):
        expect(
            entry.get("found") == 1
            and entry.get("resolved") == 1
            and entry.get("dispatch_family_is_class") == expected_class_dispatch
            and entry.get("resolved_owner_identity") == expected_owner,
            f"expected {entry_name} to publish a resolved method-cache entry with stable owner identity",
        )
    expect(
        isinstance(surface, dict)
        and surface.get("contract_id")
        == RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID
        and surface.get("realized_class_graph_snapshot_symbol")
        == "objc3_runtime_copy_realized_class_graph_state_for_testing"
        and surface.get("realized_class_entry_snapshot_symbol")
        == "objc3_runtime_copy_realized_class_entry_for_testing",
        "expected compile manifest to publish the class/metaclass protocol realization surface and private snapshot symbols",
    )
    expect(
        "runtime_metadata_class_metaclass_emission" in ll_text
        and "runtime_metaclass_graph_root_class_baseline" in ll_text,
        "expected LLVM IR to carry class/metaclass emission and root-class baseline proof comments",
    )

    negative_batch = compile_negative_diagnostic_batch(
        case_id=case_id,
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="missing-superclass",
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / "inheritance_override_missing_superclass.objc3",
                expected_snippets=[
                    "runtime realization failed: interface 'Widget' inherits from missing superclass 'MissingRoot'",
                ],
                expected_codes=["O3S220"],
            ),
            NegativeDiagnosticExpectation(
                key="unrealized-superclass",
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / "inheritance_override_unrealized_superclass.objc3",
                expected_snippets=[
                    "runtime realization failed: implementation 'Widget' requires realized superclass implementation 'Root'",
                ],
                expected_codes=["O3S220"],
            ),
            NegativeDiagnosticExpectation(
                key="superclass-cycle",
                fixture=ROOT / "tests" / "tooling" / "fixtures" / "native" / "inheritance_override_cycle.objc3",
                expected_snippets=[
                    "cyclic Objective-C interface inheritance cannot produce a stable ivar layout",
                    "runtime metadata export blocked",
                ],
                expected_codes=["O3P150", "O3S260"],
            ),
        ],
    )

    return CaseResult(
        case_id=case_id,
        probe=str(probe.relative_to(ROOT)).replace("\\", "/"),
        fixture=str(fixture.relative_to(ROOT)).replace("\\", "/"),
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "compile_manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "positive_execution_fixture": "tests/tooling/fixtures/native/execution/positive/class_metaclass_root_runtime_dispatch.objc3",
            "negative_execution_fixture": "tests/tooling/fixtures/native/execution/negative/class_metaclass_missing_superclass.objc3",
            "realized_class_count": graph_state.get("realized_class_count"),
            "root_class_count": graph_state.get("root_class_count"),
            "metaclass_edge_count": graph_state.get("metaclass_edge_count"),
            "receiver_class_binding_count": graph_state.get("receiver_class_binding_count"),
            "root_base_identity": root_entry.get("base_identity"),
            "widget_base_identity": widget_entry.get("base_identity"),
            "negative_diagnostics_batch": negative_batch,
        },
    )


def check_canonical_sample_set_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-sample-set"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "canonical_runnable_sample_set.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    probe = ROOT / "tests" / "tooling" / "runtime" / "canonical_runnable_sample_set_probe.cpp"
    exe_path = case_dir / "canonical_runnable_sample_set_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical runnable sample set probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    widget_entry = payload.get("widget_entry", {})
    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    count_property = payload.get("count_property", {})
    value_property = payload.get("value_property", {})
    token_property = payload.get("token_property", {})

    expect(widget_entry.get("found") == 1, "expected Widget to realize successfully for the canonical sample set")
    expect(widget_entry.get("base_identity") == 1041, "expected Widget base identity 1041 for the canonical sample set")
    expect(widget_entry.get("runtime_property_accessor_count") == 4, "expected Widget to expose four runtime property accessors")
    expect(widget_entry.get("runtime_instance_size_bytes") == 24, "expected Widget instance size to remain 24 bytes")
    expect(
        widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)",
        "expected Widget to preserve the attached Tracing category owner",
    )

    expect(registration_manifest.get("class_descriptor_count") == 4, "expected canonical sample set to publish four class descriptors")
    expect(registration_manifest.get("protocol_descriptor_count") == 2, "expected canonical sample set to publish two protocol descriptors")
    expect(registration_manifest.get("category_descriptor_count") == 2, "expected canonical sample set to publish two category descriptors")
    expect(registration_manifest.get("property_descriptor_count") == 8, "expected canonical sample set to publish eight property descriptors")
    expect(registration_manifest.get("ivar_descriptor_count") == 4, "expected canonical sample set to publish four ivar descriptors")
    expect(
        registration_manifest.get("compile_output_truthfulness_property_descriptor_count") == 8,
        "expected compile-output truthfulness to certify eight property descriptors for the canonical sample set",
    )
    expect(
        registration_manifest.get("compile_output_truthfulness_ivar_descriptor_count") == 4,
        "expected compile-output truthfulness to certify four ivar descriptors for the canonical sample set",
    )

    expect(payload.get("init_value", 0) != 0, "expected alloc/init to return a non-zero canonical sample-set receiver")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13 for the canonical sample set")
    expect(payload.get("inherited_value") == 7, "expected inheritedValue to return 7 for the canonical sample set")
    expect(payload.get("class_value") == 11, "expected classValue to return 11 for the canonical sample set")
    expect(payload.get("shared_value") == 19, "expected shared to return 19 for the canonical sample set")
    expect(payload.get("count_value") == 37, "expected count to reload 37 for the canonical sample set")
    expect(payload.get("enabled_value") == 1, "expected enabled to reload 1 for the canonical sample set")
    expect(payload.get("current_value") == 55, "expected currentValue to reload 55 for the canonical sample set")
    expect(payload.get("token_value") == 0, "expected tokenValue to remain 0 for the canonical sample set")

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker in the canonical sample set")
    expect(
        worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"},
        "expected Worker query to resolve through Worker or inherited Tracer",
    )
    expect(worker_query.get("matched_attachment_owner_identity") is None, "did not expect Worker query to require an attachment-owner match")
    expect(tracer_query.get("conforms") == 1, "expected Widget to conform to Tracer in the canonical sample set")
    expect(tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "expected Tracer query to resolve through protocol:Tracer")
    expect(
        tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer query to resolve through the attached category owner",
    )

    expect(
        count_property.get("found") == 1
        and count_property.get("slot_index") == 0
        and count_property.get("offset_bytes") == 0
        and count_property.get("size_bytes") == 4
        and count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count"
        and count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected count property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        value_property.get("found") == 1
        and value_property.get("slot_index") == 2
        and value_property.get("offset_bytes") == 8
        and value_property.get("size_bytes") == 8
        and value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue"
        and value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:",
        "expected currentValue property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        token_property.get("found") == 1
        and token_property.get("slot_index") == 3
        and token_property.get("offset_bytes") == 16
        and token_property.get("setter_available") == 0
        and token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue"
        and token_property.get("setter_owner_identity") is None,
        "expected tokenValue property reflection to preserve readonly slot/layout/accessor facts",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical sample set compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical sample set LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("class_aggregate_symbol")
        == "__objc3_sec_class_descriptors",
        "expected canonical sample set lowering surface to preserve the class aggregate root symbol",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("property_aggregate_symbol")
        == "__objc3_sec_property_descriptors",
        "expected canonical sample set lowering surface to preserve the property aggregate root symbol",
    )

    return CaseResult(
        case_id="canonical-sample-set",
        probe="tests/tooling/runtime/canonical_runnable_sample_set_probe.cpp",
        fixture="tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "widget_base_identity": widget_entry.get("base_identity"),
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "shared_value": payload["shared_value"],
            "property_descriptor_count": registration_manifest.get("property_descriptor_count"),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
