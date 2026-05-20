"""Top-level workflow action catalog section groups."""

from __future__ import annotations

from collections.abc import Mapping
from dataclasses import dataclass

from .action_catalog_application import APPLICATION_AND_ECOSYSTEM_ACTION_SPECS
from .action_catalog_core import CORE_ACTION_SPECS
from .action_catalog_developer_performance import DEVELOPER_AND_PERFORMANCE_ACTION_SPECS
from .action_catalog_reporting_release import STRESS_REPORTING_AND_RELEASE_ACTION_SPECS
from .action_catalog_tooling_test import TOOLING_AND_TEST_ACTION_SPECS
from .action_spec import ActionSpec

ACTION_CATALOG_GROUP_CONTRACT_ID = "objc3c-workflow-action-catalog-groups-v1"
ACTION_CATALOG_GROUP_OWNER_SURFACE = "scripts/objc3c_workflow/action_catalog_groups.py"


@dataclass(frozen=True)
class ActionCatalogSectionGroup:
    group_id: str
    owner_surface: str
    definitions: Mapping[str, ActionSpec]
    category_roots: tuple[str, ...]
    audience_scope: str
    public_contract: bool = True
    hidden_internal_public_split_allowed: bool = False
    wrapper_only_catalog_grouping_allowed: bool = False

    def owner_payload(self) -> dict[str, object]:
        return {
            "group_id": self.group_id,
            "owner_surface": self.owner_surface,
            "action_count": len(self.definitions),
            "category_roots": list(self.category_roots),
            "audience_scope": self.audience_scope,
            "public_contract": self.public_contract,
            "hidden_internal_public_split_allowed": (
                self.hidden_internal_public_split_allowed
            ),
            "wrapper_only_catalog_grouping_allowed": (
                self.wrapper_only_catalog_grouping_allowed
            ),
        }


ACTION_CATALOG_SECTION_GROUPS: tuple[ActionCatalogSectionGroup, ...] = (
    ActionCatalogSectionGroup(
        group_id="core",
        owner_surface="scripts/objc3c_workflow/action_catalog_core.py",
        definitions=CORE_ACTION_SPECS,
        category_roots=("build", "check", "compile", "format", "lint", "validate"),
        audience_scope="maintainer-and-operator",
    ),
    ActionCatalogSectionGroup(
        group_id="application-and-ecosystem",
        owner_surface="scripts/objc3c_workflow/action_catalog_application.py",
        definitions=APPLICATION_AND_ECOSYSTEM_ACTION_SPECS,
        category_roots=("build", "check", "materialize", "publish", "validate"),
        audience_scope="maintainer-and-operator",
    ),
    ActionCatalogSectionGroup(
        group_id="developer-and-performance",
        owner_surface="scripts/objc3c_workflow/action_catalog_developer_performance.py",
        definitions=DEVELOPER_AND_PERFORMANCE_ACTION_SPECS,
        category_roots=(
            "analyze",
            "benchmark",
            "build",
            "check",
            "format",
            "inspect",
            "publish",
            "rewrite",
            "validate",
        ),
        audience_scope="maintainer-and-operator",
    ),
    ActionCatalogSectionGroup(
        group_id="stress-reporting-and-release",
        owner_surface="scripts/objc3c_workflow/action_catalog_reporting_release.py",
        definitions=STRESS_REPORTING_AND_RELEASE_ACTION_SPECS,
        category_roots=("build", "check", "publish", "test", "validate"),
        audience_scope="maintainer-and-operator",
    ),
    ActionCatalogSectionGroup(
        group_id="tooling-and-test",
        owner_surface="scripts/objc3c_workflow/action_catalog_tooling_test.py",
        definitions=TOOLING_AND_TEST_ACTION_SPECS,
        category_roots=(
            "inspect",
            "lint",
            "materialize",
            "package",
            "proof",
            "test",
            "trace",
            "validate",
        ),
        audience_scope="operator",
    ),
)


def action_catalog_section_group_mappings() -> tuple[Mapping[str, ActionSpec], ...]:
    return tuple(group.definitions for group in ACTION_CATALOG_SECTION_GROUPS)


def action_catalog_section_owner_contracts() -> dict[str, object]:
    return {
        "contract_id": ACTION_CATALOG_GROUP_CONTRACT_ID,
        "owner_surface": ACTION_CATALOG_GROUP_OWNER_SURFACE,
        "groups": [group.owner_payload() for group in ACTION_CATALOG_SECTION_GROUPS],
        "hidden_internal_public_split_allowed": False,
        "wrapper_only_catalog_grouping_allowed": False,
        "public_contract": True,
    }


__all__ = [
    "ACTION_CATALOG_GROUP_CONTRACT_ID",
    "ACTION_CATALOG_GROUP_OWNER_SURFACE",
    "ACTION_CATALOG_SECTION_GROUPS",
    "ActionCatalogSectionGroup",
    "action_catalog_section_group_mappings",
    "action_catalog_section_owner_contracts",
]
