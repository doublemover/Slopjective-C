"""Property layout assertions for storage/reflection runtime probes."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_layout_payload import PropertyLayoutPayload


def assert_property_layout_payload(facts: PropertyLayoutPayload) -> None:
    _assert_property_layout_ir_surface(facts)
    _assert_property_layout_runtime_values(facts)
    _assert_property_layout_runtime_tables(facts)
    _assert_property_layout_cache_entries(facts)


def _assert_property_layout_ir_surface(facts: PropertyLayoutPayload) -> None:
    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in facts.ll_text,
        "expected LLVM IR to publish the runtime property/layout consumption surface",
    )
    expect(
        "synthesized_accessor_entries=6" in facts.ll_text,
        "expected property-layout fixture to preserve six synthesized accessors",
    )
    expect(
        "property_descriptor_entries=" in facts.ll_text,
        "expected property-layout fixture to publish property descriptor inventory",
    )
    expect(
        "ivar_layout_owner_entries=" in facts.ll_text,
        "expected property-layout fixture to publish ivar layout owner inventory",
    )


def _assert_property_layout_runtime_values(facts: PropertyLayoutPayload) -> None:
    expect(
        facts.first_alloc > 0,
        "expected first alloc to materialize a positive Widget instance identity",
    )
    expect(
        facts.second_alloc > 0,
        "expected second alloc to materialize a positive Widget instance identity",
    )
    expect(
        facts.first_alloc != facts.second_alloc,
        "expected property-layout runtime to allocate distinct Widget instance identities",
    )
    expect(facts.payload.get("set_count_result") == 0, "expected count setter dispatch to return zero")
    expect(
        facts.payload.get("count_value_first") == 37,
        "expected count getter to observe the written value on the first alloc",
    )
    expect(
        facts.payload.get("count_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance count storage",
    )
    expect(facts.payload.get("set_enabled_result") == 0, "expected enabled setter dispatch to return zero")
    expect(
        facts.payload.get("enabled_value_second") == 0,
        "expected second alloc to observe zero-filled per-instance enabled storage",
    )
    expect(facts.payload.get("set_value_result") == 0, "expected value setter dispatch to return zero")
    expect(
        facts.payload.get("value_result_second") == 0,
        "expected second alloc to observe zero-filled per-instance strong value storage",
    )


def _assert_property_layout_runtime_tables(facts: PropertyLayoutPayload) -> None:
    expect(
        facts.registration_state.get("registered_image_count", 0) >= 1,
        "expected property-layout runtime to report at least one registered image",
    )
    expect(
        facts.registration_state.get("registered_descriptor_total", 0) >= 1,
        "expected property-layout runtime to report a non-zero descriptor total",
    )
    expect(
        facts.selector_state.get("selector_table_entry_count", 0) >= 6,
        "expected property-layout runtime to materialize the synthesized accessor selector surface",
    )


def _assert_property_layout_cache_entries(facts: PropertyLayoutPayload) -> None:
    expect(
        facts.count_entry.get("found") == 1 and facts.count_entry.get("resolved") == 1,
        "expected count getter cache entry to resolve",
    )
    expect(
        facts.count_entry.get("parameter_count") == 0,
        "expected count getter cache entry to preserve zero parameters",
    )
    expect(
        str(facts.count_entry.get("resolved_owner_identity", "")).endswith(
            "Widget::instance_method:count"
        ),
        "expected count getter cache entry to preserve the synthesized owner identity",
    )
    expect(
        facts.set_count_entry.get("found") == 1
        and facts.set_count_entry.get("resolved") == 1,
        "expected setCount setter cache entry to resolve",
    )
    expect(
        facts.set_count_entry.get("parameter_count") == 1,
        "expected setCount setter cache entry to preserve one parameter",
    )
    expect(
        str(facts.set_count_entry.get("resolved_owner_identity", "")).endswith(
            "Widget::instance_method:setCount:"
        ),
        "expected setCount setter cache entry to preserve the synthesized owner identity",
    )


__all__ = ["assert_property_layout_payload"]
