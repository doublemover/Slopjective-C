"""Conformance, stress, and release validation timing profile rules."""

from __future__ import annotations

from .validation_timing_profile_model import ValidationProfileRule, profile_rule

RELEASE_PROFILE_RULES: dict[str, ValidationProfileRule] = {
    "conformance": profile_rule(
        path_prefixes=(
            "tests/conformance/",
            "docs/objc3c-native/src/",
            "scripts/check_objc3c_runnable_",
        ),
        recommended_actions=(
            "validate-conformance-corpus",
            "validate-runtime-architecture",
        ),
        exhaustive_actions=("test-nightly",),
        skipped_by_default=("stress and fuzz validation",),
    ),
    "stress": profile_rule(
        path_prefixes=(
            "tests/tooling/fixtures/stress/",
            "scripts/run_objc3c_fuzz",
            "scripts/run_objc3c_lowering_runtime_stress",
            "scripts/run_objc3c_mixed_module_differential",
            "scripts/run_objc3c_stress",
        ),
        recommended_actions=(
            "validate-stress",
            "test-fuzz-safety",
            "test-lowering-runtime-stress",
        ),
        exhaustive_actions=("test-nightly",),
        skipped_by_default=("docs-only validation"),
    ),
    "release-claim": profile_rule(
        path_prefixes=(
            "docs/runbooks/",
            "schemas/",
            "release/",
            "scripts/publish_",
            "scripts/build_objc3c_release",
        ),
        recommended_actions=(
            "check-release-evidence",
            "validate-release-foundation",
            "validate-public-conformance-reporting",
        ),
        exhaustive_actions=("test-nightly",),
        skipped_by_default=("local-only playground inspections"),
    ),
}
