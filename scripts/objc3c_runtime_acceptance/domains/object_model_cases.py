"""Object Model linked-runtime acceptance cases."""

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
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from ..core import (
    REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
    ROOT,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)

_EXPORTED_CASE_NAMES = [
    "check_runtime_library_case",
    "check_canonical_dispatch_case",
    "check_metaclass_graph_root_class_case",
    "check_canonical_sample_set_case",
    "check_realization_lookup_reflection_runtime_case",
    "check_live_dispatch_fast_path_case",
]


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
        },
    )


def check_canonical_dispatch_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "canonical-dispatch"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "runtime_canonical_runnable_object_runtime_library.objc3"
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "runtime_canonical_runnable_object_probe.cpp"
    exe_path = case_dir / "runtime_canonical_runnable_object_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(run_probe(exe_path), "canonical dispatch probe")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    ll_text = ll_path.read_text(encoding="utf-8")

    expect(payload.get("traced_value") == 13, "expected category-backed tracedValue dispatch to return 13")
    expect(payload.get("inherited_value") == 7, "expected superclass inheritedValue dispatch to return 7")
    expect(payload.get("class_value") == 11, "expected class method dispatch to return 11")
    expect(payload.get("alloc_value", 0) != 0, "expected alloc dispatch to return a realized instance receiver")
    expect(payload.get("init_value") == payload.get("alloc_value"), "expected init to preserve the allocated receiver")
    expect(payload.get("new_value", 0) != 0, "expected new dispatch to materialize an instance receiver")
    expect(payload.get("ignored_value") == payload.get("ignored_expected"),
           "expected unresolved selector dispatch to return the strict dispatch error value")
    expect(payload.get("ignored_cached_value") == payload.get("ignored_expected"),
           "expected cached unresolved selector dispatch to preserve the strict dispatch error value")

    worker_query = payload.get("worker_query", {})
    tracer_query = payload.get("tracer_query", {})
    graph_state = payload.get("graph_state", {})
    widget_entry = payload.get("widget_entry", {})
    method_state = payload.get("method_state", {})
    inherited_state = payload.get("inherited_state", {})
    traced_state = payload.get("traced_state", {})
    class_state = payload.get("class_state", {})
    ignored_state = payload.get("ignored_state", {})
    ignored_cached_state = payload.get("ignored_cached_state", {})
    selector_handles = payload.get("selector_handles", {})
    selector_table_state = payload.get("selector_table_state", {})
    traced_selector_entry = payload.get("traced_selector_entry", {})
    inherited_selector_entry = payload.get("inherited_selector_entry", {})
    class_selector_entry = payload.get("class_selector_entry", {})
    ignored_selector_entry = payload.get("ignored_selector_entry", {})
    traced_entry = payload.get("traced_entry", {})
    inherited_entry = payload.get("inherited_entry", {})
    class_entry = payload.get("class_entry", {})
    ignored_entry = payload.get("ignored_entry", {})
    alloc_entry = payload.get("alloc_entry", {})
    init_entry = payload.get("init_entry", {})
    new_entry = payload.get("new_entry", {})

    expect(worker_query.get("conforms") == 1, "expected Widget to conform to Worker at runtime")
    expect(tracer_query.get("conforms") == 1, "expected Widget category attachment to satisfy Tracer at runtime")
    expect(
        graph_state.get("realized_class_count") == 2
        and graph_state.get("root_class_count") == 1
        and graph_state.get("metaclass_edge_count") == 1
        and graph_state.get("receiver_class_binding_count") == 2
        and graph_state.get("attached_category_count") == 1
        and graph_state.get("protocol_conformance_edge_count") == 2
        and graph_state.get("last_realized_class_name") == "Widget"
        and graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget"
        and graph_state.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and graph_state.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish the realized Widget class graph with stable class, metaclass, and attached-category lineage",
    )
    expect(
        widget_entry.get("found") == 1
        and widget_entry.get("is_root_class") == 0
        and widget_entry.get("implementation_backed") == 1
        and widget_entry.get("attached_category_count") == 1
        and widget_entry.get("direct_protocol_count") == 1
        and widget_entry.get("attached_protocol_count") == 1
        and widget_entry.get("class_name") == "Widget"
        and widget_entry.get("class_owner_identity") == "class:Widget"
        and widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and widget_entry.get("super_class_owner_identity") == "class:Base"
        and widget_entry.get("super_metaclass_owner_identity") == "metaclass:Base"
        and widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)"
        and widget_entry.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish stable Widget class, metaclass, superclass, attached-category, and protocol realization facts",
    )
    expect(
        tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer"
        and tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer conformance to resolve through the attached Widget(Tracing) category",
    )
    expect(method_state.get("live_dispatch_count", 0) >= 6, "expected live dispatch count to cover alloc/init/new/traced/inherited/class")
    expect(method_state.get("strict_dispatch_error_count", 0) == 2, "expected canonical dispatch workload to publish both unresolved strict error calls")
    expect(method_state.get("last_selector_stable_id", 0) == ignored_entry.get("selector_stable_id", 0),
           "expected last dispatch selector stable id to match the negative-cache ignoredValue selector")
    expect(selector_handles.get("alloc", 0) != 0 and selector_handles.get("tracedValue", 0) != 0,
           "expected canonical dispatch selectors to be interned in the runtime selector pool")
    expect(alloc_entry.get("selector_stable_id", 0) == selector_handles.get("alloc", 0),
           "expected alloc cache entry to be keyed by the selector pool stable id")
    expect(init_entry.get("selector_stable_id", 0) == selector_handles.get("init", 0),
           "expected init cache entry to be keyed by the selector pool stable id")
    expect(new_entry.get("selector_stable_id", 0) == selector_handles.get("new", 0),
           "expected new cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("selector_stable_id", 0) == selector_handles.get("tracedValue", 0),
           "expected tracedValue cache entry to be keyed by the selector pool stable id")
    expect(inherited_entry.get("selector_stable_id", 0) == selector_handles.get("inheritedValue", 0),
           "expected inheritedValue cache entry to be keyed by the selector pool stable id")
    expect(class_entry.get("selector_stable_id", 0) == selector_handles.get("classValue", 0),
           "expected classValue cache entry to be keyed by the selector pool stable id")
    expect(traced_entry.get("resolved") == 1, "expected tracedValue cache entry to resolve live")
    expect(inherited_entry.get("resolved") == 1, "expected inheritedValue cache entry to resolve live")
    expect(class_entry.get("resolved") == 1, "expected classValue cache entry to resolve live")
    expect(selector_table_state.get("metadata_backed_selector_count", 0) >= 4,
           "expected canonical dispatch selector materialization to keep metadata-backed selectors interned")
    expect(selector_table_state.get("dynamic_selector_count", 0) >= 1,
           "expected unresolved selector dispatch to intern a dynamic selector entry")
    expect(selector_table_state.get("last_materialized_selector") == "ignoredValue",
           "expected ignoredValue to be the last materialized selector after the strict error probe")
    expect(selector_table_state.get("last_materialized_from_metadata") == 0,
           "expected ignoredValue to be recorded as a dynamic selector lookup")
    expect(
        traced_selector_entry.get("found") == 1
        and traced_selector_entry.get("metadata_backed") == 1
        and traced_selector_entry.get("canonical_selector") == "tracedValue",
        "expected tracedValue to remain metadata-backed in the selector table",
    )
    expect(
        inherited_selector_entry.get("found") == 1
        and inherited_selector_entry.get("metadata_backed") == 1
        and inherited_selector_entry.get("canonical_selector") == "inheritedValue",
        "expected inheritedValue to remain metadata-backed in the selector table",
    )
    expect(
        class_selector_entry.get("found") == 1
        and class_selector_entry.get("metadata_backed") == 1
        and class_selector_entry.get("canonical_selector") == "classValue",
        "expected classValue to remain metadata-backed in the selector table",
    )
    expect(
        ignored_selector_entry.get("found") == 1
        and ignored_selector_entry.get("metadata_backed") == 0
        and ignored_selector_entry.get("canonical_selector") == "ignoredValue",
        "expected ignoredValue to materialize as a dynamic selector-table entry",
    )
    expect(
        inherited_state.get("last_dispatch_used_cache") == 0
        and inherited_state.get("last_dispatch_resolved_live_method") == 1
        and inherited_state.get("last_dispatch_strict_error") == 0
        and inherited_state.get("last_selector_stable_id") == selector_handles.get("inheritedValue", 0)
        and inherited_state.get("last_normalized_receiver_identity") == 1042
        and inherited_state.get("last_category_probe_count") == 1
        and inherited_state.get("last_protocol_probe_count") == 3,
        "expected inheritedValue to miss cache first, resolve live, and preserve category/protocol probe counts",
    )
    expect(
        traced_state.get("last_dispatch_used_cache") == 0
        and traced_state.get("last_dispatch_resolved_live_method") == 1
        and traced_state.get("last_dispatch_strict_error") == 0
        and traced_state.get("last_selector_stable_id") == selector_handles.get("tracedValue", 0)
        and traced_state.get("last_normalized_receiver_identity") == 1042
        and traced_state.get("last_category_probe_count") == 1
        and traced_state.get("last_protocol_probe_count") == 0,
        "expected tracedValue to resolve live through the attached category without protocol strict error probes",
    )
    expect(
        class_state.get("last_dispatch_used_cache") == 0
        and class_state.get("last_dispatch_resolved_live_method") == 1
        and class_state.get("last_dispatch_strict_error") == 0
        and class_state.get("last_selector_stable_id") == selector_handles.get("classValue", 0)
        and class_state.get("last_normalized_receiver_identity") == 1043,
        "expected classValue to resolve live through the metaclass path",
    )
    expect(
        ignored_state.get("last_dispatch_used_cache") == 0
        and ignored_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_state.get("last_dispatch_strict_error") == 1
        and ignored_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_state.get("last_normalized_receiver_identity") == 1042
        and ignored_state.get("last_category_probe_count") == 1
        and ignored_state.get("last_protocol_probe_count") == 3,
        "expected the first ignoredValue dispatch to materialize a negative cache entry and fall back deterministically",
    )
    expect(
        ignored_cached_state.get("last_dispatch_used_cache") == 1
        and ignored_cached_state.get("last_dispatch_resolved_live_method") == 0
        and ignored_cached_state.get("last_dispatch_strict_error") == 1
        and ignored_cached_state.get("last_selector_stable_id") == ignored_entry.get("selector_stable_id", 0)
        and ignored_cached_state.get("last_normalized_receiver_identity") == 1042
        and ignored_cached_state.get("last_category_probe_count") == 1
        and ignored_cached_state.get("last_protocol_probe_count") == 3,
        "expected the second ignoredValue dispatch to reuse the negative cache entry and preserve probe counts",
    )
    expect(
        traced_entry.get("resolved_owner_identity") == "implementation:Widget(Tracing)::instance_method:tracedValue",
        "expected tracedValue cache entry to preserve the category implementation owner",
    )
    expect(
        inherited_entry.get("normalized_receiver_identity") == 1042
        and inherited_entry.get("category_probe_count") == 1
        and inherited_entry.get("protocol_probe_count") == 3
        and inherited_entry.get("resolved_class_name") == "Base"
        and inherited_entry.get("resolved_owner_identity") == "implementation:Base::instance_method:inheritedValue",
        "expected inheritedValue cache entry to preserve the instance-family lookup result through Base",
    )
    expect(
        class_entry.get("dispatch_family_is_class") == 1
        and class_entry.get("normalized_receiver_identity") == 1043
        and class_entry.get("resolved_class_name") == "Widget"
        and class_entry.get("resolved_owner_identity") == "implementation:Widget::class_method:classValue",
        "expected classValue cache entry to preserve the metaclass lookup result",
    )
    expect(
        ignored_entry.get("found") == 1
        and ignored_entry.get("resolved") == 0
        and ignored_entry.get("dispatch_family_is_class") == 0
        and ignored_entry.get("normalized_receiver_identity") == 1042
        and ignored_entry.get("category_probe_count") == 1
        and ignored_entry.get("protocol_probe_count") == 3
        and ignored_entry.get("selector") == "ignoredValue",
        "expected ignoredValue to preserve an unresolved negative cache entry on the canonical instance receiver",
    )
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical dispatch compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical dispatch LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("selector_pool_section_root_symbol")
        == "@__objc3_sec_selector_pool",
        "expected canonical dispatch lowering surface to preserve the selector pool section root",
    )

    return CaseResult(
        case_id="canonical-dispatch",
        probe="tests/tooling/runtime/runtime_canonical_runnable_object_probe.cpp",
        fixture="tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "traced_value": payload["traced_value"],
            "inherited_value": payload["inherited_value"],
            "class_value": payload["class_value"],
            "live_dispatch_count": method_state["live_dispatch_count"],
            "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
            "ignored_strict_error": payload["ignored_expected"],
        },
    )


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


