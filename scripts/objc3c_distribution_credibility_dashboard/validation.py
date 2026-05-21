from __future__ import annotations

from objc3c_distribution_credibility_dashboard.input_loading import DistributionCredibilityDashboardInputs

EXPECTED_TRUST_SIGNAL_IDS = [
    "release-foundation-lineage",
    "package-channel-install-smoke",
    "release-operations-metadata",
    "release-evidence-gate",
]

EXPECTED_WORKFLOW_STEPS = [
    "validate-release-operations",
    "validate-package-install-distribution",
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
]

EXPECTED_ARTIFACT_PATHS = {
    "source_surface_summary": "tmp/reports/distribution-credibility/source-surface-summary.json",
    "schema_surface_summary": "tmp/reports/distribution-credibility/schema-surface-summary.json",
    "dashboard_summary": "tmp/reports/distribution-credibility/dashboard-summary.json",
    "publication_summary": "tmp/reports/distribution-credibility/publication-summary.json",
    "integration_summary": "tmp/reports/distribution-credibility/integration-summary.json",
    "end_to_end_summary": "tmp/reports/distribution-credibility/end-to-end-summary.json",
    "dashboard_artifact": "tmp/artifacts/distribution-credibility/dashboard/distribution-credibility-dashboard.json",
    "trust_report_json": "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.json",
    "trust_report_markdown": "tmp/artifacts/distribution-credibility/report/objc3c-distribution-trust-report.md",
}


def _string_list(payload: dict[str, object], key: str) -> list[str]:
    values = payload.get(key)
    if not isinstance(values, list) or not values:
        return []
    return [value for value in values if isinstance(value, str) and value]


def _trust_signal_ids(inputs: DistributionCredibilityDashboardInputs) -> list[str]:
    signals = inputs.trust_architecture.get("trust_signals")
    if not isinstance(signals, list):
        return []
    return [signal.get("signal_id") for signal in signals if isinstance(signal, dict)]


def validate_dashboard_inputs(inputs: DistributionCredibilityDashboardInputs) -> list[str]:
    failures: list[str] = []
    if inputs.release_manifest.get("contract_id") != "objc3c.release.foundation.manifest.v1":
        failures.append("release-foundation manifest contract drifted")
    if inputs.package_channels.get("status") != "PASS":
        failures.append("package-channels end-to-end summary did not pass")
    package_channel_owner_policy = inputs.package_channels.get("owner_policy")
    package_channel_guardrails = (
        package_channel_owner_policy.get("hard_cutover_guardrails", {})
        if isinstance(package_channel_owner_policy, dict)
        else {}
    )
    if not isinstance(package_channel_guardrails, dict) or (
        package_channel_guardrails.get("unsupported_host_success_allowed") is not False
    ):
        failures.append("package channel unsupported host guardrail widened")
    if inputs.package_install_distribution.get("contract_id") != (
        "objc3c.package_ecosystem.install_distribution_credibility.summary.v1"
    ):
        failures.append("package install distribution summary contract drifted")
    if inputs.package_install_distribution.get("status") != "PASS":
        failures.append("package install distribution summary did not pass")
    if inputs.package_install_distribution.get("network_policy") != "no-network-during-validation":
        failures.append("package install distribution network policy widened")
    if inputs.package_install_distribution.get("hosted_registry_support") != (
        "unsupported-fail-closed-if-claimed"
    ):
        failures.append("package install hosted registry support widened")
    if inputs.package_install_distribution.get("offline_restore_support") != (
        "local-cache-digest-checked"
    ):
        failures.append("package install offline restore support drifted")
    clean_start = inputs.package_install_distribution.get("clean_start")
    if not isinstance(clean_start, dict) or clean_start.get("stale_artifacts_allowed") is not False:
        failures.append("package install clean-start policy drifted")
    generated_paths = inputs.package_install_distribution.get("generated_paths")
    if not isinstance(generated_paths, list) or not generated_paths:
        failures.append("package install generated paths drifted")
    elif any(
        not isinstance(path, str)
        or not path.startswith("tmp/artifacts/package-ecosystem/install-validation/")
        for path in generated_paths
    ):
        failures.append("package install generated path escaped validation root")
    if inputs.package_install_distribution.get("missing_public_actions") != []:
        failures.append("package install public action surface drifted")
    if inputs.release_operations_publication.get("status") != "PASS":
        failures.append("release-operations publication summary did not pass")
    if inputs.release_operations_end_to_end.get("status") != "PASS":
        failures.append("release-operations end-to-end summary did not pass")
    if inputs.release_evidence.get("schema_id") != "objc3-conformance-evidence-index/v1":
        failures.append("release-evidence index schema drifted")

    required_drill_steps = inputs.release_drill_policy.get("required_drill_steps", [])
    if not isinstance(required_drill_steps, list) or len(required_drill_steps) < 5:
        failures.append("release drill policy required_drill_steps drifted")
    if inputs.operator_policy.get("states", []) != ["ready", "degraded", "blocked"]:
        failures.append("operator release states drifted")
    if inputs.source_surface.get("contract_id") != "objc3c.distribution.credibility.source.surface.v1":
        failures.append("source surface contract drifted")
    if inputs.trust_architecture.get("contract_id") != (
        "objc3c.distribution.credibility.trust.signal.architecture.v1"
    ):
        failures.append("trust signal architecture contract drifted")
    if inputs.trust_architecture.get("upstream_surfaces") != [
        "release-foundation",
        "packaging-channels",
        "package-ecosystem",
        "release-operations",
        "release-evidence",
    ]:
        failures.append("trust signal upstream surfaces drifted")
    if _trust_signal_ids(inputs) != EXPECTED_TRUST_SIGNAL_IDS:
        failures.append("trust signal order drifted")
    if inputs.trust_architecture.get("required_signal_order") != EXPECTED_TRUST_SIGNAL_IDS:
        failures.append("trust signal required order drifted")
    if inputs.install_doc_surface.get("contract_id") != (
        "objc3c.distribution.credibility.install.release.doc.surface.v1"
    ):
        failures.append("install release doc surface contract drifted")
    if len(_string_list(inputs.install_doc_surface, "primary_docs")) < 3:
        failures.append("install release primary docs drifted")
    if len(_string_list(inputs.install_doc_surface, "release_docs")) < 3:
        failures.append("install release runbook docs drifted")
    if inputs.schema_surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        failures.append("schema surface contract drifted")
    if inputs.artifact_surface.get("contract_id") != "objc3c.distribution.credibility.artifact.surface.v1":
        failures.append("artifact surface contract drifted")
    for field_name, expected_path in EXPECTED_ARTIFACT_PATHS.items():
        if inputs.artifact_surface.get(field_name) != expected_path:
            failures.append(f"artifact surface {field_name} drifted")
    if inputs.workflow_surface.get("contract_id") != "objc3c.distribution.credibility.workflow.surface.v1":
        failures.append("workflow surface contract drifted")
    if inputs.workflow_surface.get("integrated_required_steps") != EXPECTED_WORKFLOW_STEPS:
        failures.append("workflow integrated required steps drifted")
    if inputs.workflow_surface.get("validate_action") != "validate-distribution-credibility":
        failures.append("workflow validate action drifted")
    return failures
