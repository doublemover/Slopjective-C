"""Core documentation and public command-surface action specs."""

from __future__ import annotations

from .action_catalog_core_documentation_validation import (
    CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS,
)
from .action_catalog_core_markdown import CORE_MARKDOWN_ACTION_SPECS
from .action_catalog_core_native_docs import CORE_NATIVE_DOCS_ACTION_SPECS
from .action_catalog_core_public_commands import CORE_PUBLIC_COMMAND_ACTION_SPECS
from .action_catalog_core_site_docs import CORE_SITE_DOCS_ACTION_SPECS
from .action_spec import ActionSpec

CORE_DOCS_ACTION_SPECS: dict[str, ActionSpec] = {
    **CORE_SITE_DOCS_ACTION_SPECS,
    **CORE_NATIVE_DOCS_ACTION_SPECS,
    **CORE_PUBLIC_COMMAND_ACTION_SPECS,
    **CORE_MARKDOWN_ACTION_SPECS,
    **CORE_DOCUMENTATION_VALIDATION_ACTION_SPECS,
}

__all__ = ["CORE_DOCS_ACTION_SPECS"]
