"""Typed action groups for runtime-runnable workflow owners."""

from __future__ import annotations

from collections.abc import Sequence
from dataclasses import dataclass

from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec

RUNTIME_RUNNABLE_VALIDATION_TIER = "full"
RUNTIME_CLOSURE_PUBLICATION_MODE = "checked-in-owner-contract-plus-executable-proof"
RUNTIME_CLOSURE_CLAIM_KIND = "runtime-closure-owner-contract"
RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES = (
    "fallback-runtime-behavior",
    "evidence-log-runtime-closure",
    "compatibility-shim-runtime-closure",
    "wrapper-only-runnable-action",
    "public-runtime-abi-widening-without-source-owner",
    "generated-evidence-log-source-truth",
)
RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS = (
    "no-fallback-runtime-closure-claims",
    "no-evidence-log-runtime-closure-publication",
    "no-generated-report-as-source-authority",
    "runtime-closure-publication-requires-checked-in-owner-contract",
)
RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER = "runtime-closure-owner-contract"


@dataclass(frozen=True)
class RuntimeClosureForbiddenClaim:
    shape: str
    owner: str
    policy_field: str
    required_value: object
    failure_mode: str

    def payload(self) -> dict[str, object]:
        return {
            "shape": self.shape,
            "owner": self.owner,
            "policy_field": self.policy_field,
            "required_value": self.required_value,
            "failure_mode": self.failure_mode,
        }


RUNTIME_CLOSURE_FORBIDDEN_CLAIM_CONTRACTS: tuple[
    RuntimeClosureForbiddenClaim, ...
] = (
    RuntimeClosureForbiddenClaim(
        "fallback-runtime-behavior",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "fallback_runtime_semantics_allowed",
        False,
        "fail-closed",
    ),
    RuntimeClosureForbiddenClaim(
        "evidence-log-runtime-closure",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "evidence_log_executable_proof_claims_allowed",
        False,
        "fail-closed",
    ),
    RuntimeClosureForbiddenClaim(
        "compatibility-shim-runtime-closure",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "compatibility_runtime_semantics_allowed",
        False,
        "fail-closed",
    ),
    RuntimeClosureForbiddenClaim(
        "wrapper-only-runnable-action",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "wrapper_only_runnable_actions_allowed",
        False,
        "fail-closed",
    ),
    RuntimeClosureForbiddenClaim(
        "public-runtime-abi-widening-without-source-owner",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "public_claims_require_executable_proof",
        True,
        "fail-closed",
    ),
    RuntimeClosureForbiddenClaim(
        "generated-evidence-log-source-truth",
        RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER,
        "generated_reports_are_source",
        False,
        "fail-closed",
    ),
)


@dataclass(frozen=True)
class RuntimeRunnableActionGroup:
    action: str
    summary: str
    backend: str
    handler: ActionHandler
    guarantee_owner: str
    validation_tier: str = RUNTIME_RUNNABLE_VALIDATION_TIER
    owner_contract: str = ""
    boundary_inventory: str = ""
    executable_proof_contract: str = ""
    claim_kind: str = ""
    claim_publication_mode: str = ""
    forbidden_claim_shapes: tuple[str, ...] = ()
    hard_cutover_requirements: tuple[str, ...] = ()

    def spec(self) -> ActionSpec:
        return ActionSpec(
            self.action,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=self.guarantee_owner,
        )


def runtime_runnable_action_specs(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> dict[str, ActionSpec]:
    return {group.action: group.spec() for group in _strict_groups(groups)}


def runtime_runnable_action_handlers(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> dict[str, ActionHandler]:
    return {group.action: group.handler for group in _strict_groups(groups)}


def _strict_groups(
    groups: Sequence[RuntimeRunnableActionGroup],
) -> tuple[RuntimeRunnableActionGroup, ...]:
    strict_groups = tuple(groups)
    action_names = [group.action for group in strict_groups]
    if len(action_names) != len(set(action_names)):
        raise ValueError(f"duplicate runtime-runnable workflow actions: {action_names}")
    for group in strict_groups:
        if group.spec().action != group.action:
            raise ValueError(f"runtime-runnable action key mismatch: {group.action}")
        _validate_runtime_closure_contract_metadata(group)
    return strict_groups


def _validate_runtime_closure_contract_metadata(group: RuntimeRunnableActionGroup) -> None:
    contract_fields = (
        group.owner_contract,
        group.boundary_inventory,
        group.executable_proof_contract,
        group.claim_kind,
        group.claim_publication_mode,
        *group.forbidden_claim_shapes,
        *group.hard_cutover_requirements,
    )
    if not any(contract_fields):
        return
    if not all(
        (
            group.owner_contract,
            group.boundary_inventory,
            group.executable_proof_contract,
            group.claim_kind == RUNTIME_CLOSURE_CLAIM_KIND,
            group.claim_publication_mode == RUNTIME_CLOSURE_PUBLICATION_MODE,
            set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES).issubset(
                group.forbidden_claim_shapes
            ),
            set(RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS).issubset(
                group.hard_cutover_requirements
            ),
        )
    ):
        raise ValueError(
            f"incomplete runtime closure owner metadata for {group.action}"
        )


def runtime_closure_forbidden_claim_contracts() -> list[dict[str, object]]:
    return [contract.payload() for contract in RUNTIME_CLOSURE_FORBIDDEN_CLAIM_CONTRACTS]


__all__ = [
    "RUNTIME_CLOSURE_CLAIM_KIND",
    "RUNTIME_CLOSURE_FORBIDDEN_CLAIM_CONTRACTS",
    "RUNTIME_CLOSURE_FORBIDDEN_CLAIM_OWNER",
    "RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES",
    "RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS",
    "RUNTIME_CLOSURE_PUBLICATION_MODE",
    "RUNTIME_RUNNABLE_VALIDATION_TIER",
    "RuntimeClosureForbiddenClaim",
    "RuntimeRunnableActionGroup",
    "runtime_closure_forbidden_claim_contracts",
    "runtime_runnable_action_handlers",
    "runtime_runnable_action_specs",
]
