"""Broad fixture sweep public test action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PUBLIC_TEST_FIXTURE_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-fixture-matrix": ActionSpec(
        "test-fixture-matrix",
        "broad positive dispatch fixture matrix sweep",
        "pwsh:scripts/run_objc3c_native_fixture_matrix.ps1",
        validation_tier="nightly",
        guarantee_owner="broad positive dispatch and artifact sanity",
        pass_through_args=True,
    ),
    "test-negative-expectations": ActionSpec(
        "test-negative-expectations",
        "static negative fixture expectation enforcement",
        "pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1",
        validation_tier="nightly",
        guarantee_owner="negative expectation header and token enforcement",
        pass_through_args=True,
    ),
}
