"""Imported-runtime packaging startup dispatch assertions."""

from __future__ import annotations

from typing import Any, cast

from ..expectation_matching import expect


def assert_imported_runtime_startup_dispatch_values(
    payload: dict[str, Any],
) -> tuple[int, int, int]:
    imported_provider_class_value = payload.get("imported_provider_class_value")
    imported_provider_protocol_value = payload.get("imported_provider_protocol_value")
    local_consumer_class_value = payload.get("local_consumer_class_value")
    expect(
        isinstance(imported_provider_class_value, int)
        and imported_provider_class_value == 43,
        "expected imported provider class dispatch to execute the provider class method",
    )
    expect(
        isinstance(imported_provider_protocol_value, int)
        and imported_provider_protocol_value == 41,
        "expected imported provider protocol method dispatch to execute the provider implementation",
    )
    expect(
        isinstance(local_consumer_class_value, int)
        and local_consumer_class_value == 53,
        "expected local consumer class dispatch to execute the local class method",
    )
    return (
        cast(int, imported_provider_class_value),
        cast(int, imported_provider_protocol_value),
        cast(int, local_consumer_class_value),
    )


__all__ = ["assert_imported_runtime_startup_dispatch_values"]
