"""Public conformance reporting and performance-governance handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import performance, release_governance, schema_surfaces

REPORTING_PUBLIC_PERFORMANCE_HANDLERS: dict[str, ActionHandler] = {
    "check-public-conformance-reporting-surface": release_governance.action_check_public_conformance_reporting_surface,
    "check-public-conformance-schema-surface": schema_surfaces.action_check_public_conformance_schema_surface,
    "build-public-conformance-scorecard": release_governance.action_build_public_conformance_scorecard,
    "publish-public-conformance-report": release_governance.action_publish_public_conformance_report,
    "validate-public-conformance-reporting": release_governance.action_validate_public_conformance_reporting,
    "validate-public-conformance-reporting-integration": release_governance.action_validate_public_conformance_reporting_integration,
    "validate-public-conformance-reporting-end-to-end": release_governance.action_validate_public_conformance_reporting_end_to_end,
    "check-performance-governance-surface": performance.action_check_performance_governance_surface,
    "check-performance-governance-schema-surface": performance.action_check_performance_governance_schema_surface,
    "build-performance-dashboard": performance.action_build_performance_dashboard,
    "publish-performance-report": performance.action_publish_performance_report,
    "validate-performance-governance": performance.action_validate_performance_governance,
    "validate-performance-governance-integration": performance.action_validate_performance_governance_integration,
    "validate-performance-governance-runtime-contract-linkage": performance.action_validate_performance_governance_runtime_contract_linkage,
    "validate-performance-governance-end-to-end": performance.action_validate_performance_governance_end_to_end,
}
