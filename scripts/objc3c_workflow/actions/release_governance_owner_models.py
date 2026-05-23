"""Release-governance owner contract models."""

from __future__ import annotations

from dataclasses import dataclass

from ..action_spec import ActionSpec


@dataclass(frozen=True)
class ReleaseGateOwner:
    gate_id: str
    source_owner: str
    gate_owner: str
    blocker_owner: str
    source_surface: str
    workflow_surface: str
    owned_actions: tuple[str, ...]
    upstream_owner_actions: tuple[str, ...] = ()
    workflow_child_actions: tuple[str, ...] = ()
    hard_cutover_guardrails: tuple[tuple[str, object], ...] = ()

    def owner_policy(self) -> dict[str, object]:
        policy: dict[str, object] = {
            "source_owner": self.source_owner,
            "gate_owner": self.gate_owner,
            "blocker_owner": self.blocker_owner,
            "evidence_log_allowed": False,
            "owned_actions": list(self.owned_actions),
        }
        if self.hard_cutover_guardrails:
            policy["hard_cutover_guardrails"] = dict(self.hard_cutover_guardrails)
        return policy

    def workflow_owner_policy(self) -> dict[str, object]:
        policy = self.owner_policy()
        policy["upstream_owner_actions"] = list(self.upstream_owner_actions)
        policy["workflow_child_actions"] = list(self.workflow_child_actions)
        return policy

    def action_owner_map(self) -> dict[str, dict[str, str]]:
        return {
            action: {
                "source_owner": self.source_owner,
                "gate_owner": self.gate_owner,
                "blocker_owner": self.blocker_owner,
            }
            for action in self.owned_actions
        }


@dataclass(frozen=True)
class ReleaseGovernanceActionContract:
    action: str
    summary: str
    backend: str
    gate_id: str
    validation_tier: str
    guarantee_owner: str
    evidence_log_allowed: bool = False
    pass_through_args: bool = False

    def to_action_spec(self) -> ActionSpec:
        if self.evidence_log_allowed:
            raise ValueError(f"{self.action} cannot be registered as evidence-log")
        from .release_governance_gate_owners import RELEASE_GATE_OWNERS

        owner = RELEASE_GATE_OWNERS[self.gate_id]
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=(
                f"{owner.source_owner} -> {owner.gate_owner} -> "
                f"{owner.blocker_owner}: {self.guarantee_owner}"
            ),
            pass_through_args=self.pass_through_args,
        )


__all__ = ["ReleaseGateOwner", "ReleaseGovernanceActionContract"]
