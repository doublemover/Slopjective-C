from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_distribution_credibility_dashboard.input_loading import DistributionCredibilityDashboardInputs
from objc3c_distribution_credibility_dashboard.paths import DistributionCredibilityDashboardPaths
from objc3c_distribution_credibility_dashboard.probes import (
    build_trust_signals,
    trust_state_for_signals,
)
from objc3c_distribution_credibility_dashboard.validation import validate_dashboard_inputs


@dataclass(frozen=True)
class DistributionCredibilityDashboardModel:
    status: str
    trust_state: str
    release_id: str
    release_version: str
    warning_count: int
    upstream_reports: dict[str, str]
    trust_signals: list[dict[str, Any]]
    required_drill_steps: Any
    install_docs: Any
    release_docs: Any
    operator_states: list[str]
    operator_actions: list[str]
    release_drill_policy: dict[str, Any]
    workflow_surface: dict[str, Any]
    dashboard_schema: Any
    trust_report_schema: Any
    artifact_surface: dict[str, Any]
    failures: list[str]


def build_upstream_reports(paths: DistributionCredibilityDashboardPaths) -> dict[str, str]:
    return {
        "release_manifest": repo_rel(paths.release_foundation_manifest),
        "package_channels_end_to_end": repo_rel(paths.package_channels_end_to_end),
        "package_install_distribution": repo_rel(paths.package_install_distribution_summary),
        "release_operations_publication": repo_rel(paths.release_operations_publication),
        "release_operations_end_to_end": repo_rel(paths.release_operations_end_to_end),
        "release_evidence_index": repo_rel(paths.release_evidence_index),
    }


def build_dashboard_model(
    paths: DistributionCredibilityDashboardPaths,
    inputs: DistributionCredibilityDashboardInputs,
) -> DistributionCredibilityDashboardModel:
    failures = validate_dashboard_inputs(inputs)
    warning_count = int(inputs.release_operations_publication.get("warning_count", 0))
    trust_signals = build_trust_signals(paths, inputs, warning_count=warning_count)
    required_drill_steps = inputs.release_drill_policy.get("required_drill_steps", [])
    operator_actions = [
        str(incident["required_action"])
        for incident in inputs.operator_policy.get("incident_classes", [])
        if isinstance(incident, dict) and isinstance(incident.get("required_action"), str)
    ]

    return DistributionCredibilityDashboardModel(
        status="PASS" if not failures else "FAIL",
        trust_state=trust_state_for_signals(trust_signals, failures),
        release_id=str(inputs.release_manifest.get("primary_package_manifest_sha256", "")),
        release_version=str(inputs.release_manifest.get("release_version", "v0.11")),
        warning_count=warning_count,
        upstream_reports=build_upstream_reports(paths),
        trust_signals=trust_signals,
        required_drill_steps=required_drill_steps,
        install_docs=inputs.install_doc_surface.get("primary_docs", []),
        release_docs=inputs.install_doc_surface.get("release_docs", []),
        operator_states=list(inputs.operator_policy.get("states", [])),
        operator_actions=operator_actions,
        release_drill_policy=inputs.release_drill_policy,
        workflow_surface=inputs.workflow_surface,
        dashboard_schema=inputs.schema_surface.get("dashboard_schema"),
        trust_report_schema=inputs.schema_surface.get("trust_report_schema"),
        artifact_surface=inputs.artifact_surface,
        failures=failures,
    )
