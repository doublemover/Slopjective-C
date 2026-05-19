"""Selector and cache-key expectations for canonical object dispatch."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .object_model_dispatch_payload_model import CanonicalDispatchPayload


def assert_dispatch_selector_cache_identity(facts: CanonicalDispatchPayload) -> None:
    selector_handles = facts.selector_handles
    expect(
        facts.method_state.get("live_dispatch_count", 0) >= 6,
        "expected live dispatch count to cover alloc/init/new/traced/inherited/class",
    )
    expect(
        facts.method_state.get("strict_dispatch_error_count", 0) == 2,
        "expected canonical dispatch workload to publish both unresolved strict error calls",
    )
    expect(
        facts.method_state.get("last_selector_stable_id", 0)
        == facts.ignored_entry.get("selector_stable_id", 0),
        "expected last dispatch selector stable id to match the negative-cache ignoredValue selector",
    )
    expect(
        selector_handles.get("alloc", 0) != 0
        and selector_handles.get("tracedValue", 0) != 0,
        "expected canonical dispatch selectors to be interned in the runtime selector pool",
    )
    expect(
        facts.alloc_entry.get("selector_stable_id", 0) == selector_handles.get("alloc", 0),
        "expected alloc cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.init_entry.get("selector_stable_id", 0) == selector_handles.get("init", 0),
        "expected init cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.new_entry.get("selector_stable_id", 0) == selector_handles.get("new", 0),
        "expected new cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.traced_entry.get("selector_stable_id", 0)
        == selector_handles.get("tracedValue", 0),
        "expected tracedValue cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.inherited_entry.get("selector_stable_id", 0)
        == selector_handles.get("inheritedValue", 0),
        "expected inheritedValue cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.class_entry.get("selector_stable_id", 0)
        == selector_handles.get("classValue", 0),
        "expected classValue cache entry to be keyed by the selector pool stable id",
    )
    expect(
        facts.traced_entry.get("resolved") == 1,
        "expected tracedValue cache entry to resolve live",
    )
    expect(
        facts.inherited_entry.get("resolved") == 1,
        "expected inheritedValue cache entry to resolve live",
    )
    expect(
        facts.class_entry.get("resolved") == 1,
        "expected classValue cache entry to resolve live",
    )


def assert_dispatch_selector_table_entries(facts: CanonicalDispatchPayload) -> None:
    expect(
        facts.selector_table_state.get("metadata_backed_selector_count", 0) >= 4,
        "expected canonical dispatch selector materialization to keep metadata-backed selectors interned",
    )
    expect(
        facts.selector_table_state.get("dynamic_selector_count", 0) >= 1,
        "expected unresolved selector dispatch to intern a dynamic selector entry",
    )
    expect(
        facts.selector_table_state.get("last_materialized_selector") == "ignoredValue",
        "expected ignoredValue to be the last materialized selector after the strict error probe",
    )
    expect(
        facts.selector_table_state.get("last_materialized_from_metadata") == 0,
        "expected ignoredValue to be recorded as a dynamic selector lookup",
    )
    expect(
        facts.traced_selector_entry.get("found") == 1
        and facts.traced_selector_entry.get("metadata_backed") == 1
        and facts.traced_selector_entry.get("canonical_selector") == "tracedValue",
        "expected tracedValue to remain metadata-backed in the selector table",
    )
    expect(
        facts.inherited_selector_entry.get("found") == 1
        and facts.inherited_selector_entry.get("metadata_backed") == 1
        and facts.inherited_selector_entry.get("canonical_selector") == "inheritedValue",
        "expected inheritedValue to remain metadata-backed in the selector table",
    )
    expect(
        facts.class_selector_entry.get("found") == 1
        and facts.class_selector_entry.get("metadata_backed") == 1
        and facts.class_selector_entry.get("canonical_selector") == "classValue",
        "expected classValue to remain metadata-backed in the selector table",
    )
    expect(
        facts.ignored_selector_entry.get("found") == 1
        and facts.ignored_selector_entry.get("metadata_backed") == 0
        and facts.ignored_selector_entry.get("canonical_selector") == "ignoredValue",
        "expected ignoredValue to materialize as a dynamic selector-table entry",
    )


__all__ = [
    "assert_dispatch_selector_cache_identity",
    "assert_dispatch_selector_table_entries",
]
