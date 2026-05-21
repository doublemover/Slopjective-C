"""Contract-specific distribution-credibility source-surface validators."""

from __future__ import annotations

from typing import Any

from .constants import EXPECTED_INTEGRATED_STEPS, EXPECTED_OUTPUTS, EXPECTED_TRUST_SIGNAL_IDS, EXPECTED_WORKFLOW_ACTIONS


def signal_ids(payload: dict[str, Any]) -> list[str]:
    signals = payload.get("trust_signals")
    if not isinstance(signals, list):
        return []
    return [signal.get("signal_id") for signal in signals if isinstance(signal, dict)]


def require_string_list(payload: dict[str, Any], field_name: str, *, minimum: int) -> list[str]:
    values = payload.get(field_name)
    if not isinstance(values, list) or len(values) < minimum:
        raise RuntimeError(f"{field_name} must contain at least {minimum} entries")
    rendered = [value for value in values if isinstance(value, str) and value]
    if len(rendered) != len(values):
        raise RuntimeError(f"{field_name} contained an invalid entry")
    return rendered


def validate_contract_payload(field_name: str, payload: dict[str, Any]) -> None:
    if field_name == "trust_signal_architecture":
        validate_trust_signal_architecture(payload)
    elif field_name == "package_install_distribution_credibility":
        validate_package_install_distribution_credibility(payload)
    elif field_name == "install_release_doc_surface":
        validate_install_release_doc_surface(payload)
    elif field_name == "operator_release_policy":
        validate_operator_release_policy(payload)
    elif field_name == "release_drill_policy":
        validate_release_drill_policy(payload)
    elif field_name == "artifact_surface":
        validate_artifact_surface(payload)
    elif field_name == "workflow_surface":
        validate_workflow_surface(payload)


def validate_trust_signal_architecture(payload: dict[str, Any]) -> None:
    if payload.get("upstream_surfaces") != [
        "release-foundation",
        "packaging-channels",
        "package-ecosystem",
        "release-operations",
        "release-evidence",
    ]:
        raise RuntimeError("trust_signal_architecture upstream surfaces drifted")
    if signal_ids(payload) != EXPECTED_TRUST_SIGNAL_IDS:
        raise RuntimeError("trust_signal_architecture signal order drifted")
    if payload.get("required_signal_order") != EXPECTED_TRUST_SIGNAL_IDS:
        raise RuntimeError("trust_signal_architecture required signal order drifted")
    for signal in payload.get("trust_signals", []):
        if not isinstance(signal, dict) or not signal.get("artifact_source") or not signal.get("claim_boundary"):
            raise RuntimeError("trust_signal_architecture contained an incomplete signal")


def validate_package_install_distribution_credibility(payload: dict[str, Any]) -> None:
    if payload.get("network_policy") != "no-network-during-validation":
        raise RuntimeError("package install distribution network policy drifted")
    if payload.get("network_resolution_support") != "unsupported":
        raise RuntimeError("package install distribution network resolution support drifted")
    if payload.get("hosted_registry_support") != "unsupported-fail-closed-if-claimed":
        raise RuntimeError("package install distribution hosted registry support drifted")
    generated_outputs = payload.get("generated_outputs")
    if not isinstance(generated_outputs, dict):
        raise RuntimeError("package install distribution generated outputs drifted")
    if generated_outputs.get("summary") != (
        "tmp/reports/package-ecosystem/install-distribution-credibility-summary.json"
    ):
        raise RuntimeError("package install distribution summary path drifted")
    if generated_outputs.get("clean_install_root") != (
        "tmp/artifacts/package-ecosystem/install-validation/clean-root"
    ):
        raise RuntimeError("package install distribution clean install root drifted")
    required_actions = payload.get("required_public_actions")
    if not isinstance(required_actions, list) or "validate-package-install-distribution" not in required_actions:
        raise RuntimeError("package install distribution action surface drifted")
    blocker_metadata = payload.get("blocker_metadata")
    blocking_conditions = (
        blocker_metadata.get("blocking_conditions", [])
        if isinstance(blocker_metadata, dict)
        else []
    )
    if not any("hosted registry" in str(condition) for condition in blocking_conditions):
        raise RuntimeError("package install distribution hosted registry blocker drifted")


def validate_install_release_doc_surface(payload: dict[str, Any]) -> None:
    require_string_list(payload, "primary_docs", minimum=3)
    require_string_list(payload, "release_docs", minimum=3)
    require_string_list(payload, "trust_report_inputs", minimum=4)


def validate_operator_release_policy(payload: dict[str, Any]) -> None:
    if payload.get("states") != ["ready", "degraded", "blocked"]:
        raise RuntimeError("operator_release_policy states drifted")
    require_string_list(payload, "blocking_conditions", minimum=4)
    require_string_list(payload, "degraded_conditions", minimum=3)


def validate_release_drill_policy(payload: dict[str, Any]) -> None:
    if payload.get("required_drill_steps") != [
        "stage-package-channels",
        "verify-install-smoke",
        "verify-clean-package-install",
        "verify-rollback-guidance",
        "verify-update-manifest-coherence",
        "verify-release-evidence-index",
    ]:
        raise RuntimeError("release_drill_policy required drill steps drifted")
    require_string_list(payload, "required_upstream_reports", minimum=3)


def validate_artifact_surface(payload: dict[str, Any]) -> None:
    for output_name, expected_path in EXPECTED_OUTPUTS.items():
        if payload.get(output_name) != expected_path:
            raise RuntimeError(f"artifact_surface {output_name} drifted")


def validate_workflow_surface(payload: dict[str, Any]) -> None:
    if payload.get("required_actions") != EXPECTED_WORKFLOW_ACTIONS:
        raise RuntimeError("workflow_surface required actions drifted")
    if payload.get("integrated_required_steps") != EXPECTED_INTEGRATED_STEPS:
        raise RuntimeError("workflow_surface integrated steps drifted")
    if payload.get("validate_action") != "validate-distribution-credibility":
        raise RuntimeError("workflow_surface validate action drifted")
