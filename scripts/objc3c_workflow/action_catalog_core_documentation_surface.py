"""Core documentation-surface action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_DOCUMENTATION_SURFACE_ACTION_SPECS: dict[str, ActionSpec] = {
    "check-documentation-surface": ActionSpec("check-documentation-surface", "check the reader-facing documentation structure and machine-appendix boundary", "python:scripts/check_documentation_surface.py", validation_tier="docs", guarantee_owner="reader-facing onboarding, site structure, and machine-appendix boundary stay accessible and explicit"),
}

__all__ = ["CORE_DOCUMENTATION_SURFACE_ACTION_SPECS"]
