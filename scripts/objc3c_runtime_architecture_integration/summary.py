"""Summary payload construction for runtime architecture integration."""

from __future__ import annotations

from datetime import datetime, timezone
from typing import Any

from objc3c_tooling.paths import repo_rel

from .contracts import (
    FULL_HARNESS_SUMMARY_PATH,
    INTEGRATION_CONTRACT_ID,
    INTEGRATION_PAYLOAD_SURFACE_KEYS,
    INTEGRATION_SUMMARY_PATH,
    INTEGRATION_SURFACE_CONTRACT_ID,
    PROOF_PACKET_PATH,
    REQUIRED_STEP_ACTION_GROUPS,
    SURFACE_KEYS,
)
from .validation_checks import ValidatedRuntimeArchitecture


def build_integration_summary(validated: ValidatedRuntimeArchitecture) -> dict[str, Any]:
    public_workflow_report = validated.public_workflow_report
    payload: dict[str, Any] = {
        "contract_id": INTEGRATION_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "integration_surface": {
            "contract_id": INTEGRATION_SURFACE_CONTRACT_ID,
            "runner_path": "scripts/check_objc3c_runtime_architecture_integration.py",
            "summary_path": repo_rel(INTEGRATION_SUMMARY_PATH),
            "harness_summary_path": repo_rel(FULL_HARNESS_SUMMARY_PATH),
            "public_workflow_report_path": repo_rel(validated.public_workflow_report_path),
            "runtime_acceptance_report_path": repo_rel(validated.runtime_acceptance_report_path),
            "proof_packet_path": repo_rel(PROOF_PACKET_PATH),
            "required_surface_keys": ["claim_boundary", *SURFACE_KEYS],
            "required_step_action_groups": {
                group_name: list(accepted_actions)
                for group_name, accepted_actions in REQUIRED_STEP_ACTION_GROUPS
            },
            "requires_compile_coupled_full_workflow": True,
            "proof_packet_must_match_full_workflow": True,
        },
        "claim_boundary": validated.public_claim_boundary,
        "runtime_acceptance_claim_boundary": validated.runtime_claim_boundary,
    }
    for surface_key in INTEGRATION_PAYLOAD_SURFACE_KEYS:
        payload[surface_key] = public_workflow_report[surface_key]
    payload.update(
        {
            "full_workflow_step_actions": validated.observed_actions,
            "full_workflow_child_report_paths": validated.child_report_paths,
            "proof_packet_child_report_paths": validated.proof_child_report_paths,
            "proof_chain": [
                {
                    "kind": "public-workflow-report",
                    "report_path": repo_rel(validated.public_workflow_report_path),
                    "action": public_workflow_report.get("action"),
                    "step_count": len(public_workflow_report.get("steps", [])),
                },
                {
                    "kind": "shared-harness-summary",
                    "report_path": repo_rel(FULL_HARNESS_SUMMARY_PATH),
                    "suite_id": validated.suite_summary.get("suite_id"),
                    "suite_count": validated.harness_summary.get("suite_count"),
                },
                {
                    "kind": "runtime-architecture-proof-packet",
                    "report_path": repo_rel(PROOF_PACKET_PATH),
                    "proof_chain_length": len(validated.proof_packet.get("proof_chain", [])),
                },
                {
                    "kind": "runtime-acceptance-report",
                    "report_path": repo_rel(validated.runtime_acceptance_report_path),
                    "case_count": validated.runtime_acceptance_report.get("case_count"),
                },
            ],
        }
    )
    return payload
