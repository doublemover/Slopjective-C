"""Imported-runtime packaging startup probe assertions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_dispatch import (
    assert_imported_runtime_startup_dispatch_values,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_metadata import (
    assert_imported_runtime_startup_metadata_entries,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_method_cache import (
    assert_imported_runtime_startup_method_cache,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_protocol import (
    assert_imported_runtime_startup_protocol_query,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_registration import (
    assert_imported_runtime_startup_registration_state,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_selectors import (
    assert_imported_runtime_startup_selectors,
)


@dataclass(frozen=True)
class ImportedRuntimeStartupDispatchValues:
    imported_provider_class_value: int
    imported_provider_protocol_value: int
    local_consumer_class_value: int


def assert_imported_runtime_startup_probe_payload(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
) -> ImportedRuntimeStartupDispatchValues:
    assert_imported_runtime_startup_registration_state(payload, link_plan)
    assert_imported_runtime_startup_metadata_entries(payload)
    (
        imported_provider_class_value,
        imported_provider_protocol_value,
        local_consumer_class_value,
    ) = assert_imported_runtime_startup_dispatch_values(payload)
    assert_imported_runtime_startup_selectors(payload)
    assert_imported_runtime_startup_method_cache(payload)
    assert_imported_runtime_startup_protocol_query(payload)

    return ImportedRuntimeStartupDispatchValues(
        imported_provider_class_value=imported_provider_class_value,
        imported_provider_protocol_value=imported_provider_protocol_value,
        local_consumer_class_value=local_consumer_class_value,
    )


__all__ = [
    "ImportedRuntimeStartupDispatchValues",
    "assert_imported_runtime_startup_probe_payload",
]
