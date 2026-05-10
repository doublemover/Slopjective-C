"""Dispatch state and cache-entry expectations for canonical object dispatch."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .object_model_dispatch_payload_model import CanonicalDispatchPayload


def assert_dispatch_state_transitions(facts: CanonicalDispatchPayload) -> None:
    selector_handles = facts.selector_handles
    expect(
        facts.inherited_state.get("last_dispatch_used_cache") == 0
        and facts.inherited_state.get("last_dispatch_resolved_live_method") == 1
        and facts.inherited_state.get("last_dispatch_strict_error") == 0
        and facts.inherited_state.get("last_selector_stable_id")
        == selector_handles.get("inheritedValue", 0)
        and facts.inherited_state.get("last_normalized_receiver_identity") == 1042
        and facts.inherited_state.get("last_category_probe_count") == 1
        and facts.inherited_state.get("last_protocol_probe_count") == 3,
        "expected inheritedValue to miss cache first, resolve live, and preserve category/protocol probe counts",
    )
    expect(
        facts.traced_state.get("last_dispatch_used_cache") == 0
        and facts.traced_state.get("last_dispatch_resolved_live_method") == 1
        and facts.traced_state.get("last_dispatch_strict_error") == 0
        and facts.traced_state.get("last_selector_stable_id")
        == selector_handles.get("tracedValue", 0)
        and facts.traced_state.get("last_normalized_receiver_identity") == 1042
        and facts.traced_state.get("last_category_probe_count") == 1
        and facts.traced_state.get("last_protocol_probe_count") == 0,
        "expected tracedValue to resolve live through the attached category without protocol strict error probes",
    )
    expect(
        facts.class_state.get("last_dispatch_used_cache") == 0
        and facts.class_state.get("last_dispatch_resolved_live_method") == 1
        and facts.class_state.get("last_dispatch_strict_error") == 0
        and facts.class_state.get("last_selector_stable_id")
        == selector_handles.get("classValue", 0)
        and facts.class_state.get("last_normalized_receiver_identity") == 1043,
        "expected classValue to resolve live through the metaclass path",
    )
    expect(
        facts.ignored_state.get("last_dispatch_used_cache") == 0
        and facts.ignored_state.get("last_dispatch_resolved_live_method") == 0
        and facts.ignored_state.get("last_dispatch_strict_error") == 1
        and facts.ignored_state.get("last_selector_stable_id")
        == facts.ignored_entry.get("selector_stable_id", 0)
        and facts.ignored_state.get("last_normalized_receiver_identity") == 1042
        and facts.ignored_state.get("last_category_probe_count") == 1
        and facts.ignored_state.get("last_protocol_probe_count") == 3,
        "expected the first ignoredValue dispatch to materialize a negative cache entry and fall back deterministically",
    )
    expect(
        facts.ignored_cached_state.get("last_dispatch_used_cache") == 1
        and facts.ignored_cached_state.get("last_dispatch_resolved_live_method") == 0
        and facts.ignored_cached_state.get("last_dispatch_strict_error") == 1
        and facts.ignored_cached_state.get("last_selector_stable_id")
        == facts.ignored_entry.get("selector_stable_id", 0)
        and facts.ignored_cached_state.get("last_normalized_receiver_identity") == 1042
        and facts.ignored_cached_state.get("last_category_probe_count") == 1
        and facts.ignored_cached_state.get("last_protocol_probe_count") == 3,
        "expected the second ignoredValue dispatch to reuse the negative cache entry and preserve probe counts",
    )


def assert_dispatch_cache_entry_ownership(facts: CanonicalDispatchPayload) -> None:
    expect(
        facts.traced_entry.get("resolved_owner_identity")
        == "implementation:Widget(Tracing)::instance_method:tracedValue",
        "expected tracedValue cache entry to preserve the category implementation owner",
    )
    expect(
        facts.inherited_entry.get("normalized_receiver_identity") == 1042
        and facts.inherited_entry.get("category_probe_count") == 1
        and facts.inherited_entry.get("protocol_probe_count") == 3
        and facts.inherited_entry.get("resolved_class_name") == "Base"
        and facts.inherited_entry.get("resolved_owner_identity")
        == "implementation:Base::instance_method:inheritedValue",
        "expected inheritedValue cache entry to preserve the instance-family lookup result through Base",
    )
    expect(
        facts.class_entry.get("dispatch_family_is_class") == 1
        and facts.class_entry.get("normalized_receiver_identity") == 1043
        and facts.class_entry.get("resolved_class_name") == "Widget"
        and facts.class_entry.get("resolved_owner_identity")
        == "implementation:Widget::class_method:classValue",
        "expected classValue cache entry to preserve the metaclass lookup result",
    )
    expect(
        facts.ignored_entry.get("found") == 1
        and facts.ignored_entry.get("resolved") == 0
        and facts.ignored_entry.get("dispatch_family_is_class") == 0
        and facts.ignored_entry.get("normalized_receiver_identity") == 1042
        and facts.ignored_entry.get("category_probe_count") == 1
        and facts.ignored_entry.get("protocol_probe_count") == 3
        and facts.ignored_entry.get("selector") == "ignoredValue",
        "expected ignoredValue to preserve an unresolved negative cache entry on the canonical instance receiver",
    )


__all__ = [
    "assert_dispatch_cache_entry_ownership",
    "assert_dispatch_state_transitions",
]
