"""Documentation validation timing profile rules."""

from __future__ import annotations

from .validation_timing_profile_model import ValidationProfileRule, profile_rule

DOCS_PROFILE_RULES: dict[str, ValidationProfileRule] = {
    "docs": profile_rule(
        path_prefixes=("docs/", "site/", "README", "CHANGELOG", "package.json"),
        recommended_actions=(
            "check-documentation-surface",
            "check-markdown",
            "check-public-command-surface",
        ),
        exhaustive_actions=("validate-documentation-surface",),
        deferred_actions=(
            "runtime acceptance",
            "execution smoke",
            "execution replay",
        ),
        profile_owner="validation_timing_profile_catalog_docs",
        source_owner="validation_timing_changed_paths",
        hard_blocking_decision_owner="validation_timing_budgets",
    ),
}
