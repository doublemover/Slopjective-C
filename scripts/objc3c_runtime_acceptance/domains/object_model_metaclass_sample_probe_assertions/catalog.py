"""Expected runtime results for Object Model metaclass sample probes."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class FieldExpectation:
    key: str
    expected_value: Any
    message: str


@dataclass(frozen=True)
class MappingExpectation:
    expected_fields: dict[str, Any]
    message: str


@dataclass(frozen=True)
class CacheEntryExpectation:
    entry_name: str
    expected_owner_identity: str
    expected_class_dispatch: int


METACLASS_GRAPH_RUNTIME_VALUE_EXPECTATIONS = (
    FieldExpectation(
        key="root_class_value",
        expected_value=19,
        message="expected root class method dispatch to return 19",
    ),
    FieldExpectation(
        key="widget_class_value",
        expected_value=19,
        message="expected Widget class dispatch to inherit RootObject +shared",
    ),
    FieldExpectation(
        key="widget_known_class_value",
        expected_value=19,
        message="expected normalized Widget class receiver to reuse inherited class dispatch",
    ),
    FieldExpectation(
        key="widget_inherited_instance_value",
        expected_value=17,
        message="expected Widget instance dispatch to inherit RootObject -rootValue",
    ),
    FieldExpectation(
        key="widget_own_instance_value",
        expected_value=23,
        message="expected Widget instance dispatch to resolve Widget -widgetValue",
    ),
    FieldExpectation(
        key="widget_super_instance_value",
        expected_value=17,
        message=(
            "expected Widget super dispatch from RootObject to resolve "
            "RootObject -rootValue"
        ),
    ),
    FieldExpectation(
        key="widget_super_own_selector_status",
        expected_value=-1,
        message="expected Widget super dispatch from RootObject to reject Widget-only selectors",
    ),
)

METACLASS_GRAPH_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "realized_class_count": 2,
        "root_class_count": 1,
        "metaclass_edge_count": 1,
        "receiver_class_binding_count": 2,
        "last_realized_class_name": "Widget",
        "last_realized_class_owner_identity": "class:Widget",
        "last_realized_metaclass_owner_identity": "metaclass:Widget",
    },
    message="expected realized graph to publish RootObject as the sole root and Widget as the single class/metaclass edge",
)

ROOT_ENTRY_EXPECTATION = MappingExpectation(
    expected_fields={
        "found": 1,
        "base_identity": 1024,
        "instance_receiver_identity": 1025,
        "class_receiver_identity": 1026,
        "is_root_class": 1,
        "has_super_node": 0,
        "implementation_backed": 1,
        "super_base_identity": 0,
        "class_name": "RootObject",
        "class_owner_identity": "class:RootObject",
        "metaclass_owner_identity": "metaclass:RootObject",
        "super_class_owner_identity": None,
        "super_metaclass_owner_identity": None,
        "instance_isa_owner_identity": "class:RootObject",
        "class_object_isa_owner_identity": "metaclass:RootObject",
        "metaclass_object_isa_owner_identity": "metaclass:RootObject",
        "root_class_owner_identity": "class:RootObject",
        "root_metaclass_owner_identity": "metaclass:RootObject",
    },
    message=(
        "expected RootObject entry to realize as an implementation-backed root "
        "with self-consistent class/metaclass isa and null superclass links"
    ),
)

WIDGET_ENTRY_EXPECTATION = MappingExpectation(
    expected_fields={
        "found": 1,
        "base_identity": 1041,
        "instance_receiver_identity": 1042,
        "class_receiver_identity": 1043,
        "is_root_class": 0,
        "has_super_node": 1,
        "implementation_backed": 1,
        "super_base_identity": 1024,
        "class_name": "Widget",
        "class_owner_identity": "class:Widget",
        "metaclass_owner_identity": "metaclass:Widget",
        "super_class_owner_identity": "class:RootObject",
        "super_metaclass_owner_identity": "metaclass:RootObject",
        "instance_isa_owner_identity": "class:Widget",
        "class_object_isa_owner_identity": "metaclass:Widget",
        "metaclass_object_isa_owner_identity": "metaclass:RootObject",
        "root_class_owner_identity": "class:RootObject",
        "root_metaclass_owner_identity": "metaclass:RootObject",
    },
    message="expected Widget entry to publish stable class/metaclass owner identities and RootObject class/metaclass superclass links",
)

ROOT_CLASS_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "last_resolved_class_name": "RootObject",
        "last_resolved_owner_identity": "implementation:RootObject::class_method:shared",
        "last_dispatch_resolved_live_method": 1,
    },
    message="expected RootObject class dispatch to resolve through the live metaclass method list",
)

WIDGET_CLASS_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "last_resolved_class_name": "RootObject",
        "last_resolved_owner_identity": "implementation:RootObject::class_method:shared",
        "last_dispatch_resolved_live_method": 1,
    },
    message="expected Widget class dispatch to walk the metaclass superclass chain",
)

WIDGET_KNOWN_CLASS_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "last_dispatch_used_cache": 1,
        "last_resolved_class_name": "RootObject",
    },
    message="expected repeated Widget class dispatch to reuse the cache while preserving inherited RootObject resolution",
)

WIDGET_INHERITED_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "last_resolved_class_name": "RootObject",
        "last_resolved_owner_identity": "implementation:RootObject::instance_method:rootValue",
    },
    message="expected Widget instance dispatch to walk the class superclass chain for RootObject -rootValue",
)

WIDGET_OWN_STATE_EXPECTATION = MappingExpectation(
    expected_fields={
        "last_resolved_class_name": "Widget",
        "last_resolved_owner_identity": "implementation:Widget::instance_method:widgetValue",
    },
    message="expected Widget instance dispatch to resolve its own method before walking superclasses",
)

METHOD_CACHE_ENTRY_EXPECTATIONS = (
    CacheEntryExpectation(
        entry_name="root_shared_entry",
        expected_owner_identity="implementation:RootObject::class_method:shared",
        expected_class_dispatch=1,
    ),
    CacheEntryExpectation(
        entry_name="widget_shared_entry",
        expected_owner_identity="implementation:RootObject::class_method:shared",
        expected_class_dispatch=1,
    ),
    CacheEntryExpectation(
        entry_name="widget_inherited_entry",
        expected_owner_identity="implementation:RootObject::instance_method:rootValue",
        expected_class_dispatch=0,
    ),
    CacheEntryExpectation(
        entry_name="widget_own_entry",
        expected_owner_identity="implementation:Widget::instance_method:widgetValue",
        expected_class_dispatch=0,
    ),
    CacheEntryExpectation(
        entry_name="widget_super_entry",
        expected_owner_identity="implementation:RootObject::instance_method:rootValue",
        expected_class_dispatch=0,
    ),
)

FAIL_CLOSED_DIAGNOSTICS_EXPECTATION = MappingExpectation(
    expected_fields={
        "root_super_metadata_rejected": 1,
        "root_super_metadata_reason": (
            "root class publishes superclass metaclass metadata for BrokenRoot"
        ),
        "subclass_missing_super_metadata_rejected": 1,
        "subclass_missing_super_metadata_reason": (
            "subclass class/metaclass superclass edge is incomplete for BrokenChild"
        ),
        "subclass_metaclass_link_mismatch_rejected": 1,
        "subclass_metaclass_link_mismatch_reason": (
            "subclass metaclass superclass owner does not match realized "
            "superclass metaclass for BrokenChild"
        ),
    },
    message=(
        "expected malformed metaclass metadata diagnostics to fail closed with "
        "stable reasons"
    ),
)

CANONICAL_WIDGET_ENTRY_EXPECTATIONS = (
    FieldExpectation(
        key="found",
        expected_value=1,
        message="expected Widget to realize successfully for the canonical sample set",
    ),
    FieldExpectation(
        key="base_identity",
        expected_value=1041,
        message="expected Widget base identity 1041 for the canonical sample set",
    ),
    FieldExpectation(
        key="runtime_property_accessor_count",
        expected_value=4,
        message="expected Widget to expose four runtime property accessors",
    ),
    FieldExpectation(
        key="runtime_instance_size_bytes",
        expected_value=24,
        message="expected Widget instance size to remain 24 bytes",
    ),
    FieldExpectation(
        key="last_attached_category_owner_identity",
        expected_value="category:Widget(Tracing)",
        message="expected Widget to preserve the attached Tracing category owner",
    ),
)

CANONICAL_RUNTIME_VALUE_EXPECTATIONS = (
    FieldExpectation(
        key="traced_value",
        expected_value=13,
        message="expected tracedValue to return 13 for the canonical sample set",
    ),
    FieldExpectation(
        key="inherited_value",
        expected_value=7,
        message="expected inheritedValue to return 7 for the canonical sample set",
    ),
    FieldExpectation(
        key="class_value",
        expected_value=11,
        message="expected classValue to return 11 for the canonical sample set",
    ),
    FieldExpectation(
        key="shared_value",
        expected_value=19,
        message="expected shared to return 19 for the canonical sample set",
    ),
    FieldExpectation(
        key="count_value",
        expected_value=37,
        message="expected count to reload 37 for the canonical sample set",
    ),
    FieldExpectation(
        key="enabled_value",
        expected_value=1,
        message="expected enabled to reload 1 for the canonical sample set",
    ),
    FieldExpectation(
        key="current_value",
        expected_value=55,
        message="expected currentValue to reload 55 for the canonical sample set",
    ),
    FieldExpectation(
        key="token_value",
        expected_value=0,
        message="expected tokenValue to remain 0 for the canonical sample set",
    ),
)

COUNT_PROPERTY_EXPECTATION = MappingExpectation(
    expected_fields={
        "found": 1,
        "slot_index": 0,
        "offset_bytes": 0,
        "size_bytes": 4,
        "getter_owner_identity": "interface:Widget::instance_method:count",
        "setter_owner_identity": "interface:Widget::instance_method:setCount:",
    },
    message="expected count property reflection to preserve slot/layout/accessor facts",
)

VALUE_PROPERTY_EXPECTATION = MappingExpectation(
    expected_fields={
        "found": 1,
        "slot_index": 2,
        "offset_bytes": 8,
        "size_bytes": 8,
        "getter_owner_identity": "interface:Widget::instance_method:currentValue",
        "setter_owner_identity": "interface:Widget::instance_method:setCurrentValue:",
    },
    message="expected currentValue property reflection to preserve slot/layout/accessor facts",
)

TOKEN_PROPERTY_EXPECTATION = MappingExpectation(
    expected_fields={
        "found": 1,
        "slot_index": 3,
        "offset_bytes": 16,
        "setter_available": 0,
        "getter_owner_identity": "interface:Widget::instance_method:tokenValue",
        "setter_owner_identity": None,
    },
    message="expected tokenValue property reflection to preserve readonly slot/layout/accessor facts",
)

WORKER_PROTOCOL_OWNER_IDENTITIES = frozenset(("protocol:Worker", "protocol:Tracer"))


__all__ = [
    "CANONICAL_RUNTIME_VALUE_EXPECTATIONS",
    "CANONICAL_WIDGET_ENTRY_EXPECTATIONS",
    "COUNT_PROPERTY_EXPECTATION",
    "CacheEntryExpectation",
    "FAIL_CLOSED_DIAGNOSTICS_EXPECTATION",
    "FieldExpectation",
    "MappingExpectation",
    "METACLASS_GRAPH_RUNTIME_VALUE_EXPECTATIONS",
    "METACLASS_GRAPH_STATE_EXPECTATION",
    "METHOD_CACHE_ENTRY_EXPECTATIONS",
    "ROOT_CLASS_STATE_EXPECTATION",
    "ROOT_ENTRY_EXPECTATION",
    "TOKEN_PROPERTY_EXPECTATION",
    "VALUE_PROPERTY_EXPECTATION",
    "WIDGET_CLASS_STATE_EXPECTATION",
    "WIDGET_ENTRY_EXPECTATION",
    "WIDGET_INHERITED_STATE_EXPECTATION",
    "WIDGET_KNOWN_CLASS_STATE_EXPECTATION",
    "WIDGET_OWN_STATE_EXPECTATION",
    "WORKER_PROTOCOL_OWNER_IDENTITIES",
]
