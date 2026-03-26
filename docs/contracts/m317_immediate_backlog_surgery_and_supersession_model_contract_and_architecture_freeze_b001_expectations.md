# M317 Immediate Backlog Surgery and Supersession Model Contract and Architecture Freeze Expectations (B001)

Contract ID: `objc3c-cleanup-immediate-backlog-surgery-supersession-model/m317-b001-v1`

## Required outcomes

- Publish one canonical tracked surgery model at `spec/planning/compiler/m317/m317_b001_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze_model.json`.
- Consume the current-state map frozen by `M317-A002`.
- Freeze the allowed backlog-surgery actions:
  - `narrow_in_place`
  - `corrective_tranche_consumes_existing_scope`
  - `boundary_preserve`
  - `cleanup_first_block`
  - `concept_hold`
  - `future_dependency_rewrite`
- Freeze the prohibited surgery patterns:
  - duplicate ownership
  - unlabeled or blockerless roadmap issues
  - using `M277-B002` or `M288` as cleanup dumping grounds
  - starting downstream runtime work before cleanup-first completion
- Backlog metadata proof for the live open backlog must remain:
  - `0` unlabeled open issues
  - `0` open issues missing an execution-order marker
- Validation evidence lands under `tmp/reports/m317/M317-B001/`.
