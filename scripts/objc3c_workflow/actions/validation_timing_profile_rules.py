"""Validation timing profile rule catalog."""

from __future__ import annotations


VALIDATION_PROFILE_RULES: dict[str, dict[str, object]] = {
    "docs": {
        "path_prefixes": ("docs/", "site/", "README", "CHANGELOG", "package.json"),
        "recommended_actions": (
            "check-documentation-surface",
            "check-markdown",
            "check-public-command-surface",
        ),
        "exhaustive_actions": ("validate-documentation-surface",),
        "skipped_by_default": (
            "runtime acceptance",
            "execution smoke",
            "execution replay",
        ),
    },
    "lowering": {
        "path_prefixes": (
            "native/objc3c/src/ir/",
            "native/objc3c/src/sema/",
            "native/objc3c/src/parser/",
            "tests/native/",
            "tests/tooling/fixtures/native/",
        ),
        "recommended_actions": (
            "test-behavior-matrix",
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-diagnostics",
            "test-execution-replay-focused",
        ),
        "exhaustive_actions": ("test-full", "test-nightly"),
        "skipped_by_default": ("full smoke matrix", "nightly recovery fan-out"),
    },
    "runtime": {
        "path_prefixes": (
            "native/objc3c/src/runtime/",
            "tests/tooling/runtime/",
            "native/objc3c/runtime/",
        ),
        "recommended_actions": (
            "test-runtime-acceptance-fast",
            "test-runtime-acceptance-block-arc",
            "test-runtime-acceptance-concurrency",
            "test-execution-smoke",
            "test-execution-replay-focused",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("release packaging validations",),
    },
    "diagnostics": {
        "path_prefixes": (
            "tests/tooling/fixtures/native/negative/",
            "tests/tooling/fixtures/native/diagnostics/",
            "native/objc3c/src/diagnostics/",
        ),
        "recommended_actions": (
            "test-negative-expectations",
            "test-runtime-acceptance-diagnostics",
        ),
        "exhaustive_actions": ("test-runtime-acceptance", "test-nightly"),
        "skipped_by_default": ("runtime-only smoke cases not touching diagnostics"),
    },
    "conformance": {
        "path_prefixes": (
            "tests/conformance/",
            "docs/objc3c-native/src/",
            "scripts/check_objc3c_runnable_",
        ),
        "recommended_actions": (
            "validate-conformance-corpus",
            "validate-runtime-architecture",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("stress and fuzz validation"),
    },
    "stress": {
        "path_prefixes": (
            "tests/tooling/fixtures/stress/",
            "scripts/run_objc3c_fuzz",
            "scripts/run_objc3c_lowering_runtime_stress",
            "scripts/run_objc3c_mixed_module_differential",
            "scripts/run_objc3c_stress",
        ),
        "recommended_actions": (
            "validate-stress",
            "test-fuzz-safety",
            "test-lowering-runtime-stress",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("docs-only validation"),
    },
    "release-claim": {
        "path_prefixes": (
            "docs/runbooks/",
            "schemas/",
            "release/",
            "scripts/publish_",
            "scripts/build_objc3c_release",
        ),
        "recommended_actions": (
            "check-release-evidence",
            "validate-release-foundation",
            "validate-public-conformance-reporting",
        ),
        "exhaustive_actions": ("test-nightly",),
        "skipped_by_default": ("local-only playground inspections"),
    },
}
