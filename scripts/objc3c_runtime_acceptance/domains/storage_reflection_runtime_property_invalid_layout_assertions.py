"""Assertions for malformed property/ivar layout runtime acceptance."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_property_invalid_layout_payload import (
    PropertyInvalidLayoutPayload,
)


def assert_property_invalid_layout_payload(
    facts: PropertyInvalidLayoutPayload,
) -> None:
    expect(
        facts.registration_status == 0,
        "expected malformed property/ivar layout image registration to remain OK",
    )
    expect(
        facts.strict_published_layout == 0,
        "expected RuntimePropertyIvarDescriptorHasStrictPublishedLayout to reject the disagreed layout",
    )
    expect(
        facts.image_walk.get("last_registration_used_staged_table") == 1,
        "expected invalid-layout proof to consume a staged registration table",
    )
    expect(
        facts.image_walk.get("last_walked_property_descriptor_count") == 1,
        "expected invalid-layout proof to publish one property descriptor",
    )
    expect(
        facts.image_walk.get("last_walked_ivar_descriptor_count") == 1,
        "expected invalid-layout proof to publish one ivar descriptor",
    )
    expect(
        facts.class_entry.get("found") == 1,
        "expected invalid-layout class to realize despite rejected property layout",
    )
    expect(
        facts.class_entry.get("runtime_property_accessor_count") == 0,
        "expected invalid-layout class to expose no runtime property accessors",
    )
    expect(
        facts.class_entry.get("runtime_instance_size_bytes") == 0,
        "expected invalid-layout class not to materialize slot-backed storage",
    )
    expect(
        facts.property_entry.get("found") == 0,
        "expected malformed property to have no reflection hit",
    )
    expect(
        facts.property_entry.get("has_runtime_getter") == 0
        and facts.property_entry.get("has_runtime_setter") == 0,
        "expected malformed property to expose no runtime accessor hit",
    )
    expect(
        facts.registry_state.get("last_query_found") == 0,
        "expected malformed property lookup to fail closed",
    )
    expect(
        facts.registry_state.get("reflectable_property_count") == 0,
        "expected malformed property not to become reflectable",
    )
    expect(
        facts.registry_state.get("slot_backed_property_count") == 0,
        "expected malformed layout not to become slot-backed",
    )


__all__ = ["assert_property_invalid_layout_payload"]
