"""Core build, docs, hygiene, and command-surface action specs."""

from __future__ import annotations

from .action_catalog_core_build import CORE_BUILD_ACTION_SPECS
from .action_catalog_core_codegen import CORE_CODEGEN_ACTION_SPECS
from .action_catalog_core_docs import CORE_DOCS_ACTION_SPECS
from .action_catalog_core_hygiene import CORE_HYGIENE_ACTION_SPECS
from .action_catalog_core_showcase import CORE_SHOWCASE_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

CORE_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    CORE_BUILD_ACTION_SPECS,
    CORE_CODEGEN_ACTION_SPECS,
    CORE_DOCS_ACTION_SPECS,
    CORE_HYGIENE_ACTION_SPECS,
    CORE_SHOWCASE_ACTION_SPECS,
)
