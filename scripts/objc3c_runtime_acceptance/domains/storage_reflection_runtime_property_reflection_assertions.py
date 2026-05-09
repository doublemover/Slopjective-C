"""Assertions for storage/reflection property metadata runtime acceptance."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_property_reflection_payload import (
    PropertyReflectionPayload,
)


def assert_property_reflection_payload(facts: PropertyReflectionPayload) -> None:
    expect(
        facts.widget_entry.get("found") == 1,
        "expected Widget realized class entry to be present",
    )
    expect(
        facts.token_property.get("found") == 1,
        "expected token property to be reflectable",
    )
    expect(
        facts.token_property.get("setter_available") == 0,
        "expected readonly token property to have no setter",
    )
    expect(
        facts.token_property.get("has_runtime_getter") == 1,
        "expected token property getter to be runtime-backed",
    )
    expect(
        facts.value_property.get("found") == 1,
        "expected value property to be reflectable",
    )
    expect(
        facts.value_property.get("setter_available") == 1,
        "expected value property to expose a setter",
    )
    expect(
        facts.value_property.get("has_runtime_getter") == 1
        and facts.value_property.get("has_runtime_setter") == 1,
        "expected value property getter/setter to be runtime-backed",
    )
    expect(
        facts.count_property.get("found") == 1,
        "expected count property to be reflectable",
    )
    expect(
        facts.count_property.get("has_runtime_getter") == 1
        and facts.count_property.get("has_runtime_setter") == 1,
        "expected count property getter/setter to be runtime-backed",
    )
    expect(
        facts.registry_after_count.get("slot_backed_property_count", 0) >= 3,
        "expected slot-backed property registry to include the three Widget properties",
    )
    expect(
        facts.missing_property.get("found") == 0,
        "expected missing property lookup to fail closed",
    )
    expect(
        facts.missing_class_property.get("found") == 0,
        "expected missing class property lookup to fail closed",
    )


__all__ = ["assert_property_reflection_payload"]
