"""Imported-runtime replay probe payload shaping."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    ImportedRuntimeStartupDispatchValues,
)

from .catalog import IMPORTED_PROTOCOL_DISPATCH_VALUE
from .catalog import LOCAL_CLASS_DISPATCH_VALUE
from .catalog import PROVIDER_CLASS_DISPATCH_VALUE
from .data import ImportedRuntimeReplayProbeAssertionData
from .data import PayloadEqualityExpectation
from .data import StartupDispatchReplayExpectation


def imported_runtime_replay_probe_assertion_data(
    link_plan: dict[str, Any],
    provider_identity: str,
    consumer_identity: str,
    startup_values: ImportedRuntimeStartupDispatchValues,
) -> ImportedRuntimeReplayProbeAssertionData:
    return ImportedRuntimeReplayProbeAssertionData(
        link_plan=link_plan,
        provider_identity=provider_identity,
        consumer_identity=consumer_identity,
        startup_values=startup_values,
        translation_unit_expectations=_translation_unit_expectations(
            provider_identity,
            consumer_identity,
        ),
        startup_dispatch_expectations=_startup_dispatch_expectations(
            startup_values
        ),
    )


def _translation_unit_expectations(
    provider_identity: str,
    consumer_identity: str,
) -> tuple[PayloadEqualityExpectation, ...]:
    return (
        PayloadEqualityExpectation(
            "post_replay_imported_translation_unit_identity_key",
            provider_identity,
            "expected replay to preserve the provider translation unit identity key",
        ),
        PayloadEqualityExpectation(
            "post_replay_local_translation_unit_identity_key",
            consumer_identity,
            "expected replay to preserve the consumer translation unit identity key",
        ),
    )


def _startup_dispatch_expectations(
    startup_values: ImportedRuntimeStartupDispatchValues,
) -> tuple[StartupDispatchReplayExpectation, ...]:
    return (
        StartupDispatchReplayExpectation(
            "post_replay_imported_provider_class_value",
            startup_values.imported_provider_class_value,
            PROVIDER_CLASS_DISPATCH_VALUE,
            "expected imported provider class dispatch value to survive replay",
        ),
        StartupDispatchReplayExpectation(
            "post_replay_imported_provider_protocol_value",
            startup_values.imported_provider_protocol_value,
            IMPORTED_PROTOCOL_DISPATCH_VALUE,
            "expected imported provider protocol dispatch value to survive replay",
        ),
        StartupDispatchReplayExpectation(
            "post_replay_local_consumer_class_value",
            startup_values.local_consumer_class_value,
            LOCAL_CLASS_DISPATCH_VALUE,
            "expected local consumer class dispatch value to survive replay",
        ),
    )


__all__ = ["imported_runtime_replay_probe_assertion_data"]
