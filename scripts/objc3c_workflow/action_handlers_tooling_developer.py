"""Developer-tooling, ecosystem, and application handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    application_surfaces,
    developer_tooling,
    ecosystem_publication,
    hygiene,
    validation_timing,
)

DEVELOPER_ECOSYSTEM_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-bonus-tool-integration": developer_tooling.action_inspect_bonus_tool_integration,
    "inspect-validation-timing": validation_timing.action_inspect_validation_timing,
    "materialize-project-template": developer_tooling.action_materialize_project_template,
    "materialize-canonical-application-workspace": application_surfaces.action_materialize_canonical_application_workspace,
    "trace-compile-stages": developer_tooling.action_trace_compile_stages,
    "test-capability-routed-source-parity": developer_tooling.action_test_capability_routed_source_parity,
    "validate-developer-tooling": developer_tooling.action_validate_developer_tooling,
    "validate-runnable-developer-tooling": developer_tooling.action_validate_runnable_developer_tooling,
    "validate-bonus-experiences": developer_tooling.action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": developer_tooling.action_validate_runnable_bonus_experiences,
    "validate-application-architecture": application_surfaces.action_validate_application_architecture,
    "validate-runnable-application-architecture": application_surfaces.action_validate_runnable_application_architecture,
    "build-package-lock": ecosystem_publication.action_build_package_lock,
    "validate-package-authoring": ecosystem_publication.action_validate_package_authoring,
    "validate-package-mirror": ecosystem_publication.action_validate_package_mirror,
    "validate-package-ecosystem": ecosystem_publication.action_validate_package_ecosystem,
    "validate-runnable-package-ecosystem": ecosystem_publication.action_validate_runnable_package_ecosystem,
    "validate-long-horizon-operations": ecosystem_publication.action_validate_long_horizon_operations,
    "publish-long-horizon-operations": ecosystem_publication.action_publish_long_horizon_operations,
    "validate-adoption-legibility": ecosystem_publication.action_validate_adoption_legibility,
    "publish-adoption-legibility": ecosystem_publication.action_publish_adoption_legibility,
    "validate-governance-sustainability": ecosystem_publication.action_validate_governance_sustainability,
    "publish-governance-sustainability": ecosystem_publication.action_publish_governance_sustainability,
    "publish-planning-issues": ecosystem_publication.action_publish_planning_issues,
    "check-planning-publication-drift": ecosystem_publication.action_check_planning_publication_drift,
    "lint-spec": hygiene.action_lint_spec,
}
