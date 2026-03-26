# M317-A001 Packet: Backlog overlap and correction inventory - Contract and architecture freeze

## Contract

- Contract ID: `objc3c-cleanup-backlog-overlap-correction-inventory/m317-a001-v1`
- Inventory file: `spec/planning/compiler/m317/m317_a001_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze_inventory.json`
- Dependency guidance file: `tmp/planning/cleanup_acceleration_program/github_issue_alteration_guidance.md`
- Execution-order file: `tmp/planning/cleanup_acceleration_program/execution_order.md`
- Metadata policy file: `tmp/planning/cleanup_acceleration_program/open_issue_metadata_policy.md`

## Scope

- Freeze the canonical overlap inventory before further backlog surgery work lands.
- Keep `M317-A001` at the inventory boundary: identify overlap families, critical issue sets, future consumer milestones, and current metadata rules without claiming that the later amendment work is finished here.
- Treat the overlap inventory as a machine-auditable source for:
  - `M317-A002` current issue supersession mapping
  - `M317-B001` immediate surgery policy
  - `M317-B002` current issue amendment execution
  - `M317-B003` post-`M292` dependency rewrites

## Frozen overlap families

1. `scaffold_retirement_scope_narrowing`
   - Narrow `#7399` to architecture-only retirement inventory work.
2. `realized_dispatch_corrective_overlap`
   - Sharpen `#7421`, `#7425`, and `#7428` so they require selector-pool-backed realized dispatch semantics and proof.
3. `synthesized_accessor_corrective_overlap`
   - Sharpen `#7434`, `#7438`, and `#7441` so they require synthesized getter/setter semantics, real LLVM IR bodies, and runtime-backed accessor execution.
4. `human_facing_superclean_boundary`
   - Keep `#7529-#7538` focused on human-facing polish and instruction coherence rather than structural cleanup and proof hygiene.
5. `future_post_m292_dependency_consumers`
   - Require `M293-M304` to consume the cleanup-first validation, command-surface, and proof-hygiene program instead of seeding more noisy patterns.

## Acceptance

- The tracked inventory JSON names the exact overlap families, issue sets, milestone sets, and corrective relationships.
- The local guidance docs, execution-order docs, and metadata policy agree with the inventory.
- Live GitHub proof shows the critical overlap issues and future milestones carry execution-order markers and that the open backlog is fully labeled and blocker-tagged.
- Next issue: `M317-A002`.
