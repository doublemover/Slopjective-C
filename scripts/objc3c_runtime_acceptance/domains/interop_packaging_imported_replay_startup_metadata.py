"""Imported-runtime packaging startup runtime metadata assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_startup_metadata_entries(
    payload: dict[str, Any],
) -> None:
    expect(
        payload.get("imported_entry_status") == 0
        and payload.get("imported_entry_found") == 1,
        "expected imported provider runtime metadata to be realized at startup",
    )
    expect(
        payload.get("local_entry_status") == 0
        and payload.get("local_entry_found") == 1,
        "expected local consumer runtime metadata to be realized at startup",
    )
    expect(
        payload.get("imported_registration_order_ordinal") == 1,
        "expected imported provider runtime metadata to preserve registration ordinal one",
    )
    expect(
        payload.get("local_registration_order_ordinal") == 2,
        "expected local consumer runtime metadata to preserve registration ordinal two",
    )
    expect(
        payload.get("imported_direct_protocol_count") == 1,
        "expected imported provider class to publish one direct protocol",
    )
    expect(
        payload.get("imported_attached_protocol_count") == 0,
        "expected imported provider class to publish no attached protocols",
    )
    expect(
        payload.get("imported_runtime_property_accessor_count") == 0,
        "expected imported provider class to publish no runtime property accessors",
    )
    expect(
        payload.get("imported_module_name") == "runtimePackagingProvider",
        "expected imported provider class entry to preserve the provider module name",
    )
    expect(
        isinstance(payload.get("imported_translation_unit_identity_key"), str)
        and payload.get("imported_translation_unit_identity_key") != "",
        "expected imported provider class entry to publish a non-empty translation unit identity key",
    )
    expect(
        isinstance(payload.get("imported_class_owner_identity"), str)
        and payload.get("imported_class_owner_identity") != "",
        "expected imported provider class owner identity to be non-empty",
    )
    expect(
        payload.get("local_direct_protocol_count") == 0,
        "expected local consumer class to publish no direct protocols",
    )
    expect(
        payload.get("local_attached_protocol_count") == 0,
        "expected local consumer class to publish no attached protocols",
    )
    expect(
        payload.get("local_runtime_property_accessor_count") == 0,
        "expected local consumer class to publish no runtime property accessors",
    )
    expect(
        payload.get("local_module_name") == "runtimePackagingConsumer",
        "expected local consumer class entry to preserve the consumer module name",
    )
    expect(
        isinstance(payload.get("local_translation_unit_identity_key"), str)
        and payload.get("local_translation_unit_identity_key") != "",
        "expected local consumer class entry to publish a non-empty translation unit identity key",
    )
    expect(
        isinstance(payload.get("local_class_owner_identity"), str)
        and payload.get("local_class_owner_identity") != "",
        "expected local consumer class owner identity to be non-empty",
    )
    expect(
        payload.get("protocol_query_attached_category_count") == 0,
        "expected imported-runtime protocol conformance query to publish no attached categories",
    )


__all__ = ["assert_imported_runtime_startup_metadata_entries"]
