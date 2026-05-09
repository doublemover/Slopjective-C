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
        skipped_by_default=(
            "runtime acceptance",
            "execution smoke",
            "execution replay",
        ),
    ),
}
