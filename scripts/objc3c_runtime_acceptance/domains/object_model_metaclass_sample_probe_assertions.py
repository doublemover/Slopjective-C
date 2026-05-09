"""Runtime probe assertions for Object Model metaclass sample cases."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.assertions import expect


@dataclass(frozen=True)
class MetaclassGraphProbeFacts:
    graph_state: dict[str, Any]
    root_entry: dict[str, Any]
    widget_entry: dict[str, Any]
    root_class_state: dict[str, Any]
    widget_class_state: dict[str, Any]
    widget_known_class_state: dict[str, Any]
    widget_inherited_state: dict[str, Any]
    widget_own_state: dict[str, Any]
    root_shared_entry: dict[str, Any]
    widget_shared_entry: dict[str, Any]
    widget_inherited_entry: dict[str, Any]
    widget_own_entry: dict[str, Any]


@dataclass(frozen=True)
class CanonicalSampleProbeFacts:
    payload: dict[str, Any]
    widget_entry: dict[str, Any]
    worker_query: dict[str, Any]
    tracer_query: dict[str, Any]
    count_property: dict[str, Any]
    value_property: dict[str, Any]
    token_property: dict[str, Any]


def capture_canonical_sample_probe_facts(
    payload: dict[str, Any],
) -> CanonicalSampleProbeFacts:
    return CanonicalSampleProbeFacts(
        payload=payload,
        widget_entry=payload.get("widget_entry", {}),
        worker_query=payload.get("worker_query", {}),
        tracer_query=payload.get("tracer_query", {}),
        count_property=payload.get("count_property", {}),
        value_property=payload.get("value_property", {}),
        token_property=payload.get("token_property", {}),
    )


def assert_metaclass_graph_probe_payload(
    payload: dict[str, Any],
) -> MetaclassGraphProbeFacts:
    facts = MetaclassGraphProbeFacts(
        graph_state=payload.get("graph_state", {}),
        root_entry=payload.get("root_entry", {}),
        widget_entry=payload.get("widget_entry", {}),
        root_class_state=payload.get("root_class_state", {}),
        widget_class_state=payload.get("widget_class_state", {}),
        widget_known_class_state=payload.get("widget_known_class_state", {}),
        widget_inherited_state=payload.get("widget_inherited_state", {}),
        widget_own_state=payload.get("widget_own_state", {}),
        root_shared_entry=payload.get("root_shared_entry", {}),
        widget_shared_entry=payload.get("widget_shared_entry", {}),
        widget_inherited_entry=payload.get("widget_inherited_entry", {}),
        widget_own_entry=payload.get("widget_own_entry", {}),
    )

    expect(payload.get("root_class_value") == 19, "expected root class method dispatch to return 19")
    expect(payload.get("widget_class_value") == 19, "expected Widget class dispatch to inherit RootObject +shared")
    expect(payload.get("widget_known_class_value") == 19, "expected normalized Widget class receiver to reuse inherited class dispatch")
    expect(payload.get("widget_inherited_instance_value") == 17, "expected Widget instance dispatch to inherit RootObject -rootValue")
    expect(payload.get("widget_own_instance_value") == 23, "expected Widget instance dispatch to resolve Widget -widgetValue")
    expect(
        facts.graph_state.get("realized_class_count") == 2
        and facts.graph_state.get("root_class_count") == 1
        and facts.graph_state.get("metaclass_edge_count") == 1
        and facts.graph_state.get("receiver_class_binding_count") == 2
        and facts.graph_state.get("last_realized_class_name") == "Widget"
        and facts.graph_state.get("last_realized_class_owner_identity") == "class:Widget"
        and facts.graph_state.get("last_realized_metaclass_owner_identity") == "metaclass:Widget",
        "expected realized graph to publish RootObject as the sole root and Widget as the single class/metaclass edge",
    )
    expect(
        facts.root_entry.get("found") == 1
        and facts.root_entry.get("base_identity") == 1024
        and facts.root_entry.get("is_root_class") == 1
        and facts.root_entry.get("implementation_backed") == 1
        and facts.root_entry.get("class_name") == "RootObject"
        and facts.root_entry.get("class_owner_identity") == "class:RootObject"
        and facts.root_entry.get("metaclass_owner_identity") == "metaclass:RootObject"
        and facts.root_entry.get("super_class_owner_identity") is None
        and facts.root_entry.get("super_metaclass_owner_identity") is None,
        "expected RootObject entry to realize as an implementation-backed root with null superclass and metaclass-super links",
    )
    expect(
        facts.widget_entry.get("found") == 1
        and facts.widget_entry.get("base_identity") == 1041
        and facts.widget_entry.get("is_root_class") == 0
        and facts.widget_entry.get("implementation_backed") == 1
        and facts.widget_entry.get("class_name") == "Widget"
        and facts.widget_entry.get("class_owner_identity") == "class:Widget"
        and facts.widget_entry.get("metaclass_owner_identity") == "metaclass:Widget"
        and facts.widget_entry.get("super_class_owner_identity") == "class:RootObject"
        and facts.widget_entry.get("super_metaclass_owner_identity") == "metaclass:RootObject",
        "expected Widget entry to publish stable class/metaclass owner identities and RootObject superclass links",
    )
    expect(
        facts.root_class_state.get("last_resolved_class_name") == "RootObject"
        and facts.root_class_state.get("last_resolved_owner_identity") == "implementation:RootObject::class_method:shared"
        and facts.root_class_state.get("last_dispatch_resolved_live_method") == 1,
        "expected RootObject class dispatch to resolve through the live metaclass method list",
    )
    expect(
        facts.widget_class_state.get("last_resolved_class_name") == "RootObject"
        and facts.widget_class_state.get("last_resolved_owner_identity") == "implementation:RootObject::class_method:shared"
        and facts.widget_class_state.get("last_dispatch_resolved_live_method") == 1,
        "expected Widget class dispatch to walk the metaclass superclass chain",
    )
    expect(
        facts.widget_known_class_state.get("last_dispatch_used_cache") == 1
        and facts.widget_known_class_state.get("last_resolved_class_name") == "RootObject",
        "expected repeated Widget class dispatch to reuse the cache while preserving inherited RootObject resolution",
    )
    expect(
        facts.widget_inherited_state.get("last_resolved_class_name") == "RootObject"
        and facts.widget_inherited_state.get("last_resolved_owner_identity") == "implementation:RootObject::instance_method:rootValue",
        "expected Widget instance dispatch to walk the class superclass chain for RootObject -rootValue",
    )
    expect(
        facts.widget_own_state.get("last_resolved_class_name") == "Widget"
        and facts.widget_own_state.get("last_resolved_owner_identity") == "implementation:Widget::instance_method:widgetValue",
        "expected Widget instance dispatch to resolve its own method before walking superclasses",
    )
    for entry_name, entry, expected_owner, expected_class_dispatch in (
        ("root_shared_entry", facts.root_shared_entry, "implementation:RootObject::class_method:shared", 1),
        ("widget_shared_entry", facts.widget_shared_entry, "implementation:RootObject::class_method:shared", 1),
        ("widget_inherited_entry", facts.widget_inherited_entry, "implementation:RootObject::instance_method:rootValue", 0),
        ("widget_own_entry", facts.widget_own_entry, "implementation:Widget::instance_method:widgetValue", 0),
    ):
        expect(
            entry.get("found") == 1
            and entry.get("resolved") == 1
            and entry.get("dispatch_family_is_class") == expected_class_dispatch
            and entry.get("resolved_owner_identity") == expected_owner,
            f"expected {entry_name} to publish a resolved method-cache entry with stable owner identity",
        )

    return facts


def assert_canonical_sample_widget_realization(
    facts: CanonicalSampleProbeFacts,
) -> None:
    expect(facts.widget_entry.get("found") == 1, "expected Widget to realize successfully for the canonical sample set")
    expect(facts.widget_entry.get("base_identity") == 1041, "expected Widget base identity 1041 for the canonical sample set")
    expect(facts.widget_entry.get("runtime_property_accessor_count") == 4, "expected Widget to expose four runtime property accessors")
    expect(facts.widget_entry.get("runtime_instance_size_bytes") == 24, "expected Widget instance size to remain 24 bytes")
    expect(
        facts.widget_entry.get("last_attached_category_owner_identity") == "category:Widget(Tracing)",
        "expected Widget to preserve the attached Tracing category owner",
    )


def assert_canonical_sample_runtime_values(facts: CanonicalSampleProbeFacts) -> None:
    payload = facts.payload
    expect(payload.get("init_value", 0) != 0, "expected alloc/init to return a non-zero canonical sample-set receiver")
    expect(payload.get("traced_value") == 13, "expected tracedValue to return 13 for the canonical sample set")
    expect(payload.get("inherited_value") == 7, "expected inheritedValue to return 7 for the canonical sample set")
    expect(payload.get("class_value") == 11, "expected classValue to return 11 for the canonical sample set")
    expect(payload.get("shared_value") == 19, "expected shared to return 19 for the canonical sample set")
    expect(payload.get("count_value") == 37, "expected count to reload 37 for the canonical sample set")
    expect(payload.get("enabled_value") == 1, "expected enabled to reload 1 for the canonical sample set")
    expect(payload.get("current_value") == 55, "expected currentValue to reload 55 for the canonical sample set")
    expect(payload.get("token_value") == 0, "expected tokenValue to remain 0 for the canonical sample set")


def assert_canonical_sample_protocol_queries(
    facts: CanonicalSampleProbeFacts,
) -> None:
    expect(facts.worker_query.get("conforms") == 1, "expected Widget to conform to Worker in the canonical sample set")
    expect(
        facts.worker_query.get("matched_protocol_owner_identity") in {"protocol:Worker", "protocol:Tracer"},
        "expected Worker query to resolve through Worker or inherited Tracer",
    )
    expect(facts.worker_query.get("matched_attachment_owner_identity") is None, "did not expect Worker query to require an attachment-owner match")
    expect(facts.tracer_query.get("conforms") == 1, "expected Widget to conform to Tracer in the canonical sample set")
    expect(facts.tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer", "expected Tracer query to resolve through protocol:Tracer")
    expect(
        facts.tracer_query.get("matched_attachment_owner_identity") == "category:Widget(Tracing)",
        "expected Tracer query to resolve through the attached category owner",
    )


def assert_canonical_sample_property_reflection(
    facts: CanonicalSampleProbeFacts,
) -> None:
    expect(
        facts.count_property.get("found") == 1
        and facts.count_property.get("slot_index") == 0
        and facts.count_property.get("offset_bytes") == 0
        and facts.count_property.get("size_bytes") == 4
        and facts.count_property.get("getter_owner_identity") == "implementation:Widget::instance_method:count"
        and facts.count_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCount:",
        "expected count property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        facts.value_property.get("found") == 1
        and facts.value_property.get("slot_index") == 2
        and facts.value_property.get("offset_bytes") == 8
        and facts.value_property.get("size_bytes") == 8
        and facts.value_property.get("getter_owner_identity") == "implementation:Widget::instance_method:currentValue"
        and facts.value_property.get("setter_owner_identity") == "implementation:Widget::instance_method:setCurrentValue:",
        "expected currentValue property reflection to preserve slot/layout/accessor facts",
    )
    expect(
        facts.token_property.get("found") == 1
        and facts.token_property.get("slot_index") == 3
        and facts.token_property.get("offset_bytes") == 16
        and facts.token_property.get("setter_available") == 0
        and facts.token_property.get("getter_owner_identity") == "implementation:Widget::instance_method:tokenValue"
        and facts.token_property.get("setter_owner_identity") is None,
        "expected tokenValue property reflection to preserve readonly slot/layout/accessor facts",
    )


__all__ = [
    "CanonicalSampleProbeFacts",
    "MetaclassGraphProbeFacts",
    "assert_canonical_sample_property_reflection",
    "assert_canonical_sample_protocol_queries",
    "assert_canonical_sample_runtime_values",
    "assert_canonical_sample_widget_realization",
    "assert_metaclass_graph_probe_payload",
    "capture_canonical_sample_probe_facts",
]
