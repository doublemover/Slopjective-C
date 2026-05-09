"""Core documentation validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-documentation-surface": ActionSpec("validate-documentation-surface", "run the full documentation build and reader-surface validation flow", "runner-internal + generated documentation checks", validation_tier="docs", guarantee_owner="site output, native docs, command appendix, and reader-facing onboarding remain buildable, in sync, and explicit"),
}

__all__ = ["CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS"]
