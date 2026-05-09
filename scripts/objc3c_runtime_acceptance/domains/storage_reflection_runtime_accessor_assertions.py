"""Assertions for synthesized accessor runtime acceptance."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_accessor_payload import (
    SynthesizedAccessorRuntimePayload,
)


def assert_synthesized_accessor_runtime_payload(
    facts: SynthesizedAccessorRuntimePayload,
) -> None:
    _assert_synthesized_accessor_values(facts)
    _assert_synthesized_accessor_runtime_tables(facts)
    _assert_synthesized_accessor_cache_entries(facts)


def _assert_synthesized_accessor_values(
    facts: SynthesizedAccessorRuntimePayload,
) -> None:
    expect(
        facts.payload.get("widget_instance", 0) > 0,
        "expected synthesized-accessor runtime probe to allocate a positive Widget receiver",
    )
    expect(
        facts.payload.get("set_count_result") == 0,
        "expected synthesized-accessor count setter dispatch to return zero",
    )
    expect(
        facts.payload.get("count_value") == 37,
        "expected synthesized-accessor count getter to reload 37",
    )
    expect(
        facts.payload.get("set_enabled_result") == 0,
        "expected synthesized-accessor enabled setter dispatch to return zero",
    )
    expect(
        facts.payload.get("enabled_value") == 1,
        "expected synthesized-accessor enabled getter to reload 1",
    )
    expect(
        facts.payload.get("set_value_result") == 0,
        "expected synthesized-accessor value setter dispatch to return zero",
    )
    expect(
        facts.payload.get("value_result") == 55,
        "expected synthesized-accessor value getter to reload 55",
    )


def _assert_synthesized_accessor_runtime_tables(
    facts: SynthesizedAccessorRuntimePayload,
) -> None:
    expect(
        facts.registration_state.get("registered_image_count", 0) >= 1,
        "expected synthesized-accessor runtime probe to report at least one registered image",
    )
    expect(
        facts.registration_state.get("registered_descriptor_total", 0) >= 1,
        "expected synthesized-accessor runtime probe to report a non-zero descriptor total",
    )
    expect(
        facts.selector_state.get("selector_table_entry_count", 0) >= 6,
        "expected synthesized-accessor runtime probe to materialize the accessor selector surface",
    )
    expect(
        facts.selector_state.get("metadata_backed_selector_count", 0) >= 6,
        "expected synthesized-accessor runtime probe to preserve metadata-backed selectors",
    )


def _assert_synthesized_accessor_cache_entries(
    facts: SynthesizedAccessorRuntimePayload,
) -> None:
    expected_entries: tuple[tuple[Any, str, int, str], ...] = (
        (facts.count_entry, "count", 0, "implementation:Widget::instance_method:count"),
        (facts.set_count_entry, "setCount:", 1, "implementation:Widget::instance_method:setCount:"),
        (facts.enabled_entry, "enabled", 0, "implementation:Widget::instance_method:enabled"),
        (facts.set_enabled_entry, "setEnabled:", 1, "implementation:Widget::instance_method:setEnabled:"),
        (facts.value_entry, "value", 0, "implementation:Widget::instance_method:value"),
        (facts.set_value_entry, "setValue:", 1, "implementation:Widget::instance_method:setValue:"),
    )
    for entry, selector, parameter_count, owner_identity in expected_entries:
        expect(
            entry.get("found") == 1 and entry.get("resolved") == 1,
            f"expected {selector} cache entry to resolve live",
        )
        expect(
            entry.get("selector") == selector,
            f"expected {selector} cache entry to preserve selector spelling",
        )
        expect(
            entry.get("parameter_count") == parameter_count,
            f"expected {selector} cache entry to preserve parameter count {parameter_count}",
        )
        expect(
            entry.get("resolved_class_name") == "Widget",
            f"expected {selector} cache entry to resolve against Widget",
        )
        expect(
            entry.get("resolved_owner_identity") == owner_identity,
            f"expected {selector} cache entry to preserve owner identity",
        )


__all__ = ["assert_synthesized_accessor_runtime_payload"]
