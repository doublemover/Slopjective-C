from runtime_runnable_closure_contracts_support import (
    RUNTIME_CLOSURE_CLAIM_KIND,
    RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES,
    RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS,
    RUNTIME_CLOSURE_OWNER_METADATA,
    RUNTIME_CLOSURE_PUBLICATION_MODE,
    runtime_closure_action_groups_by_action,
)


def runtime_closure_runnable_actions_publish_owner_contract_metadata() -> None:
    groups = runtime_closure_action_groups_by_action()

    for action, (
        owner_contract,
        boundary_inventory,
        executable_proof_contract,
    ) in RUNTIME_CLOSURE_OWNER_METADATA.items():
        group = groups[action]

        assert group.owner_contract == owner_contract
        assert group.boundary_inventory == boundary_inventory
        assert group.executable_proof_contract == executable_proof_contract
        assert group.claim_kind == RUNTIME_CLOSURE_CLAIM_KIND
        assert group.claim_publication_mode == RUNTIME_CLOSURE_PUBLICATION_MODE
        assert set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES).issubset(
            group.forbidden_claim_shapes
        )
        assert set(RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS).issubset(
            group.hard_cutover_requirements
        )
