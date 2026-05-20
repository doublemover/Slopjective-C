"""Assertions for storage/reflection property metadata runtime acceptance."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_property_reflection_payload import (
    PropertyReflectionPayload,
)


def _profile_has(profile: object, *tokens: str) -> bool:
    text = profile if isinstance(profile, str) else ""
    return all(token in text for token in tokens)


def _has_fields(prop: dict[str, object], **expected: int) -> bool:
    return all(prop.get(field) == value for field, value in expected.items())


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
        _profile_has(
            facts.token_property.get("property_attribute_profile"),
            "readonly=1",
            "getter=tokenValue",
            "behavior=Projected",
            "attributes=behavior=Projected,getter=tokenValue,readonly",
        ),
        "expected token property reflection to preserve readonly, behavior, and custom getter attributes",
    )
    expect(
        facts.token_property.get("property_behavior_name") == "Projected",
        "expected token property reflection to publish projected behavior name",
    )
    expect(
        _has_fields(
            facts.token_property,
            attribute_count=3,
            is_readonly=1,
            is_nonatomic=0,
            is_strong=0,
            is_assign=0,
            is_weak=0,
            is_copy=0,
            has_custom_getter=1,
            has_custom_setter=0,
        ),
        "expected token property reflection to expose structured readonly/custom-getter attributes",
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
        _profile_has(
            facts.value_property.get("property_attribute_profile"),
            "nonatomic=1",
            "strong=1",
            "getter=currentValue",
            "setter=setCurrentValue:",
            "behavior=Observed",
            "attributes=behavior=Observed,getter=currentValue,nonatomic,setter=setCurrentValue:,strong",
        ),
        "expected value property reflection to preserve nonatomic, strong, behavior, getter, and setter attributes",
    )
    expect(
        facts.value_property.get("property_behavior_name") == "Observed",
        "expected value property reflection to publish observed behavior name",
    )
    expect(
        _has_fields(
            facts.value_property,
            attribute_count=5,
            is_readonly=0,
            is_nonatomic=1,
            is_strong=1,
            is_assign=0,
            is_weak=0,
            is_copy=0,
            has_custom_getter=1,
            has_custom_setter=1,
        ),
        "expected value property reflection to expose structured nonatomic/strong/custom-accessor attributes",
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
        _profile_has(
            facts.count_property.get("property_attribute_profile"),
            "assign=1",
            "setter=setCount:",
            "attributes=assign,setter=setCount:",
        ),
        "expected count property reflection to preserve assign and custom setter attributes",
    )
    expect(
        facts.count_property.get("property_behavior_name") is None,
        "expected count property reflection to leave behavior name absent",
    )
    expect(
        _has_fields(
            facts.count_property,
            attribute_count=2,
            is_readonly=0,
            is_nonatomic=0,
            is_strong=0,
            is_assign=1,
            is_weak=0,
            is_copy=0,
            has_custom_getter=0,
            has_custom_setter=1,
        ),
        "expected count property reflection to expose structured assign/custom-setter attributes",
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
