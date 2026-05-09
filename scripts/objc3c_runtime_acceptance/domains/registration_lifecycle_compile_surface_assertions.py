"""Compile-surface assertions for registration lifecycle acceptance."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


REGISTRATION_DESCRIPTOR_COUNT_FIELDS = (
    "class_descriptor_count",
    "protocol_descriptor_count",
    "category_descriptor_count",
    "property_descriptor_count",
    "ivar_descriptor_count",
)


def assert_registration_descriptor_contract(
    registration_descriptor: dict[str, Any],
) -> None:
    descriptor_identifier = registration_descriptor.get(
        "registration_descriptor_identifier"
    )
    expect(
        isinstance(descriptor_identifier, str)
        and descriptor_identifier.endswith("_registration_descriptor")
        and len(descriptor_identifier) > len("_registration_descriptor"),
        "expected lifecycle fixture to publish a registration descriptor identifier",
    )

    translation_unit_identity_key = registration_descriptor.get(
        "translation_unit_identity_key"
    )
    expect(
        isinstance(translation_unit_identity_key, str)
        and translation_unit_identity_key != "",
        "expected lifecycle fixture to publish a translation-unit identity key",
    )

    registration_ordinal = registration_descriptor.get(
        "translation_unit_registration_order_ordinal"
    )
    expect(
        type(registration_ordinal) is int and registration_ordinal == 1,
        "expected lifecycle fixture registration descriptor to start at ordinal one",
    )

    for count_field in REGISTRATION_DESCRIPTOR_COUNT_FIELDS:
        count_value = registration_descriptor.get(count_field)
        expect(
            type(count_value) is int and count_value >= 0,
            f"expected lifecycle fixture registration descriptor to publish {count_field}",
        )

    expect(
        registration_descriptor.get("class_descriptor_count", 0) > 0,
        "expected lifecycle fixture registration descriptor to publish at least one class descriptor",
    )


__all__ = [
    "REGISTRATION_DESCRIPTOR_COUNT_FIELDS",
    "assert_registration_descriptor_contract",
]
