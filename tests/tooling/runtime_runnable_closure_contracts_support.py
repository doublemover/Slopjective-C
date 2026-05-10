from __future__ import annotations

from runtime_runnable_action_support import (
    RUNTIME_CLOSURE_CLAIM_KIND,
    RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES,
    RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS,
    RUNTIME_CLOSURE_OWNER_METADATA,
    RUNTIME_CLOSURE_PUBLICATION_MODE,
    runtime_closure_forbidden_claim_contracts,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
)


def runtime_closure_action_groups_by_action() -> dict:
    return {
        group.action: group
        for group in (
            *runtime_runnable_conformance.RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS,
            *runtime_runnable_e2e.RUNTIME_RUNNABLE_E2E_ACTION_GROUPS,
        )
    }


def runtime_closure_forbidden_claims_by_shape() -> tuple[list[dict], dict]:
    contracts = runtime_closure_forbidden_claim_contracts()
    return contracts, {contract["shape"]: contract for contract in contracts}
