"""Native compiler/runtime validation timing profile rules."""

from __future__ import annotations

from .validation_timing_profile_model import ValidationProfileRule, profile_rule

NATIVE_PROFILE_RULES: dict[str, ValidationProfileRule] = {
    "lowering": profile_rule(
        path_prefixes=(
            "native/objc3c/src/ir/",
            "native/objc3c/src/sema/",
            "native/objc3c/src/parser/",
            "tests/native/",
            "tests/tooling/fixtures/native/",
        ),
        recommended_actions=(
            "test-behavior-matrix",
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-diagnostics",
            "test-execution-replay-focused",
        ),
        exhaustive_actions=("test-full", "test-nightly"),
        deferred_actions=("full smoke matrix", "nightly recovery fan-out"),
        profile_owner="validation_timing_profile_catalog_native",
        source_owner="validation_timing_changed_paths",
        hard_blocking_decision_owner="validation_timing_budgets",
    ),
    "runtime": profile_rule(
        path_prefixes=(
            "native/objc3c/src/runtime/",
            "tests/tooling/runtime/",
            "native/objc3c/runtime/",
        ),
        recommended_actions=(
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-block-arc",
            "test-runtime-acceptance-concurrency",
            "test-execution-smoke",
            "test-execution-replay-focused",
        ),
        exhaustive_actions=("test-runtime-acceptance", "test-nightly"),
        deferred_actions=("release packaging validations",),
        profile_owner="validation_timing_profile_catalog_native",
        source_owner="validation_timing_changed_paths",
        hard_blocking_decision_owner="validation_timing_budgets",
    ),
    "diagnostics": profile_rule(
        path_prefixes=(
            "tests/tooling/fixtures/native/negative/",
            "tests/tooling/fixtures/native/diagnostics/",
            "native/objc3c/src/diagnostics/",
        ),
        recommended_actions=(
            "test-negative-expectations",
            "test-runtime-acceptance-diagnostics",
        ),
        exhaustive_actions=("test-runtime-acceptance", "test-nightly"),
        deferred_actions=("runtime-only smoke cases not touching diagnostics"),
        profile_owner="validation_timing_profile_catalog_native",
        source_owner="validation_timing_changed_paths",
        hard_blocking_decision_owner="validation_timing_budgets",
    ),
}
