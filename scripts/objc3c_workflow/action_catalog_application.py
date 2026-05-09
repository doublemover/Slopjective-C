"""Application, stdlib, package ecosystem, and planning action specs."""

from __future__ import annotations

from .action_catalog_adoption_governance import ADOPTION_GOVERNANCE_ACTION_SPECS
from .action_catalog_application_stdlib import APPLICATION_STDLIB_ACTION_SPECS
from .action_catalog_application_workspaces import APPLICATION_WORKSPACE_ACTION_SPECS
from .action_catalog_package_ecosystem import PACKAGE_ECOSYSTEM_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_spec import ActionSpec

APPLICATION_AND_ECOSYSTEM_ACTION_SPECS: dict[str, ActionSpec] = (
    merge_action_catalog_sections(
        APPLICATION_WORKSPACE_ACTION_SPECS,
        APPLICATION_STDLIB_ACTION_SPECS,
        PACKAGE_ECOSYSTEM_ACTION_SPECS,
        ADOPTION_GOVERNANCE_ACTION_SPECS,
    )
)
