"""External-validation trust and artifact owner contracts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


EXTERNAL_VALIDATION_OWNER_CONTRACT_ID = (
    "objc3c.external_validation.owner.contracts.v1"
)


@dataclass(frozen=True)
class ExternalValidationOwnerContract:
    contract_id: str
    source_owner: str
    trust_policy_owner: str
    intake_owner: str
    quarantine_owner: str
    artifact_owner: str
    workflow_owner: str
    public_claim_owner: str
    owned_actions: tuple[str, ...]
    validate_child_actions: tuple[str, ...]
    accepted_trust_states: tuple[str, ...]
    quarantine_trust_states: tuple[str, ...]
    required_report_contracts: tuple[tuple[str, str], ...]
    hard_cutover_guardrails: tuple[tuple[str, Any], ...]

    def hard_cutover_policy(self) -> dict[str, Any]:
        return dict(self.hard_cutover_guardrails)

    def action_owner_map(self) -> dict[str, dict[str, str]]:
        return {
            action: {
                "source_owner": self.source_owner,
                "trust_policy_owner": self.trust_policy_owner,
                "intake_owner": self.intake_owner,
                "quarantine_owner": self.quarantine_owner,
                "artifact_owner": self.artifact_owner,
                "workflow_owner": self.workflow_owner,
                "public_claim_owner": self.public_claim_owner,
            }
            for action in self.owned_actions
        }

    def to_payload(self) -> dict[str, Any]:
        return {
            "contract_id": self.contract_id,
            "schema_version": 1,
            "source_owner": self.source_owner,
            "trust_policy_owner": self.trust_policy_owner,
            "intake_owner": self.intake_owner,
            "quarantine_owner": self.quarantine_owner,
            "artifact_owner": self.artifact_owner,
            "workflow_owner": self.workflow_owner,
            "public_claim_owner": self.public_claim_owner,
            "owned_actions": list(self.owned_actions),
            "validate_child_actions": list(self.validate_child_actions),
            "accepted_trust_states": list(self.accepted_trust_states),
            "quarantine_trust_states": list(self.quarantine_trust_states),
            "required_report_contracts": dict(self.required_report_contracts),
            "hard_cutover_guardrails": self.hard_cutover_policy(),
            "action_owner_map": self.action_owner_map(),
        }


EXTERNAL_VALIDATION_OWNER_CONTRACT = ExternalValidationOwnerContract(
    contract_id=EXTERNAL_VALIDATION_OWNER_CONTRACT_ID,
    source_owner="external-validation-source-surface",
    trust_policy_owner="external-validation-trust-policy",
    intake_owner="external-validation-intake",
    quarantine_owner="external-validation-quarantine",
    artifact_owner="external-validation-artifacts",
    workflow_owner="external-validation-workflow",
    public_claim_owner="external-validation-public-claims",
    owned_actions=(
        "check-external-validation-surface",
        "test-external-validation-replay",
        "publish-external-repro-corpus",
        "check-external-support-claim-gate",
        "validate-external-validation",
        "validate-external-validation-integration",
    ),
    validate_child_actions=(
        "check-external-validation-surface",
        "test-external-validation-replay",
        "publish-external-repro-corpus",
        "check-external-support-claim-gate",
    ),
    accepted_trust_states=("accepted",),
    quarantine_trust_states=("quarantined", "rejected"),
    required_report_contracts=(
        (
            "tmp/reports/external-validation/source-surface-summary.json",
            "objc3c.external_validation.source.surface.summary.v1",
        ),
        (
            "tmp/reports/external-validation/intake-replay-summary.json",
            "objc3c.external_validation.intake.replay.summary.v1",
        ),
        (
            "tmp/reports/external-validation/publication-summary.json",
            "objc3c.external_validation.publication.summary.v1",
        ),
        (
            "tmp/reports/external-validation/support-claim-gate-summary.json",
            "objc3c.external_validation.support_claim_gate.summary.v1",
        ),
        (
            "tmp/reports/external-validation/integration-summary.json",
            "objc3c.external_validation.integration.summary.v1",
        ),
    ),
    hard_cutover_guardrails=(
        ("retired_route_trust_route_allowed", False),
        ("local_only_validation_claim_allowed", False),
        ("evidence_log_validation_claim_allowed", False),
        ("wrapper_only_action_surface_allowed", False),
        ("unaccepted_fixture_publication_allowed", False),
        ("quarantined_fixture_capability_truth_allowed", False),
        ("artifact_owner_required_for_public_claim", True),
        ("intake_owner_required_for_trust_promotion", True),
        ("quarantine_owner_required_for_blocking_evidence", True),
    ),
)


def external_validation_owner_contract() -> dict[str, Any]:
    return EXTERNAL_VALIDATION_OWNER_CONTRACT.to_payload()


def external_validation_action_owner_map() -> dict[str, dict[str, str]]:
    return EXTERNAL_VALIDATION_OWNER_CONTRACT.action_owner_map()


def require_external_validation_action(action_name: str) -> None:
    if action_name not in EXTERNAL_VALIDATION_OWNER_CONTRACT.owned_actions:
        raise ValueError(f"{action_name} is not an external-validation owned action")


__all__ = [
    "EXTERNAL_VALIDATION_OWNER_CONTRACT",
    "EXTERNAL_VALIDATION_OWNER_CONTRACT_ID",
    "ExternalValidationOwnerContract",
    "external_validation_action_owner_map",
    "external_validation_owner_contract",
    "require_external_validation_action",
]
