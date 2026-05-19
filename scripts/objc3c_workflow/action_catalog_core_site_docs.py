"""Core site action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_SITE_DOCS_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-site": ActionSpec("build-site", "build published site output and format it", "python:scripts/build_site_index.py + npx prettier"),
    "check-site": ActionSpec("check-site", "check generated site output for drift", "python:scripts/build_site_index.py --check", validation_tier="docs", guarantee_owner="published site index generation stays in sync with site/src inputs"),
}

__all__ = ["CORE_SITE_DOCS_ACTION_SPECS"]
