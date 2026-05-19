from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import repo_rel

from objc3c_distribution_credibility_dashboard.input_loading import DistributionCredibilityDashboardInputs
from objc3c_distribution_credibility_dashboard.paths import DistributionCredibilityDashboardPaths
from objc3c_distribution_credibility_dashboard.validation import EXPECTED_TRUST_SIGNAL_IDS


def signal_status(passed: bool, *, warning: bool = False) -> str:
    if not passed:
        return "FAIL"
    if warning:
        return "WARN"
    return "PASS"


def signal_definitions(inputs: DistributionCredibilityDashboardInputs) -> dict[str, dict[str, Any]]:
    signals = inputs.trust_architecture.get("trust_signals", [])
    if not isinstance(signals, list):
        return {}
    return {
        str(signal["signal_id"]): signal
        for signal in signals
        if isinstance(signal, dict) and signal.get("signal_id") in EXPECTED_TRUST_SIGNAL_IDS
    }


def build_trust_signals(
    paths: DistributionCredibilityDashboardPaths,
    inputs: DistributionCredibilityDashboardInputs,
    *,
    warning_count: int,
) -> list[dict[str, Any]]:
    definitions = signal_definitions(inputs)
    signal_payloads = {
        "release-foundation-lineage": {
            "signal_id": "release-foundation-lineage",
            "status": signal_status(
                inputs.release_manifest.get("reproducibility_match") is True
                and isinstance(inputs.release_manifest.get("release_evidence_index_sha256"), str)
                and bool(inputs.release_manifest.get("release_evidence_index_sha256"))
            ),
            "source_path": repo_rel(paths.release_foundation_manifest),
            "detail": "release manifest retains reproducibility and evidence-index linkage",
        },
        "package-channel-install-smoke": {
            "signal_id": "package-channel-install-smoke",
            "status": signal_status(
                inputs.package_channels.get("status") == "PASS"
                and all(
                    isinstance(inputs.package_channels.get(key), str)
                    for key in ("portable_archive", "installer_archive", "offline_archive")
                )
            ),
            "source_path": repo_rel(paths.package_channels_end_to_end),
            "detail": "package channel install and rollback smoke stayed executable on the packaged release surface",
        },
        "release-operations-metadata": {
            "signal_id": "release-operations-metadata",
            "status": signal_status(
                inputs.release_operations_publication.get("status") == "PASS"
                and inputs.release_operations_end_to_end.get("status") == "PASS"
            ),
            "source_path": repo_rel(paths.release_operations_end_to_end),
            "detail": f"release operations publication remained coherent across {warning_count} expected compatibility warnings",
        },
        "release-evidence-gate": {
            "signal_id": "release-evidence-gate",
            "status": signal_status(
                inputs.release_evidence.get("schema_id") == "objc3-conformance-evidence-index/v1"
            ),
            "source_path": repo_rel(paths.release_evidence_index),
            "detail": "release evidence index exists for the shipped conformance artifact set",
        },
    }

    rendered: list[dict[str, Any]] = []
    for signal_id in EXPECTED_TRUST_SIGNAL_IDS:
        payload = dict(signal_payloads[signal_id])
        definition = definitions.get(signal_id, {})
        payload["artifact_source"] = definition.get("artifact_source", "")
        payload["claim_boundary"] = definition.get("claim_boundary", "")
        rendered.append(payload)
    return rendered


def trust_state_for_signals(trust_signals: list[dict[str, Any]], failures: list[str]) -> str:
    if any(signal["status"] == "FAIL" for signal in trust_signals) or failures:
        return "blocked"
    if any(signal["status"] == "WARN" for signal in trust_signals):
        return "degraded"
    return "ready"
