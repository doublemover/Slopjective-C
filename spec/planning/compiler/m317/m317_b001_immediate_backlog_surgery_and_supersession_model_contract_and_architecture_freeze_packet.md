# M317-B001 Packet: Immediate backlog surgery and supersession model - Contract and architecture freeze

## Contract

- Contract ID: `objc3c-cleanup-immediate-backlog-surgery-supersession-model/m317-b001-v1`
- Dependency contract: `objc3c-cleanup-current-github-amendment-supersession-map/m317-a002-v1`
- Model file: `spec/planning/compiler/m317/m317_b001_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze_model.json`

## Scope

- Freeze the policy that governs immediate backlog surgery for the cleanup-first program.
- Define which edits are allowed, when they are allowed, and what later issues must not do.
- Keep this issue at the contract boundary: it defines the model that `M317-B002` executes and `M317-B003` extends, rather than performing another round of issue edits itself.

## Allowed action families

1. `narrow_in_place`
2. `corrective_tranche_consumes_existing_scope`
3. `boundary_preserve`
4. `cleanup_first_block`
5. `concept_hold`
6. `future_dependency_rewrite`

## Acceptance

- The tracked model names the allowed actions, compatibility rules, prohibited patterns, and example issue or milestone anchors.
- Live GitHub validation proves the current backlog still conforms to the model.
- Next issue: `M317-B002`.
