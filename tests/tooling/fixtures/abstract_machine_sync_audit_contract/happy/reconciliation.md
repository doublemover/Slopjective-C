# Fixture: M13 Reconciliation Happy Path

```json
{
  "contract_id": "V013-SPEC-02-RECON-v1",
  "normalized_topics": {
    "part_open_issue_status": { "part_0": "closed", "part_3": "closed", "part_10": "closed" }
  },
  "consumer_bindings": [
    { "consumer_seed_id": "V013-SPEC-03", "edge_ids": ["EDGE-V013-001"], "required_fields": ["normalized_topics.part_open_issue_status", "conflict_decisions"] },
    { "consumer_seed_id": "V013-REL-03", "edge_ids": ["EDGE-V013-017", "EDGE-V013-018"], "required_fields": ["conflict_decisions", "normalized_topics.part_open_issue_status"] }
  ]
}
```

| Consumer seed | Dependency edge | Consumption type | Required contract fields |
| --- | --- | --- | --- |
| `V013-SPEC-03` | `EDGE-V013-001` | Normative baseline import for abstract-machine sync audit refresh. | `normalized_topics.part_open_issue_status`, `conflict_decisions` |

| Consumer seed | Transitive path | Impact |
| --- | --- | --- |
| `V013-REL-03` | `V013-SPEC-02 -> V013-SPEC-03 -> V013-REL-03` and `V013-SPEC-02 -> V013-SPEC-04 -> V013-REL-03` | Kickoff packet correctness depends on this package’s normalized Part 0/3/10 and conflict outputs. |
