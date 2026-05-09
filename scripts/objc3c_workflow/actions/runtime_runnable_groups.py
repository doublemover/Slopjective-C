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
    "report-only-runtime-closure",
    "compatibility-shim-runtime-closure",
    "wrapper-only-runnable-action",
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
        )
    ):
        raise ValueError(
            f"incomplete runtime closure owner metadata for {group.action}"
        )


__all__ = [
    "RUNTIME_CLOSURE_CLAIM_KIND",
    "RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES",
    "RUNTIME_CLOSURE_PUBLICATION_MODE",
    "RUNTIME_RUNNABLE_VALIDATION_TIER",
    "RuntimeRunnableActionGroup",
    "runtime_runnable_action_handlers",
    "runtime_runnable_action_specs",
]
