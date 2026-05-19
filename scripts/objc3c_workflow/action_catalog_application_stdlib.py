"""Stdlib workspace and validation action specs."""

from __future__ import annotations

from .action_catalog_application_stdlib_integrations import (
    APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
)
from .action_catalog_application_stdlib_runnable import (
    APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
)
from .action_catalog_application_stdlib_workspace import (
    APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
)
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

APPLICATION_STDLIB_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
    APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
    APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
)


__all__ = ["APPLICATION_STDLIB_ACTION_SPECS"]
