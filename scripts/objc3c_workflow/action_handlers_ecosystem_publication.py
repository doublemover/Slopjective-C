"""Package ecosystem and governance publication handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import ecosystem_publication

ECOSYSTEM_PUBLICATION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-package-lock": ecosystem_publication.action_build_package_lock,
    "validate-package-manager-model": ecosystem_publication.action_validate_package_manager_model,
    "validate-package-authoring": ecosystem_publication.action_validate_package_authoring,
    "validate-package-mirror": ecosystem_publication.action_validate_package_mirror,
    "validate-package-ecosystem": ecosystem_publication.action_validate_package_ecosystem,
    "validate-package-install-distribution": ecosystem_publication.action_validate_package_install_distribution,
    "validate-runnable-package-ecosystem": ecosystem_publication.action_validate_runnable_package_ecosystem,
    "validate-long-horizon-operations": ecosystem_publication.action_validate_long_horizon_operations,
    "publish-long-horizon-operations": ecosystem_publication.action_publish_long_horizon_operations,
    "validate-adoption-legibility": ecosystem_publication.action_validate_adoption_legibility,
    "publish-adoption-legibility": ecosystem_publication.action_publish_adoption_legibility,
    "validate-governance-sustainability": ecosystem_publication.action_validate_governance_sustainability,
    "publish-governance-sustainability": ecosystem_publication.action_publish_governance_sustainability,
    "publish-planning-issues": ecosystem_publication.action_publish_planning_issues,
    "check-planning-publication-drift": ecosystem_publication.action_check_planning_publication_drift,
}

__all__ = ["ECOSYSTEM_PUBLICATION_ACTION_HANDLERS"]
