"""Imported-runtime packaging startup selector assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_startup_selectors(payload: dict[str, Any]) -> None:
    expect(
        payload.get("selector_table_status") == 0,
        "expected imported-runtime startup selector-table snapshot copy to succeed",
    )
    expect(
        payload.get("selector_table_entry_count") == 3,
        "expected imported-runtime startup to publish three selector entries",
    )
    expect(
        payload.get("selector_metadata_backed_selector_count") == 3,
        "expected imported-runtime startup to publish three metadata-backed selectors",
    )
    expect(
        payload.get("selector_dynamic_selector_count") == 0,
        "expected imported-runtime startup to avoid dynamic selector entries",
    )
    expect(
        payload.get("provider_selector_status") == 0
        and payload.get("provider_selector_found") == 1,
        "expected provider class selector metadata to be installed at startup",
    )
    expect(
        payload.get("provider_selector_metadata_backed") == 1,
        "expected provider class selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("provider_selector_provider_count") == 1,
        "expected provider class selector metadata to name one provider",
    )
    expect(
        payload.get("provider_selector_first_ordinal") == 1,
        "expected provider class selector metadata to retain the provider registration ordinal",
    )
    expect(
        payload.get("provider_selector_last_ordinal") == 1,
        "expected provider class selector metadata to end at the provider registration ordinal",
    )
    expect(
        payload.get("imported_protocol_selector_status") == 0
        and payload.get("imported_protocol_selector_found") == 1,
        "expected imported protocol selector metadata to be installed at startup",
    )
    expect(
        payload.get("imported_protocol_selector_metadata_backed") == 1,
        "expected imported protocol selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("imported_protocol_selector_provider_count") == 1,
        "expected imported protocol selector metadata to name one provider",
    )
    expect(
        payload.get("imported_protocol_selector_first_ordinal") == 1,
        "expected imported protocol selector metadata to retain the provider registration ordinal",
    )
    expect(
        payload.get("imported_protocol_selector_last_ordinal") == 1,
        "expected imported protocol selector metadata to end at the provider registration ordinal",
    )
    expect(
        payload.get("local_selector_status") == 0
        and payload.get("local_selector_found") == 1,
        "expected local class selector metadata to be installed at startup",
    )
    expect(
        payload.get("local_selector_metadata_backed") == 1,
        "expected local class selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("local_selector_provider_count") == 1,
        "expected local class selector metadata to name one provider",
    )
    expect(
        payload.get("local_selector_first_ordinal") == 2,
        "expected local class selector metadata to retain the local registration ordinal",
    )
    expect(
        payload.get("local_selector_last_ordinal") == 2,
        "expected local class selector metadata to end at the local registration ordinal",
    )


__all__ = ["assert_imported_runtime_startup_selectors"]
