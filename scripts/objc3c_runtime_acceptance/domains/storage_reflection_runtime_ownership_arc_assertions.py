"""ARC, weak, strong, and unowned storage ownership assertions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_payload_assertions import (
    StorageOwnershipReflectionFacts,
)


@dataclass(frozen=True)
class ExpectedOwnershipProperty:
    property_name: str
    slot_index: int
    property_attribute_profile: str
    ownership_lifetime_profile: str
    ownership_runtime_hook_profile: str | None
    accessor_ownership_profile: str
    effective_getter_selector: str
    effective_setter_selector: str


EXPECTED_OWNERSHIP_PROPERTIES: dict[str, ExpectedOwnershipProperty] = {
    "current_value_property": ExpectedOwnershipProperty(
        property_name="currentValue",
        slot_index=0,
        property_attribute_profile=(
            "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;"
            "strong=1;weak=0;unowned=0;unsafe_unretained=0;assign=0;"
            "nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;"
            "attributes=nonatomic,strong"
        ),
        ownership_lifetime_profile="strong-owned",
        ownership_runtime_hook_profile=None,
        accessor_ownership_profile=(
            "getter=currentValue;setter_available=1;setter=setCurrentValue:;"
            "ownership_lifetime=strong-owned;runtime_hook="
        ),
        effective_getter_selector="currentValue",
        effective_setter_selector="setCurrentValue:",
    ),
    "copied_value_property": ExpectedOwnershipProperty(
        property_name="copiedValue",
        slot_index=1,
        property_attribute_profile=(
            "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=1;retain=0;"
            "strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=0;"
            "nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;"
            "attributes=copy,nonatomic"
        ),
        ownership_lifetime_profile="strong-owned",
        ownership_runtime_hook_profile=None,
        accessor_ownership_profile=(
            "getter=copiedValue;setter_available=1;setter=setCopiedValue:;"
            "ownership_lifetime=strong-owned;runtime_hook="
        ),
        effective_getter_selector="copiedValue",
        effective_setter_selector="setCopiedValue:",
    ),
    "weak_value_property": ExpectedOwnershipProperty(
        property_name="weakValue",
        slot_index=2,
        property_attribute_profile=(
            "readonly=0;readwrite=0;atomic=0;nonatomic=1;copy=0;retain=0;"
            "strong=0;weak=1;unowned=0;unsafe_unretained=0;assign=0;"
            "nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;"
            "attributes=nonatomic,weak"
        ),
        ownership_lifetime_profile="weak",
        ownership_runtime_hook_profile="objc-weak-side-table",
        accessor_ownership_profile=(
            "getter=weakValue;setter_available=1;setter=setWeakValue:;"
            "ownership_lifetime=weak;runtime_hook=objc-weak-side-table"
        ),
        effective_getter_selector="weakValue",
        effective_setter_selector="setWeakValue:",
    ),
    "borrowed_value_property": ExpectedOwnershipProperty(
        property_name="borrowedValue",
        slot_index=3,
        property_attribute_profile=(
            "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;"
            "strong=0;weak=0;unowned=0;unsafe_unretained=0;assign=1;"
            "nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;"
            "attributes=assign"
        ),
        ownership_lifetime_profile="unowned-unsafe",
        ownership_runtime_hook_profile="objc-unowned-unsafe-direct",
        accessor_ownership_profile=(
            "getter=borrowedValue;setter_available=1;setter=setBorrowedValue:;"
            "ownership_lifetime=unowned-unsafe;runtime_hook=objc-unowned-unsafe-direct"
        ),
        effective_getter_selector="borrowedValue",
        effective_setter_selector="setBorrowedValue:",
    ),
    "guarded_value_property": ExpectedOwnershipProperty(
        property_name="guardedValue",
        slot_index=4,
        property_attribute_profile=(
            "readonly=0;readwrite=0;atomic=0;nonatomic=0;copy=0;retain=0;"
            "strong=0;weak=0;unowned=1;unsafe_unretained=0;assign=0;"
            "nullable=0;nonnull=0;null_resettable=0;class=0;direct=0;"
            "attributes=unowned"
        ),
        ownership_lifetime_profile="unowned-safe",
        ownership_runtime_hook_profile="objc-unowned-safe-guard",
        accessor_ownership_profile=(
            "getter=guardedValue;setter_available=1;setter=setGuardedValue:;"
            "ownership_lifetime=unowned-safe;runtime_hook=objc-unowned-safe-guard"
        ),
        effective_getter_selector="guardedValue",
        effective_setter_selector="setGuardedValue:",
    ),
}


def assert_arc_weak_strong_ownership_profiles(
    facts: StorageOwnershipReflectionFacts,
) -> None:
    for payload_key, expected in EXPECTED_OWNERSHIP_PROPERTIES.items():
        _assert_ownership_property(
            facts.payload.get(payload_key, {}),
            expected,
            facts.box_entry,
        )


def _assert_ownership_property(
    prop: dict[str, Any],
    expected: ExpectedOwnershipProperty,
    box_entry: dict[str, Any],
) -> None:
    expect(
        prop.get("found") == 1,
        f"expected {expected.property_name} to be reflectable",
    )
    expect(
        prop.get("has_runtime_getter") == 1 and prop.get("has_runtime_setter") == 1,
        f"expected {expected.property_name} to execute through runtime-backed accessors",
    )
    expect(
        prop.get("base_identity") == box_entry.get("base_identity"),
        f"expected {expected.property_name} to share Box base identity",
    )
    expect(
        prop.get("slot_index") == expected.slot_index,
        f"expected {expected.property_name} to keep slot index {expected.slot_index}",
    )
    expect(
        prop.get("size_bytes") == 8 and prop.get("alignment_bytes") == 8,
        f"expected {expected.property_name} to preserve 8-byte object storage layout",
    )
    expect(
        prop.get("property_name") == expected.property_name,
        f"expected runtime property name for {expected.property_name}",
    )
    expect(
        prop.get("effective_getter_selector") == expected.effective_getter_selector,
        f"expected getter selector for {expected.property_name}",
    )
    expect(
        prop.get("effective_setter_selector") == expected.effective_setter_selector,
        f"expected setter selector for {expected.property_name}",
    )
    expect(
        prop.get("property_attribute_profile") == expected.property_attribute_profile,
        f"expected property attribute profile for {expected.property_name}",
    )
    expect(
        prop.get("ownership_lifetime_profile") == expected.ownership_lifetime_profile,
        f"expected ownership lifetime profile for {expected.property_name}",
    )
    if expected.ownership_runtime_hook_profile is None:
        expect(
            prop.get("ownership_runtime_hook_profile") in (None, ""),
            f"expected no runtime hook profile for {expected.property_name}",
        )
    else:
        expect(
            prop.get("ownership_runtime_hook_profile")
            == expected.ownership_runtime_hook_profile,
            f"expected runtime hook profile for {expected.property_name}",
        )
    expect(
        prop.get("accessor_ownership_profile") == expected.accessor_ownership_profile,
        f"expected accessor ownership profile for {expected.property_name}",
    )
    expect(
        prop.get("getter_owner_identity"),
        f"expected getter owner identity for {expected.property_name}",
    )
    expect(
        prop.get("setter_owner_identity"),
        f"expected setter owner identity for {expected.property_name}",
    )


__all__ = [
    "EXPECTED_OWNERSHIP_PROPERTIES",
    "ExpectedOwnershipProperty",
    "assert_arc_weak_strong_ownership_profiles",
]
