"""Core hygiene and capability action specs."""

from __future__ import annotations

from .action_catalog_core_capabilities import CORE_CAPABILITY_ACTION_SPECS
from .action_catalog_core_linting import CORE_LINTING_ACTION_SPECS
from .action_catalog_core_source_hygiene import CORE_SOURCE_HYGIENE_ACTION_SPECS
from .action_catalog_core_superclean import CORE_SUPERCLEAN_ACTION_SPECS
from .action_spec import ActionSpec

CORE_HYGIENE_ACTION_SPECS: dict[str, ActionSpec] = {
    **CORE_LINTING_ACTION_SPECS,
    **CORE_CAPABILITY_ACTION_SPECS,
    "check-release-evidence": CORE_SUPERCLEAN_ACTION_SPECS["check-release-evidence"],
    **CORE_SOURCE_HYGIENE_ACTION_SPECS,
    "check-repo-superclean-surface": CORE_SUPERCLEAN_ACTION_SPECS[
        "check-repo-superclean-surface"
    ],
    "validate-repo-superclean": CORE_SUPERCLEAN_ACTION_SPECS[
        "validate-repo-superclean"
    ],
}

__all__ = ["CORE_HYGIENE_ACTION_SPECS"]
