"""Imported-runtime packaging startup method-cache assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_startup_method_cache(payload: dict[str, Any]) -> None:
    expect(
        payload.get("method_cache_state_status") == 0,
        "expected imported-runtime startup method-cache snapshot copy to succeed",
    )
    expect(
        payload.get("method_cache_entry_count") == 3,
        "expected imported-runtime startup to publish three method-cache entries",
    )
    expect(
        payload.get("method_cache_live_dispatch_count") == 3,
        "expected imported-runtime startup to publish three live dispatch entries",
    )
    expect(
        payload.get("method_cache_strict_dispatch_error_count") == 0,
        "expected imported-runtime startup to avoid metadata-backed strict dispatch errors",
    )
    expect(
        payload.get("method_cache_last_selector") == "localClassValue",
        "expected imported-runtime startup to publish the last resolved selector",
    )
    expect(
        payload.get("method_cache_last_resolved_class_name") == "LocalConsumer",
        "expected imported-runtime startup to resolve the last method-cache class name",
    )
    expect(
        payload.get("method_cache_last_resolved_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected imported-runtime startup to resolve the last method-cache owner identity",
    )
    expect(
        payload.get("provider_method_status") == 0
        and payload.get("provider_method_found") == 1
        and payload.get("provider_method_resolved") == 1,
        "expected provider class method metadata to resolve at startup",
    )
    expect(
        payload.get("provider_method_owner_identity")
        == "implementation:ImportedProvider::class_method:providerClassValue",
        "expected provider class method metadata to publish the resolved owner identity at startup",
    )
    expect(
        payload.get("imported_protocol_method_status") == 0
        and payload.get("imported_protocol_method_found") == 1
        and payload.get("imported_protocol_method_resolved") == 1,
        "expected imported protocol method metadata to resolve at startup",
    )
    expect(
        payload.get("imported_protocol_method_owner_identity")
        == "implementation:ImportedProvider::class_method:importedProtocolValue",
        "expected imported protocol method metadata to publish the resolved owner identity at startup",
    )
    expect(
        payload.get("local_method_status") == 0
        and payload.get("local_method_found") == 1
        and payload.get("local_method_resolved") == 1,
        "expected local class method metadata to resolve at startup",
    )
    expect(
        payload.get("local_method_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected local class method metadata to publish the resolved owner identity at startup",
    )


__all__ = ["assert_imported_runtime_startup_method_cache"]
