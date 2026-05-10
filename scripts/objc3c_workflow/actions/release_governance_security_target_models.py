"""Security-hardening workflow target models."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..action_spec import ActionSpec
from .release_governance_security_names import SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS


@dataclass(frozen=True)
class SecurityHardeningOwnerContract:
    owner_id: str
    owner: str
    source_contract: str
    claim_authority: str
    required_actions: tuple[str, ...]
    forbidden_claims: tuple[str, ...]

    def as_fixture_contract(self) -> dict[str, object]:
        return {
            "source_contract": self.source_contract,
            "owner": self.owner,
            "claim_authority": self.claim_authority,
            "required_actions": list(self.required_actions),
            "forbidden_claims": list(self.forbidden_claims),
        }


@dataclass(frozen=True)
class SecurityHardeningTarget:
    action_name: str
    summary: str
    backend: str
    guarantee_owner: str
    script: Path | None
    validation_tier: str
    owner_contract_ids: tuple[str, ...] = ()
    hard_cutover_guardrails: tuple[tuple[str, object], ...] = (
        SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS
    )

    def to_action_spec(self) -> ActionSpec:
        guardrails = dict(self.hard_cutover_guardrails)
        disallowed_flags = (
            "evidence_log_security_proof_allowed",
            "generated_report_capability_truth_allowed",
            "local_tabletop_capability_truth_allowed",
            "retired_route_claims_allowed",
            "trust_bypass_claims_allowed",
            "wrapper_only_security_actions_allowed",
        )
        if any(guardrails.get(flag) for flag in disallowed_flags):
            raise ValueError(f"{self.action_name} cannot publish bypass security claims")
        owner_contracts = ", ".join(self.owner_contract_ids)
        guarantee_owner = self.guarantee_owner
        if owner_contracts:
            guarantee_owner = f"{owner_contracts}: {guarantee_owner}"
        return ActionSpec(
            self.action_name,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=guarantee_owner,
        )


__all__ = [
    "SecurityHardeningOwnerContract",
    "SecurityHardeningTarget",
]
