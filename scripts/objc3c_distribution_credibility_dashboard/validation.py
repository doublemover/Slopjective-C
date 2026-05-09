from __future__ import annotations

from objc3c_distribution_credibility_dashboard.input_loading import DistributionCredibilityDashboardInputs


def validate_dashboard_inputs(inputs: DistributionCredibilityDashboardInputs) -> list[str]:
    failures: list[str] = []
    if inputs.release_manifest.get("contract_id") != "objc3c.release.foundation.manifest.v1":
        failures.append("release-foundation manifest contract drifted")
    if inputs.package_channels.get("status") != "PASS":
        failures.append("package-channels end-to-end summary did not pass")
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
    if inputs.install_doc_surface.get("contract_id") != (
        "objc3c.distribution.credibility.install.release.doc.surface.v1"
    ):
        failures.append("install release doc surface contract drifted")
    if inputs.schema_surface.get("contract_id") != "objc3c.distribution.credibility.schema.surface.v1":
        failures.append("schema surface contract drifted")
    if inputs.artifact_surface.get("contract_id") != "objc3c.distribution.credibility.artifact.surface.v1":
        failures.append("artifact surface contract drifted")
    return failures
