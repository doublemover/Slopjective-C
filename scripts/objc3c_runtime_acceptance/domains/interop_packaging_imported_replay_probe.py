"""Imported-runtime packaging replay probe assertion coordinator."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_runtime_probe import (
    assert_imported_runtime_replay_probe_payload,
)
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    assert_imported_runtime_startup_probe_payload,
)


def assert_imported_runtime_probe_payload(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
    provider_identity: str,
    consumer_identity: str,
) -> None:
    startup_values = assert_imported_runtime_startup_probe_payload(
        payload,
        link_plan,
    )
    assert_imported_runtime_replay_probe_payload(
        payload,
        link_plan,
        provider_identity,
        consumer_identity,
        startup_values,
    )


__all__ = ["assert_imported_runtime_probe_payload"]
