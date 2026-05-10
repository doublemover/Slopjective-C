"""Return-value and class-graph expectations for canonical object dispatch."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .object_model_dispatch_payload_model import CanonicalDispatchPayload


def assert_dispatch_return_values(facts: CanonicalDispatchPayload) -> None:
    payload = facts.payload
    expect(
        payload.get("traced_value") == 13,
        "expected category-backed tracedValue dispatch to return 13",
    )
    expect(
        payload.get("inherited_value") == 7,
        "expected superclass inheritedValue dispatch to return 7",
    )
    expect(
        payload.get("class_value") == 11,
        "expected class method dispatch to return 11",
    )
    expect(
        payload.get("alloc_value", 0) != 0,
        "expected alloc dispatch to return a realized instance receiver",
    )
    expect(
        payload.get("init_value") == payload.get("alloc_value"),
        "expected init to preserve the allocated receiver",
    )
    expect(
        payload.get("new_value", 0) != 0,
        "expected new dispatch to materialize an instance receiver",
    )
    expect(
        payload.get("ignored_value") == payload.get("ignored_expected"),
        "expected unresolved selector dispatch to return the strict dispatch error value",
    )
    expect(
        payload.get("ignored_cached_value") == payload.get("ignored_expected"),
        "expected cached unresolved selector dispatch to preserve the strict dispatch error value",
    )


def assert_dispatch_class_graph(facts: CanonicalDispatchPayload) -> None:
    expect(
        facts.worker_query.get("conforms") == 1,
        "expected Widget to conform to Worker at runtime",
    )
    expect(
        facts.tracer_query.get("conforms") == 1,
        "expected Widget category attachment to satisfy Tracer at runtime",
    )
    expect(
        facts.graph_state.get("realized_class_count") == 2
        and facts.graph_state.get("root_class_count") == 1
        and facts.graph_state.get("metaclass_edge_count") == 1
        and facts.graph_state.get("receiver_class_binding_count") == 2
        and facts.graph_state.get("attached_category_count") == 1
        and facts.graph_state.get("protocol_conformance_edge_count") == 2
        and facts.graph_state.get("last_realized_class_name") == "Widget"
        and facts.graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and facts.graph_state.get("last_realized_metaclass_owner_identity")
        == "metaclass:Widget"
        and facts.graph_state.get("last_attached_category_owner_identity")
        == "category:Widget(Tracing)"
        and facts.graph_state.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish the realized Widget class graph with stable class, metaclass, and attached-category lineage",
    )
    expect(
        facts.widget_entry.get("found") == 1
        and facts.widget_entry.get("is_root_class") == 0
        and facts.widget_entry.get("implementation_backed") == 1
        and facts.widget_entry.get("attached_category_count") == 1
        and facts.widget_entry.get("direct_protocol_count") == 1
        and facts.widget_entry.get("attached_protocol_count") == 1
        and facts.widget_entry.get("class_name") == "Widget"
        and facts.widget_entry.get("class_owner_identity") == "class:Widget"
        and facts.widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and facts.widget_entry.get("super_class_owner_identity") == "class:Base"
        and facts.widget_entry.get("super_metaclass_owner_identity") == "metaclass:Base"
        and facts.widget_entry.get("last_attached_category_owner_identity")
        == "category:Widget(Tracing)"
        and facts.widget_entry.get("last_attached_category_name") == "Tracing",
        "expected canonical dispatch to publish stable Widget class, metaclass, superclass, attached-category, and protocol realization facts",
    )
    expect(
        facts.tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer"
        and facts.tracer_query.get("matched_attachment_owner_identity")
        == "category:Widget(Tracing)",
        "expected Tracer conformance to resolve through the attached Widget(Tracing) category",
    )


__all__ = [
    "assert_dispatch_class_graph",
    "assert_dispatch_return_values",
]
