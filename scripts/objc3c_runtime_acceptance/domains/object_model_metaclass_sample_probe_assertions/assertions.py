"""Runtime probe assertions for Object Model metaclass sample cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from .catalog import CANONICAL_RUNTIME_VALUE_EXPECTATIONS
from .catalog import CANONICAL_WIDGET_ENTRY_EXPECTATIONS
from .catalog import COUNT_PROPERTY_EXPECTATION
from .catalog import METACLASS_GRAPH_RUNTIME_VALUE_EXPECTATIONS
from .catalog import METACLASS_GRAPH_STATE_EXPECTATION
from .catalog import METHOD_CACHE_ENTRY_EXPECTATIONS
from .catalog import ROOT_CLASS_STATE_EXPECTATION
from .catalog import ROOT_ENTRY_EXPECTATION
from .catalog import TOKEN_PROPERTY_EXPECTATION
from .catalog import VALUE_PROPERTY_EXPECTATION
from .catalog import WIDGET_CLASS_STATE_EXPECTATION
from .catalog import WIDGET_ENTRY_EXPECTATION
from .catalog import WIDGET_INHERITED_STATE_EXPECTATION
from .catalog import WIDGET_KNOWN_CLASS_STATE_EXPECTATION
from .catalog import WIDGET_OWN_STATE_EXPECTATION
from .catalog import WORKER_PROTOCOL_OWNER_IDENTITIES
from .data import CanonicalSampleProbeFacts
from .data import MetaclassGraphProbeFacts
from .predicates import mapping_has_expected_fields


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

    for expectation in METACLASS_GRAPH_RUNTIME_VALUE_EXPECTATIONS:
        expect(payload.get(expectation.key) == expectation.expected_value, expectation.message)

    expect(
        mapping_has_expected_fields(
            facts.graph_state,
            METACLASS_GRAPH_STATE_EXPECTATION.expected_fields,
        ),
        METACLASS_GRAPH_STATE_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(facts.root_entry, ROOT_ENTRY_EXPECTATION.expected_fields),
        ROOT_ENTRY_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.widget_entry,
            WIDGET_ENTRY_EXPECTATION.expected_fields,
        ),
        WIDGET_ENTRY_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.root_class_state,
            ROOT_CLASS_STATE_EXPECTATION.expected_fields,
        ),
        ROOT_CLASS_STATE_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.widget_class_state,
            WIDGET_CLASS_STATE_EXPECTATION.expected_fields,
        ),
        WIDGET_CLASS_STATE_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.widget_known_class_state,
            WIDGET_KNOWN_CLASS_STATE_EXPECTATION.expected_fields,
        ),
        WIDGET_KNOWN_CLASS_STATE_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.widget_inherited_state,
            WIDGET_INHERITED_STATE_EXPECTATION.expected_fields,
        ),
        WIDGET_INHERITED_STATE_EXPECTATION.message,
    )
    expect(
        mapping_has_expected_fields(
            facts.widget_own_state,
            WIDGET_OWN_STATE_EXPECTATION.expected_fields,
        ),
        WIDGET_OWN_STATE_EXPECTATION.message,
    )

    for expectation in METHOD_CACHE_ENTRY_EXPECTATIONS:
        entry = getattr(facts, expectation.entry_name)
        expect(
            entry.get("found") == 1
            and entry.get("resolved") == 1
            and entry.get("dispatch_family_is_class")
            == expectation.expected_class_dispatch
            and entry.get("resolved_owner_identity")
            == expectation.expected_owner_identity,
            f"expected {expectation.entry_name} to publish a resolved method-cache entry with stable owner identity",
        )

    return facts


def assert_canonical_sample_widget_realization(
    facts: CanonicalSampleProbeFacts,
) -> None:
    for expectation in CANONICAL_WIDGET_ENTRY_EXPECTATIONS:
        expect(
            facts.widget_entry.get(expectation.key) == expectation.expected_value,
            expectation.message,
        )


def assert_canonical_sample_runtime_values(facts: CanonicalSampleProbeFacts) -> None:
    payload = facts.payload
    expect(
        payload.get("init_value", 0) != 0,
        "expected alloc/init to return a non-zero canonical sample-set receiver",
    )
    for expectation in CANONICAL_RUNTIME_VALUE_EXPECTATIONS:
        expect(payload.get(expectation.key) == expectation.expected_value, expectation.message)


def assert_canonical_sample_protocol_queries(
    facts: CanonicalSampleProbeFacts,
) -> None:
    expect(
        facts.worker_query.get("conforms") == 1,
        "expected Widget to conform to Worker in the canonical sample set",
    )
    expect(
        facts.worker_query.get("matched_protocol_owner_identity")
        in WORKER_PROTOCOL_OWNER_IDENTITIES,
        "expected Worker query to resolve through Worker or inherited Tracer",
    )
    expect(
        facts.worker_query.get("matched_attachment_owner_identity") is None,
        "did not expect Worker query to require an attachment-owner match",
    )
    expect(
        facts.tracer_query.get("conforms") == 1,
        "expected Widget to conform to Tracer in the canonical sample set",
    )
    expect(
        facts.tracer_query.get("matched_protocol_owner_identity") == "protocol:Tracer",
        "expected Tracer query to resolve through protocol:Tracer",
    )
    expect(
        facts.tracer_query.get("matched_attachment_owner_identity")
        == "category:Widget(Tracing)",
        "expected Tracer query to resolve through the attached category owner",
    )


def assert_canonical_sample_property_reflection(
    facts: CanonicalSampleProbeFacts,
) -> None:
    for payload, expectation in (
        (facts.count_property, COUNT_PROPERTY_EXPECTATION),
        (facts.value_property, VALUE_PROPERTY_EXPECTATION),
        (facts.token_property, TOKEN_PROPERTY_EXPECTATION),
    ):
        expect(
            mapping_has_expected_fields(payload, expectation.expected_fields),
            expectation.message,
        )


__all__ = [
    "assert_canonical_sample_property_reflection",
    "assert_canonical_sample_protocol_queries",
    "assert_canonical_sample_runtime_values",
    "assert_canonical_sample_widget_realization",
    "assert_metaclass_graph_probe_payload",
]