def check_realization_lookup_reflection_runtime_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = run_dir / "realization-lookup-reflection-runtime"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "canonical_runnable_sample_set.objc3"
    )
    obj_path, _, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    registration_manifest_path = (
        case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )

    probe = ROOT / REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE
    exe_path = case_dir / "object_model_lookup_reflection_runtime_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_json_output(
        run_probe(exe_path), "realization lookup reflection runtime probe"
    )
    aggregate = payload.get("aggregate", {})
    registration_manifest = json.loads(
        registration_manifest_path.read_text(encoding="utf-8")
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("widget_found") == 1, "expected Widget class lookup to succeed")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13")
    expect(payload.get("count_value") == 41, "expected count to reload the written value")
    expect(
        payload.get("count_property_found") == 1,
        "expected count property reflection lookup to succeed",
    )
    expect(payload.get("tracer_conforms") == 1, "expected Widget to conform to Tracer")
    expect(
        aggregate.get("realized_class_count") == 2,
        "expected aggregate realized class count to report the two live instance-class nodes",
    )
    expect(
        aggregate.get("reflectable_property_count") == 4,
        "expected aggregate reflectable property count to report the four live Widget/Base property accessors",
    )
    expect(
        aggregate.get("attached_category_count") == 1,
        "expected aggregate attached category count to report the single live attached category",
    )
    expect(
        aggregate.get("protocol_conformance_edge_count", 0) >= 2,
        "expected aggregate protocol conformance edge count to stay live",
    )
    expect(
        aggregate.get("method_cache_entry_count", 0) >= 4,
        "expected aggregate method cache entry count to reflect the executed dispatches",
    )
    expect(
        aggregate.get("last_class_query_found") == 1
        and aggregate.get("last_queried_class_name") == "Widget"
        and aggregate.get("last_resolved_class_name") == "Widget",
        "expected aggregate state to preserve the last class lookup",
    )
    expect(
        aggregate.get("last_property_query_found") == 1
        and aggregate.get("last_property_query_inherited") == 0
        and aggregate.get("last_queried_property_name") == "count"
        and aggregate.get("last_resolved_property_class_name") == "Widget",
        "expected aggregate state to preserve the last property lookup",
    )
    expect(
        aggregate.get("last_protocol_query_class_found") == 1
        and aggregate.get("last_protocol_query_protocol_found") == 1
        and aggregate.get("last_protocol_query_conforms") == 1
        and aggregate.get("last_queried_protocol_class_name") == "Widget"
        and aggregate.get("last_queried_protocol_name") == "Tracer",
        "expected aggregate state to preserve the last protocol-conformance query",
    )
    expect(
        bool(aggregate.get("last_resolved_class_owner_identity"))
        and bool(aggregate.get("last_resolved_property_owner_identity"))
        and bool(aggregate.get("last_matched_protocol_owner_identity")),
        "expected aggregate state to preserve the last resolved owner identities",
    )
    expect(
        registration_manifest.get("class_descriptor_count") == 4
        and registration_manifest.get("property_descriptor_count") == 8
        and registration_manifest.get("category_descriptor_count") == 2,
        "expected canonical sample-set registration manifests to keep the broader emitted descriptor counts",
    )
    implementation_surface = manifest.get(
        "runtime_realization_lookup_reflection_implementation_surface", {}
    )
    expect(
        implementation_surface.get("contract_id")
        == RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        "expected compile manifest to publish the realization lookup/reflection implementation surface",
    )

    return CaseResult(
        case_id="realization-lookup-reflection-runtime",
        probe=REALIZATION_LOOKUP_REFLECTION_RUNTIME_PROBE,
        fixture="tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "realized_class_count": aggregate["realized_class_count"],
            "reflectable_property_count": aggregate["reflectable_property_count"],
            "method_cache_entry_count": aggregate["method_cache_entry_count"],
            "last_queried_protocol_name": aggregate["last_queried_protocol_name"],
        },
    )


