"""Public test orchestration action specs."""

from __future__ import annotations

from .action_catalog_public_test_composites import (
    PUBLIC_TEST_COMPOSITE_ACTION_SPECS,
)
from .action_catalog_public_test_fixtures import PUBLIC_TEST_FIXTURE_ACTION_SPECS
from .action_catalog_public_test_native import PUBLIC_TEST_NATIVE_ACTION_SPECS
from .action_spec import ActionSpec

PUBLIC_TEST_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-default": PUBLIC_TEST_COMPOSITE_ACTION_SPECS["test-default"],
    "test-behavior-matrix": PUBLIC_TEST_NATIVE_ACTION_SPECS["test-behavior-matrix"],
    "test-smoke": PUBLIC_TEST_COMPOSITE_ACTION_SPECS["test-smoke"],
    "test-ci": PUBLIC_TEST_COMPOSITE_ACTION_SPECS["test-ci"],
    "test-recovery": PUBLIC_TEST_NATIVE_ACTION_SPECS["test-recovery"],
    "test-compile-wrapper-self-audit": PUBLIC_TEST_NATIVE_ACTION_SPECS[
        "test-compile-wrapper-self-audit"
    ],
    "test-llvm-capability-routing": PUBLIC_TEST_NATIVE_ACTION_SPECS[
        "test-llvm-capability-routing"
    ],
    "test-execution-smoke": PUBLIC_TEST_NATIVE_ACTION_SPECS["test-execution-smoke"],
    "test-hosted-execution-smoke": PUBLIC_TEST_NATIVE_ACTION_SPECS[
        "test-hosted-execution-smoke"
    ],
    "test-execution-replay": PUBLIC_TEST_NATIVE_ACTION_SPECS[
        "test-execution-replay"
    ],
    "test-execution-replay-focused": PUBLIC_TEST_NATIVE_ACTION_SPECS[
        "test-execution-replay-focused"
    ],
    "test-fixture-matrix": PUBLIC_TEST_FIXTURE_ACTION_SPECS["test-fixture-matrix"],
    "test-negative-expectations": PUBLIC_TEST_FIXTURE_ACTION_SPECS[
        "test-negative-expectations"
    ],
    "test-full": PUBLIC_TEST_COMPOSITE_ACTION_SPECS["test-full"],
    "test-nightly": PUBLIC_TEST_COMPOSITE_ACTION_SPECS["test-nightly"],
}

__all__ = ["PUBLIC_TEST_ACTION_SPECS"]
