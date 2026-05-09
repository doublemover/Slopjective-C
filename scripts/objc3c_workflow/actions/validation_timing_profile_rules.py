"""Validation timing profile rule catalog."""

from __future__ import annotations

from .validation_timing_profile_catalog_docs import DOCS_PROFILE_RULES
from .validation_timing_profile_catalog_native import NATIVE_PROFILE_RULES
from .validation_timing_profile_catalog_release import RELEASE_PROFILE_RULES
from .validation_timing_profile_model import ValidationProfileRule

VALIDATION_PROFILE_RULES: dict[str, ValidationProfileRule] = {
    **DOCS_PROFILE_RULES,
    **NATIVE_PROFILE_RULES,
    **RELEASE_PROFILE_RULES,
}

__all__ = ["VALIDATION_PROFILE_RULES", "ValidationProfileRule"]
