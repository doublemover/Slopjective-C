"""Application workspace and developer-tooling runnable action specs."""

from __future__ import annotations

from .action_catalog_application_architecture import (
    APPLICATION_ARCHITECTURE_ACTION_SPECS,
)
from .action_catalog_application_developer_tooling import (
    APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS,
)
from .action_catalog_application_playground import APPLICATION_PLAYGROUND_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

APPLICATION_WORKSPACE_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    APPLICATION_PLAYGROUND_ACTION_SPECS,
    APPLICATION_ARCHITECTURE_ACTION_SPECS,
    APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS,
)


__all__ = ["APPLICATION_WORKSPACE_ACTION_SPECS"]
