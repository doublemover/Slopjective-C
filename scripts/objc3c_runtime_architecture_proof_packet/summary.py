"""Summary payload construction for runtime architecture proof packets."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.paths import repo_rel

from .contracts import (
    HARNESS_SUMMARY_PATH,
    PACKET_SURFACE_KEYS,
    PROOF_PACKET_CONTRACT_ID,
    PROOF_PACKET_PATH,
    PROOF_PACKET_SURFACE_CONTRACT_ID,
    SURFACE_KEYS,
)
from .validation_checks import ValidatedRuntimeArchitectureProofPacket


def build_proof_packet(validated: ValidatedRuntimeArchitectureProofPacket) -> dict[str, Any]:
    public_workflow_report = validated.public_workflow_report
    runtime_acceptance_report = validated.runtime_acceptance_report
    packet: dict[str, Any] = {
        "contract_id": PROOF_PACKET_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "proof_packet_surface": {
            "contract_id": PROOF_PACKET_SURFACE_CONTRACT_ID,
            "runner_path": "scripts/check_objc3c_runtime_architecture_proof_packet.py",
            "packet_path": repo_rel(PROOF_PACKET_PATH),
            "harness_summary_path": repo_rel(HARNESS_SUMMARY_PATH),
            "public_workflow_report_path": repo_rel(validated.public_workflow_report_path),
            "runtime_acceptance_report_path": repo_rel(validated.runtime_acceptance_report_path),
            "required_surface_keys": ["claim_boundary", *SURFACE_KEYS],
            "claim_boundary_alignment_rule": "composite-claim-boundary-must-match-between-shared-harness-and-public-workflow-and-must-carry-the-runtime-acceptance-suite-as-an-authoritative-child-surface",
            "child_step_report_paths": validated.child_report_paths,
            "requires_compile_coupled_child_reports": True,
        },
        "composite_claim_boundary": public_workflow_report["claim_boundary"],
        "runtime_acceptance_claim_boundary": runtime_acceptance_report["claim_boundary"],
    }
    for surface_key in PACKET_SURFACE_KEYS:
        packet[surface_key] = runtime_acceptance_report[surface_key]
    packet["proof_chain"] = [
        {
            "kind": "runtime-acceptance-report",
            "report_path": repo_rel(validated.runtime_acceptance_report_path),
            "case_count": runtime_acceptance_report.get("case_count"),
        },
        {
            "kind": "public-workflow-report",
            "report_path": repo_rel(validated.public_workflow_report_path),
            "action": public_workflow_report.get("action"),
            "step_count": len(public_workflow_report.get("steps", [])),
        },
        {
            "kind": "shared-harness-summary",
            "report_path": repo_rel(HARNESS_SUMMARY_PATH),
            "suite_id": validated.suite_summary.get("suite_id"),
            "suite_count": validated.harness_summary.get("suite_count"),
        },
    ]
    return packet
