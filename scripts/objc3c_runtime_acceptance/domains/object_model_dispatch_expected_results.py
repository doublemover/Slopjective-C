"""Strict expected-result orchestration for canonical object dispatch payloads."""

from __future__ import annotations

from typing import Any

from .object_model_dispatch_payload_model import CanonicalDispatchPayload
from .object_model_dispatch_payload_model import capture_canonical_dispatch_payload
from .object_model_dispatch_selector_expectations import (
    assert_dispatch_selector_cache_identity,
)
from .object_model_dispatch_selector_expectations import (
    assert_dispatch_selector_table_entries,
)
from .object_model_dispatch_state_expectations import (
    assert_dispatch_cache_entry_ownership,
)
from .object_model_dispatch_state_expectations import assert_dispatch_state_transitions
from .object_model_dispatch_value_expectations import assert_dispatch_class_graph
from .object_model_dispatch_value_expectations import assert_dispatch_return_values


def assert_canonical_dispatch_expected_results(payload: dict[str, Any]) -> None:
    facts = capture_canonical_dispatch_payload(payload)
    assert_dispatch_return_values(facts)
    assert_dispatch_class_graph(facts)
    assert_dispatch_selector_cache_identity(facts)
    assert_dispatch_selector_table_entries(facts)
    assert_dispatch_state_transitions(facts)
    assert_dispatch_cache_entry_ownership(facts)


__all__ = [
    "CanonicalDispatchPayload",
    "assert_canonical_dispatch_expected_results",
    "capture_canonical_dispatch_payload",
]
