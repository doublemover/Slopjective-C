"""Imported-runtime replay probe assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    ImportedRuntimeStartupDispatchValues,
)

from objc3c_runtime_acceptance.expectation_matching import expect
from .catalog import POST_REPLAY_BOOTSTRAP_EXPECTATIONS
from .catalog import POST_REPLAY_METADATA_ENTRY_EXPECTATIONS
from .catalog import POST_REPLAY_METHOD_CACHE_EXPECTATIONS
from .catalog import POST_REPLAY_METHOD_ENTRY_EXPECTATIONS
from .catalog import POST_REPLAY_METHOD_OWNER_EXPECTATIONS
from .catalog import POST_REPLAY_MODULE_NAME_EXPECTATIONS
from .catalog import POST_REPLAY_REGISTRATION_EXPECTATIONS
from .catalog import POST_REPLAY_REPLAY_GENERATION_EXPECTATION
from .catalog import POST_REPLAY_SELECTOR_ENTRY_EXPECTATIONS
from .catalog import POST_REPLAY_SELECTOR_TABLE_EXPECTATIONS
from .catalog import POST_RESET_AND_REPLAY_COPY_EXPECTATIONS
from .data import ImportedRuntimeReplayProbeAssertionData
from .data import PayloadEqualityExpectation
from .data import PayloadMinimumExpectation
from .data import PayloadResolvedMethodExpectation
from .data import PayloadStatusFlagExpectation
from .data import StartupDispatchReplayExpectation
from .payloads import imported_runtime_replay_probe_assertion_data
from .predicates import payload_method_is_resolved
from .predicates import payload_status_flag_is_set
from .predicates import payload_value_at_least
from .predicates import payload_value_matches
from .predicates import replay_image_count_matches_link_plan
from .predicates import startup_dispatch_value_survived


def assert_imported_runtime_replay_probe_payload(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
    provider_identity: str,
    consumer_identity: str,
    startup_values: ImportedRuntimeStartupDispatchValues,
) -> None:
    assertion_data = imported_runtime_replay_probe_assertion_data(
        link_plan,
        provider_identity,
        consumer_identity,
        startup_values,
    )

    _expect_equalities(payload, POST_RESET_AND_REPLAY_COPY_EXPECTATIONS)
    _expect_replay_image_count_matches_link_plan(payload, assertion_data)
    _expect_equalities(payload, POST_REPLAY_REGISTRATION_EXPECTATIONS)
    _expect_minimum(payload, POST_REPLAY_REPLAY_GENERATION_EXPECTATION)
    _expect_equalities(payload, POST_REPLAY_BOOTSTRAP_EXPECTATIONS)
    _expect_status_flags(payload, POST_REPLAY_METADATA_ENTRY_EXPECTATIONS)
    _expect_module_identity(payload, assertion_data)
    _expect_startup_dispatch_values_survived(payload, assertion_data)
    _expect_equalities(payload, POST_REPLAY_SELECTOR_TABLE_EXPECTATIONS)
    _expect_status_flags(payload, POST_REPLAY_SELECTOR_ENTRY_EXPECTATIONS)
    _expect_equalities(payload, POST_REPLAY_METHOD_CACHE_EXPECTATIONS)
    _expect_resolved_method(payload, POST_REPLAY_METHOD_ENTRY_EXPECTATIONS[0])
    _expect_equalities(payload, (POST_REPLAY_METHOD_OWNER_EXPECTATIONS[0],))
    _expect_resolved_method(payload, POST_REPLAY_METHOD_ENTRY_EXPECTATIONS[1])
    _expect_equalities(payload, (POST_REPLAY_METHOD_OWNER_EXPECTATIONS[1],))
    _expect_resolved_method(payload, POST_REPLAY_METHOD_ENTRY_EXPECTATIONS[2])
    _expect_equalities(payload, (POST_REPLAY_METHOD_OWNER_EXPECTATIONS[2],))


def _expect_replay_image_count_matches_link_plan(
    payload: dict[str, Any],
    assertion_data: ImportedRuntimeReplayProbeAssertionData,
) -> None:
    expect(
        replay_image_count_matches_link_plan(payload, assertion_data.link_plan),
        "expected replay image count to match the cross-module link plan",
    )


def _expect_module_identity(
    payload: dict[str, Any],
    assertion_data: ImportedRuntimeReplayProbeAssertionData,
) -> None:
    _expect_equalities(payload, (POST_REPLAY_MODULE_NAME_EXPECTATIONS[0],))
    _expect_equalities(payload, (assertion_data.translation_unit_expectations[0],))
    _expect_equalities(payload, (POST_REPLAY_MODULE_NAME_EXPECTATIONS[1],))
    _expect_equalities(payload, (assertion_data.translation_unit_expectations[1],))


def _expect_startup_dispatch_values_survived(
    payload: dict[str, Any],
    assertion_data: ImportedRuntimeReplayProbeAssertionData,
) -> None:
    for expectation in assertion_data.startup_dispatch_expectations:
        _expect_startup_dispatch_value_survived(payload, expectation)


def _expect_equalities(
    payload: dict[str, Any],
    expectations: tuple[PayloadEqualityExpectation, ...],
) -> None:
    for expectation in expectations:
        expect(
            payload_value_matches(
                payload,
                expectation.key,
                expectation.expected,
            ),
            expectation.message,
        )


def _expect_minimum(
    payload: dict[str, Any],
    expectation: PayloadMinimumExpectation,
) -> None:
    expect(
        payload_value_at_least(
            payload,
            expectation.key,
            expectation.minimum,
            default=expectation.default,
        ),
        expectation.message,
    )


def _expect_status_flags(
    payload: dict[str, Any],
    expectations: tuple[PayloadStatusFlagExpectation, ...],
) -> None:
    for expectation in expectations:
        expect(
            payload_status_flag_is_set(
                payload,
                expectation.status_key,
                expectation.flag_key,
            ),
            expectation.message,
        )


def _expect_resolved_method(
    payload: dict[str, Any],
    expectation: PayloadResolvedMethodExpectation,
) -> None:
    expect(
        payload_method_is_resolved(
            payload,
            expectation.status_key,
            expectation.found_key,
            expectation.resolved_key,
        ),
        expectation.message,
    )


def _expect_startup_dispatch_value_survived(
    payload: dict[str, Any],
    expectation: StartupDispatchReplayExpectation,
) -> None:
    expect(
        startup_dispatch_value_survived(
            payload,
            expectation.key,
            expectation.startup_value,
            expectation.expected,
        ),
        expectation.message,
    )


__all__ = ["assert_imported_runtime_replay_probe_payload"]