def check_live_dispatch_fast_path_case(clangxx: str, run_dir: Path) -> CaseResult:
    case_dir = run_dir / "dispatch-fast-path"
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "live_dispatch_fast_path_positive.objc3"
    )
    obj_path, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / "compile")
    probe = ROOT / "tests" / "tooling" / "runtime" / "live_dispatch_fast_path_probe.cpp"
    exe_path = case_dir / "live_dispatch_fast_path_probe.exe"
    compile_probe(clangxx, probe, exe_path, [obj_path])
    payload = parse_key_value_output(run_probe(exe_path), "dispatch fast-path probe")
    ll_text = ll_path.read_text(encoding="utf-8")
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    registration_manifest_path = case_dir / "compile" / "module.runtime-registration-manifest.json"
    registration_manifest = json.loads(registration_manifest_path.read_text(encoding="utf-8"))

    expect(payload.get("baseline_status") == 0, "expected baseline method-cache snapshot to succeed")
    expect(payload.get("dynamic_entry_status") == 0, "expected dynamic fast-path entry lookup to succeed")
    expect(payload.get("explicit_entry_status") == 0, "expected explicit fast-path entry lookup to succeed")
    expect(payload.get("strict_error_entry_status") == 0, "expected strict error method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("strict_error_first") == payload.get("strict_error_expected") == payload.get("strict_error_second"),
           "expected strict dispatch error to stay deterministic across cache miss/hit")
    expect(payload.get("baseline_cache_entry_count") == 4,
           "expected realized dispatch runtime to seed four method-cache entries")
    expect(payload.get("baseline_fast_path_seed_count") == 4,
           "expected realized dispatch runtime to publish seeded fast-path entries")
    expect(payload.get("dynamic_entry_found") == 1 and payload.get("dynamic_entry_resolved") == 1,
           "expected dynamicEscape entry to resolve live")
    expect(payload.get("dynamic_entry_fast_path_seeded") == 1,
           "expected dynamicEscape entry to be seeded for fast-path dispatch")
    expect(payload.get("dynamic_entry_effective_direct_dispatch") == 0,
           "expected dynamicEscape entry to stay runtime-dispatched")
    expect(payload.get("dynamic_entry_fast_path_reason") == "class-final",
           "expected dynamicEscape fast-path reason to remain class-final")
    expect(payload.get("explicit_entry_found") == 1 and payload.get("explicit_entry_resolved") == 1,
           "expected explicitDirect entry to resolve live")
    expect(payload.get("explicit_entry_fast_path_seeded") == 1,
           "expected explicitDirect entry to be seeded for direct dispatch")
    expect(payload.get("explicit_entry_effective_direct_dispatch") == 1,
           "expected explicitDirect entry to preserve direct dispatch semantics")
    expect(payload.get("explicit_entry_fast_path_reason") == "direct",
           "expected explicitDirect fast-path reason to remain direct")
    expect(payload.get("mixed_first_state_last_dispatch_used_cache") == 1,
           "expected first mixed dispatch runtime call to hit the seeded cache")
    expect(payload.get("mixed_first_state_last_dispatch_used_fast_path") == 1,
           "expected first mixed dispatch runtime call to use the seeded fast path")
    expect(payload.get("mixed_first_state_last_dispatch_resolved_live_method") == 1,
           "expected first mixed dispatch runtime call to resolve a live method")
    expect(payload.get("mixed_first_state_last_dispatch_strict_error") == 0,
           "did not expect first mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_first_state_last_selector") == "dynamicEscape",
           "expected first mixed dispatch runtime call to target dynamicEscape")
    expect(payload.get("mixed_first_dispatch_state_status") == 0,
           "expected first mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_first_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected first mixed dispatch runtime call to report the cache-hit fast path")
    expect(payload.get("mixed_first_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected first mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_first_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected first mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_first_dispatch_state_last_used_builtin") == 0,
           "expected first mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("mixed_second_state_last_dispatch_used_cache") == 1,
           "expected repeated mixed dispatch runtime call to remain cached")
    expect(payload.get("mixed_second_state_last_dispatch_used_fast_path") == 1,
           "expected repeated mixed dispatch runtime call to remain on the fast path")
    expect(payload.get("mixed_second_state_last_dispatch_strict_error") == 0,
           "did not expect repeated mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_second_dispatch_state_status") == 0,
           "expected repeated mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_second_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected repeated mixed dispatch runtime call to stay on the cache-hit fast path")
    expect(payload.get("mixed_second_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected repeated mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_second_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected repeated mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_second_dispatch_state_last_used_builtin") == 0,
           "expected repeated mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("strict_error_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("strict_error_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("strict_error_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("strict_error_first_state_last_dispatch_strict_error") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("strict_error_first_dispatch_state_status") == 0,
           "expected first missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_first_dispatch_state_last_dispatch_path") == "slow-path-error",
           "expected first missingDispatch: call to report slow-path strict dispatch error")
    expect(payload.get("strict_error_first_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected first missingDispatch: call to report strict dispatch error status")
    expect(payload.get("strict_error_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the strict error cache entry")
    expect(payload.get("strict_error_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("strict_error_second_state_last_dispatch_strict_error") == 1,
           "expected repeated missingDispatch: call to remain a strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_status") == 0,
           "expected repeated missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_second_dispatch_state_last_dispatch_path") == "cache-hit-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error status")
    expect(
        "; method_dispatch_and_selector_thunk_lowering_surface = "
        "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
        in ll_text,
        "expected LLVM IR to publish authoritative method dispatch and selector thunk lowering surface",
    )
    expect("direct_dispatch_call_sites=5" in ll_text,
           "expected mixed dispatch fixture to emit five direct dispatch calls")
    expect("runtime_dispatch_call_sites=1" in ll_text,
           "expected mixed dispatch fixture to emit one live runtime dispatch call")
    expect("selector_pool_gep_sites=1" in ll_text,
           "expected mixed dispatch fixture to materialize one selector thunk gep")
    expect("selector_pool_count=4" in ll_text,
           "expected mixed dispatch fixture to publish four pooled selectors")
    expect("dynamic_opt_out_sites=2" in ll_text,
           "expected mixed dispatch fixture to preserve two objc_dynamic opt-out sites")
    expect("call i32 @objc3_method_PolicyBox_class_implicitDirect()" in ll_text,
           "expected implicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_explicitDirect()" in ll_text,
           "expected explicit direct calls to lower as exact direct LLVM calls")
    expect("call i32 @objc3_method_PolicyBox_class_callers()" in ll_text,
           "expected runFixture to preserve direct class-method dispatch to callers")
    expect("call i32 @objc3_runtime_dispatch_i32(" in ll_text,
           "expected dynamicEscape lowering to retain the live runtime dispatch call")
    expect("@__objc3_sec_selector_pool" in ll_text,
           "expected mixed dispatch fixture to emit the selector pool section root")
    lowering_surface = manifest.get("dispatch_and_synthesized_accessor_lowering_surface", {})
    expect(isinstance(lowering_surface, dict),
           "expected compile manifest to publish the live lowering surface")
    expect(lowering_surface.get("runtime_dispatch_symbol_matches_lowering") is True,
           "expected compile manifest lowering surface to keep dispatch symbols aligned")
    expect(lowering_surface.get("message_send_sites") == 6,
           "expected compile manifest lowering surface to publish six message send sites")
    runtime_abi_surface = manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(runtime_abi_surface, dict),
           "expected compile manifest to publish dispatch/accessor runtime ABI surface")
    expect(runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in compile manifest")
    expect(runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime ABI surface to publish canonical runtime dispatch symbol")
    expect(runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime ABI surface to publish dispatch state snapshot helper")
    expect(runtime_abi_surface.get("method_cache_state_snapshot_symbol") == "objc3_runtime_copy_method_cache_state_for_testing",
           "expected runtime ABI surface to publish method cache state snapshot helper")
    expect(runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime ABI surface to publish property registry snapshot helper")
    expect(runtime_abi_surface.get("arc_debug_state_snapshot_symbol") == "objc3_runtime_copy_arc_debug_state_for_testing",
           "expected runtime ABI surface to publish ARC debug snapshot helper")
    expect(runtime_abi_surface.get("bind_current_property_context_symbol") == "objc3_runtime_bind_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context bind helper")
    expect(runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime ABI surface to publish property context clear helper")
    expect(runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime ABI surface to remain on the private testing boundary")
    expect(runtime_abi_surface.get("deterministic") is True,
           "expected runtime ABI surface to report deterministic handoff")
    storage_runtime_abi_surface = manifest.get("storage_accessor_runtime_abi_surface", {})
    expect(isinstance(storage_runtime_abi_surface, dict),
           "expected compile manifest to publish storage/accessor runtime ABI surface")
    expect(storage_runtime_abi_surface.get("contract_id") == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
           "expected storage/accessor runtime ABI surface contract id in compile manifest")
    expect(storage_runtime_abi_surface.get("abi_boundary_model") == "private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening",
           "expected storage/accessor runtime ABI surface to publish the private helper boundary model")
    expect(storage_runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected storage/accessor runtime ABI surface to publish property registry snapshot helper")
    expect(storage_runtime_abi_surface.get("property_entry_snapshot_symbol") == "objc3_runtime_copy_property_entry_for_testing",
           "expected storage/accessor runtime ABI surface to publish property entry snapshot helper")
    expect(storage_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish current-property read helper")
    expect(storage_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish current-property exchange helper")
    expect(storage_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected storage/accessor runtime ABI surface to publish weak current-property load helper")
    expect(storage_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected storage/accessor runtime ABI surface to remain private-testing only")
    expect(storage_runtime_abi_surface.get("deterministic") is True,
           "expected storage/accessor runtime ABI surface to report deterministic handoff")
    registration_runtime_abi_surface = registration_manifest.get("dispatch_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish dispatch/accessor runtime ABI surface")
    expect(registration_runtime_abi_surface.get("contract_id") == "objc3c.runtime.dispatch_accessor.abi.surface.v1",
           "expected dispatch/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_runtime_abi_surface.get("runtime_dispatch_symbol") == "objc3_runtime_dispatch_i32",
           "expected runtime registration manifest to publish canonical runtime dispatch symbol")
    expect(registration_runtime_abi_surface.get("dispatch_state_snapshot_symbol") == "objc3_runtime_copy_dispatch_state_for_testing",
           "expected runtime registration manifest to publish dispatch state snapshot helper")
    expect(registration_runtime_abi_surface.get("current_property_read_symbol") == "objc3_runtime_read_current_property_i32",
           "expected runtime registration manifest to publish current-property read helper")
    expect(registration_runtime_abi_surface.get("current_property_exchange_symbol") == "objc3_runtime_exchange_current_property_i32",
           "expected runtime registration manifest to publish current-property exchange helper")
    expect(registration_runtime_abi_surface.get("weak_current_property_load_symbol") == "objc3_runtime_load_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property load helper")
    expect(registration_runtime_abi_surface.get("autorelease_symbol") == "objc3_runtime_autorelease_i32",
           "expected runtime registration manifest to publish autorelease helper")
    expect(registration_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest ABI surface to remain private-testing only")
    expect(registration_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest ABI surface to report deterministic handoff")
    registration_storage_runtime_abi_surface = registration_manifest.get("storage_accessor_runtime_abi_surface", {})
    expect(isinstance(registration_storage_runtime_abi_surface, dict),
           "expected runtime registration manifest to publish storage/accessor runtime ABI surface")
    expect(registration_storage_runtime_abi_surface.get("contract_id") == RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
           "expected storage/accessor runtime ABI surface contract id in runtime registration manifest")
    expect(registration_storage_runtime_abi_surface.get("property_registry_state_snapshot_symbol") == "objc3_runtime_copy_property_registry_state_for_testing",
           "expected runtime registration manifest to publish property registry snapshot helper")
    expect(registration_storage_runtime_abi_surface.get("current_property_write_symbol") == "objc3_runtime_write_current_property_i32",
           "expected runtime registration manifest to publish current-property write helper")
    expect(registration_storage_runtime_abi_surface.get("clear_current_property_context_symbol") == "objc3_runtime_clear_current_property_context_for_testing",
           "expected runtime registration manifest to publish property context clear helper")
    expect(registration_storage_runtime_abi_surface.get("weak_current_property_store_symbol") == "objc3_runtime_store_weak_current_property_i32",
           "expected runtime registration manifest to publish weak current-property store helper")
    expect(registration_storage_runtime_abi_surface.get("private_testing_surface_only") is True,
           "expected runtime registration manifest storage/accessor ABI surface to remain private-testing only")
    expect(registration_storage_runtime_abi_surface.get("deterministic") is True,
           "expected runtime registration manifest storage/accessor ABI surface to report deterministic handoff")

    return CaseResult(
        case_id="dispatch-fast-path",
        probe="tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        fixture="tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
        claim_class="linked-runtime-probe",
        passed=True,
        summary={
            "llvm_ir": str(ll_path.relative_to(ROOT)).replace("\\", "/"),
            "manifest": str(manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "registration_manifest": str(registration_manifest_path.relative_to(ROOT)).replace("\\", "/"),
            "baseline_cache_entry_count": payload.get("baseline_cache_entry_count"),
            "baseline_fast_path_seed_count": payload.get("baseline_fast_path_seed_count"),
            "mixed_first_dispatch_path": payload.get("mixed_first_dispatch_state_last_dispatch_path"),
            "mixed_first_implementation_kind": payload.get("mixed_first_dispatch_state_last_implementation_kind"),
            "mixed_second_dispatch_path": payload.get("mixed_second_dispatch_state_last_dispatch_path"),
            "mixed_second_implementation_kind": payload.get("mixed_second_dispatch_state_last_implementation_kind"),
            "strict_error_first_dispatch_path": payload.get("strict_error_first_dispatch_state_last_dispatch_path"),
            "strict_error_first_implementation_kind": payload.get("strict_error_first_dispatch_state_last_implementation_kind"),
            "strict_error_second_dispatch_path": payload.get("strict_error_second_dispatch_state_last_dispatch_path"),
            "strict_error_second_implementation_kind": payload.get("strict_error_second_dispatch_state_last_implementation_kind"),
            "mixed_first_live_dispatch_count": payload.get("mixed_first_state_live_dispatch_count"),
            "mixed_second_live_dispatch_count": payload.get("mixed_second_state_live_dispatch_count"),
            "strict_error_first_strict_dispatch_error_count": payload.get("strict_error_first_state_strict_dispatch_error_count"),
            "strict_error_second_strict_dispatch_error_count": payload.get("strict_error_second_state_strict_dispatch_error_count"),
        },
    )


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
