"""Canonical object dispatch runtime payload assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.assertions import expect


def assert_canonical_dispatch_payload(payload: dict[str, Any]) -> None:
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


def build_canonical_dispatch_summary(payload: dict[str, Any]) -> dict[str, Any]:
    method_state = payload.get("method_state", {})
    return {
        "traced_value": payload["traced_value"],
        "inherited_value": payload["inherited_value"],
        "class_value": payload["class_value"],
        "live_dispatch_count": method_state["live_dispatch_count"],
        "attached_category_count": payload.get("graph_state", {}).get("attached_category_count"),
        "ignored_strict_error": payload["ignored_expected"],
    }


__all__ = [
    "assert_canonical_dispatch_payload",
    "build_canonical_dispatch_summary",
]
