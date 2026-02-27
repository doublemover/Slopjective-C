# Fixture: M14 Report Happy Path

## Drift findings table

| drift_id | class | canonical_home | impacted_sections | am_matrix_rows | rel03_impact | summary | owner | target_date | status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `AM-AUDIT-2026Q2-01` | `normative conflict` | Part 6 (`#part-6-6`) | `#am-6-5`, `#part-6-6-1`, `#part-6-6-4` | `AM-T07`, `AM-T08`, `AM-T13` | `blocking` | Normative conflict baseline. | `owner@example.com` | `2026-03-15` | `open` |
| `AM-AUDIT-2026Q2-02` | `missing example` | Part 7 (`#part-7-6-5`) | `#part-7-6-5`, `#part-8-3`, `#part-8-6` | `AM-T12`, `AM-T14` | `advisory` | Missing example baseline. | `owner@example.com` | `2026-03-22` | `tracked` |
| `AM-AUDIT-2026Q2-03` | `editorial mismatch` | Part 0 (`#part-0-4-1`) | `#part-0-4-1`, `#part-3-4-5` | `AM-T09` | `advisory` | Editorial mismatch baseline. | `owner@example.com` | `2026-03-29` | `tracked` |

## REL-03 impact snapshot (`EDGE-V013-017`)

| rel_dependency | requirement | impacted_drift_ids | current risk | release-note requirement |
| --- | --- | --- | --- | --- |
| `EDGE-V013-017` (`V013-SPEC-03 -> V013-REL-03`) | Kickoff packet must cite current abstract-machine sync status. | `AM-AUDIT-2026Q2-01`, `AM-AUDIT-2026Q2-02`, `AM-AUDIT-2026Q2-03` | `medium` (`1` blocking + `2` advisory findings) | `V013-REL-03` must cite this report and include unresolved drift IDs/statuses in kickoff handoff. |

## Deterministic remediation priority order

Priority scoring model:

- `class_weight`: `normative conflict=3`, `missing example=2`, `editorial mismatch=1`
- `rel03_weight`: `blocking=2`, `advisory=1`
- `am_row_count`: number of AM matrix rows directly touched by the finding
- `priority_score = (100 * class_weight) + (10 * rel03_weight) + am_row_count`

Tie-break rule: if scores are equal, sort by lexical `drift_id`.
