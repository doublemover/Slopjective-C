#!/usr/bin/env python3
"""Validate the checked-in distribution-credibility source surface."""

from __future__ import annotations

import sys
from pathlib import Path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.json_io import write_report_json


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "distribution-credibility" / "source-surface-summary.json"
SURFACE_CONTRACT_ID = "objc3c.distribution.credibility.source.surface.v1"
SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.source.surface.summary.v1"

EXPECTED_CONTRACT_IDS = {
    "trust_signal_architecture": "objc3c.distribution.credibility.trust.signal.architecture.v1",
    "install_release_doc_surface": "objc3c.distribution.credibility.install.release.doc.surface.v1",
    "operator_release_policy": "objc3c.distribution.credibility.operator.release.policy.v1",
    "release_drill_policy": "objc3c.distribution.credibility.release.drill.policy.v1",
    "artifact_surface": "objc3c.distribution.credibility.artifact.surface.v1",
    "schema_surface": "objc3c.distribution.credibility.schema.surface.v1",
    "workflow_surface": "objc3c.distribution.credibility.workflow.surface.v1",
}

EXPECTED_TRUST_SIGNAL_IDS = [
    "release-foundation-lineage",
    "package-channel-install-smoke",
    "release-operations-metadata",
    "release-evidence-gate",
]

EXPECTED_WORKFLOW_ACTIONS = [
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
    "validate-distribution-credibility",
    "validate-distribution-credibility-end-to-end",
]

EXPECTED_INTEGRATED_STEPS = [
    "validate-release-operations",
    "check-distribution-credibility-surface",
    "check-distribution-credibility-schema-surface",
    "build-distribution-credibility-dashboard",
    "publish-distribution-credibility",
]

EXPECTED_OUTPUTS = {
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


def fail(message: str) -> int:
    print(f"distribution-credibility-source-surface: {message}", file=sys.stderr)
    return 1


def signal_ids(payload: dict[str, object]) -> list[str]:
    signals = payload.get("trust_signals")
    if not isinstance(signals, list):
        return []
    return [signal.get("signal_id") for signal in signals if isinstance(signal, dict)]


def require_string_list(payload: dict[str, object], field_name: str, *, minimum: int) -> list[str]:
    values = payload.get(field_name)
    if not isinstance(values, list) or len(values) < minimum:
        raise RuntimeError(f"{field_name} must contain at least {minimum} entries")
    rendered = [value for value in values if isinstance(value, str) and value]
    if len(rendered) != len(values):
        raise RuntimeError(f"{field_name} contained an invalid entry")
    return rendered


def validate_contract_payload(field_name: str, payload: dict[str, object]) -> None:
    if field_name == "trust_signal_architecture":
        if payload.get("upstream_surfaces") != [
            "release-foundation",
            "packaging-channels",
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
    elif field_name == "install_release_doc_surface":
        require_string_list(payload, "primary_docs", minimum=3)
        require_string_list(payload, "release_docs", minimum=3)
        require_string_list(payload, "trust_report_inputs", minimum=4)
    elif field_name == "operator_release_policy":
        if payload.get("states") != ["ready", "degraded", "blocked"]:
            raise RuntimeError("operator_release_policy states drifted")
        require_string_list(payload, "blocking_conditions", minimum=4)
        require_string_list(payload, "degraded_conditions", minimum=3)
    elif field_name == "release_drill_policy":
        if payload.get("required_drill_steps") != [
            "stage-package-channels",
            "verify-install-smoke",
            "verify-rollback-guidance",
            "verify-update-manifest-coherence",
            "verify-release-evidence-index",
        ]:
            raise RuntimeError("release_drill_policy required drill steps drifted")
        require_string_list(payload, "required_upstream_reports", minimum=3)
    elif field_name == "artifact_surface":
        for output_name, expected_path in EXPECTED_OUTPUTS.items():
            if payload.get(output_name) != expected_path:
                raise RuntimeError(f"artifact_surface {output_name} drifted")
    elif field_name == "workflow_surface":
        if payload.get("required_actions") != EXPECTED_WORKFLOW_ACTIONS:
            raise RuntimeError("workflow_surface required actions drifted")
        if payload.get("integrated_required_steps") != EXPECTED_INTEGRATED_STEPS:
            raise RuntimeError("workflow_surface integrated steps drifted")
        if payload.get("validate_action") != "validate-distribution-credibility":
            raise RuntimeError("workflow_surface validate action drifted")




def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != SURFACE_CONTRACT_ID:
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != "distribution-credibility-source-surface":
        return fail("unexpected source surface kind")
    if source_surface.get("schema_version") != 1:
        return fail("unexpected source surface schema_version")

    checked_paths: list[str] = [repo_rel(SOURCE_SURFACE)]
    for field_name, expected_contract_id in EXPECTED_CONTRACT_IDS.items():
        raw_path = source_surface.get(field_name)
        if not isinstance(raw_path, str) or not raw_path:
            return fail(f"{field_name} was missing from the source surface")
        target = ROOT / raw_path
        if not target.is_file():
            return fail(f"{field_name} referenced missing file {raw_path}")
        payload = load_json(target)
        if payload.get("contract_id") != expected_contract_id:
            return fail(f"{field_name} drifted from expected contract id {expected_contract_id}")
        try:
            validate_contract_payload(field_name, payload)
        except RuntimeError as exc:
            return fail(str(exc))
        checked_paths.append(raw_path)

    runbook = source_surface.get("runbook")
    if not isinstance(runbook, str) or not runbook:
        return fail("runbook was missing from the source surface")
    runbook_path = ROOT / runbook
    if not runbook_path.is_file():
        return fail(f"runbook referenced missing file {runbook}")
    checked_paths.append(runbook)

    for list_name in ("checked_in_sources", "machine_owned_output_roots", "explicit_non_goals"):
        items = source_surface.get(list_name)
        if not isinstance(items, list) or not items:
            return fail(f"{list_name} must be a non-empty list")
        if list_name != "checked_in_sources":
            continue
        for raw_path in items:
            if not isinstance(raw_path, str) or not raw_path:
                return fail(f"{list_name} contained an invalid path entry")
            target = ROOT / raw_path
            if not target.exists():
                return fail(f"{list_name} referenced missing path {raw_path}")
            checked_paths.append(raw_path)

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("distribution-credibility-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
