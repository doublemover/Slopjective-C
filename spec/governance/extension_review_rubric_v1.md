# Extension Review Rubric v1 (`C-04`)

_Normative governance artifact - 2026-02-23._

This rubric defines deterministic scoring and acceptance behavior for extension
review decisions. It is the canonical `C-04` artifact.

This document is aligned to:

- `spec/planning/issue_153_extension_review_rubric_package.md`
- `spec/governance/macro_derive_extension_charter_v1.md`

## 1. Scope

This rubric governs:

1. weighted criteria for safety, portability, ergonomics, and tooling cost,
2. score computation and evidence-quality adjustments,
3. acceptance thresholds and hard-fail overrides,
4. tie-break behavior for equal or near-equal outcomes.

## 2. Weighted Criteria

| Criterion ID | Criterion | Weight | Minimum evidence expectation |
| --- | --- | ---: | --- |
| `R1` | Safety and security risk management | 35 | Threat analysis, deterministic negative tests, and failure-mode diagnostics. |
| `R2` | Portability and cross-vendor interoperability | 25 | At least two independent implementations or one implementation plus independent verifier harness. |
| `R3` | Ergonomics and developer experience | 20 | Diagnostic quality examples, migration guidance, and adoption cost assessment. |
| `R4` | Tooling and operational cost | 20 | Build and CI cost profile, operational risk notes, and maintenance burden estimate. |

Total weight is fixed at `100`.

## 3. Score Scale and Formula

### 3.1 Raw score scale

| Raw score | Meaning |
| --- | --- |
| `0` | Unscorable or unacceptable due to missing evidence or blocking defect. |
| `1` | High risk and not acceptable for transition approval. |
| `2` | Weak quality with material unresolved gaps. |
| `3` | Adequate baseline quality. |
| `4` | Strong quality with reproducible evidence. |
| `5` | Exemplary quality and broad confidence. |

### 3.2 Computation

For criterion `i`:

- `raw_points_i = (raw_score_i / 5) * weight_i`
- `adjusted_points_i = raw_points_i * evidence_multiplier_i`

Overall adjusted score:

- `overall_adjusted_score = sum(adjusted_points_i)` in range `0..100`.

## 4. Decision Bands

| Adjusted score band | Decision outcome |
| --- | --- |
| `90..100` | `ACCEPT-STRONG` |
| `80..89.9` | `ACCEPT` |
| `70..79.9` | `CONDITIONAL-ACCEPT` |
| `60..69.9` | `DEFER` |
| `< 60` | `REJECT` |

## 5. Acceptance Threshold Rules

An extension review is accepted only if all checks pass:

1. `overall_adjusted_score >= 80`,
2. criterion floors:
   - `R1 >= 4`
   - `R2 >= 3`
   - `R3 >= 3`
   - `R4 >= 2`
3. no criterion score is `0` or `1`,
4. no hard-fail condition from Section 6 is active,
5. evidence-quality gate from Section 7 passes.

`CONDITIONAL-ACCEPT` is permitted only when:

- `R1 >= 4`,
- no hard-fail condition is active,
- remediation items have named owner and due date.

## 6. Hard-Fail Conditions

| Hard-fail ID | Trigger | Required disposition |
| --- | --- | --- |
| `HF-01` | Open critical security or safety issue without approved mitigation plan. | `REJECT` or emergency `hold`. |
| `HF-02` | Non-deterministic behavior in required extension path without deterministic fallback. | Minimum `DEFER`. |
| `HF-03` | Portability claims lack independent corroboration and replayable evidence. | `DEFER` until evidence is complete. |
| `HF-04` | Tooling cost introduces release or CI instability above approved risk budget without mitigation. | `DEFER` or `REJECT`. |
| `HF-05` | Provenance or traceability gap prevents audit replay. | `DEFER` until repaired. |

## 7. Evidence-Quality Grades

### 7.1 Grade definitions

| Grade | Meaning |
| --- | --- |
| `EQ0` | Missing, contradictory, stale, or non-reproducible evidence. |
| `EQ1` | Weak evidence with partial coverage or replay gaps. |
| `EQ2` | Sufficient reproducible evidence for decision support. |
| `EQ3` | Strong independently corroborated evidence with edge-case coverage. |

### 7.2 Multipliers

| Grade | Multiplier |
| --- | ---: |
| `EQ3` | `1.00` |
| `EQ2` | `0.95` |
| `EQ1` | `0.80` |
| `EQ0` | `0.00` |

### 7.3 Quality-gating rules

1. `EQ0` for `R1` or `R2` blocks acceptance (`HF-05`),
2. `EQ1` for `R1` caps outcome at `DEFER`,
3. `ACCEPT` or `ACCEPT-STRONG` requires at least three criteria at `EQ2` or
   higher,
4. `CONDITIONAL-ACCEPT` allows at most one criterion at `EQ1`.

## 8. Tie-Break Rules

When outcomes tie after normal scoring, apply these comparators in order:

1. higher adjusted `R1`,
2. higher adjusted `R2`,
3. higher count of `EQ3` criteria,
4. higher adjusted `R4`,
5. lower count of open remediation items.

If still tied, default is `DEFER` and escalation to charter tie-break authority.

## 9. Required Review Scorecard Fields

Every rubric record MUST include:

| Field | Requirement |
| --- | --- |
| `rubric_review_id` | Stable immutable identifier. |
| `extension_id` | Candidate extension ID under review. |
| `criteria_scores` | Raw scores for `R1` through `R4`. |
| `evidence_grades` | `EQ*` grade per criterion. |
| `computed_points` | Raw and adjusted points per criterion plus total score. |
| `hard_fail_flags` | Explicit pass or fail state for `HF-01` through `HF-05`. |
| `decision_outcome` | `ACCEPT-STRONG`, `ACCEPT`, `CONDITIONAL-ACCEPT`, `DEFER`, or `REJECT`. |
| `conditions` | Required only for conditional outcomes. |
| `review_owner` | Responsible review owner. |
| `review_date` | Absolute `YYYY-MM-DD`. |

## 10. Calibration Baselines

Reference calculations:

| Example ID | Outcome | Notes |
| --- | --- | --- |
| `CAL-A` | `ACCEPT` | `R1=5`, `R2=4`, `R3=4`, `R4=3`, all evidence `EQ2+`; adjusted score `81.6`. |
| `CAL-B` | `REJECT` | Safety floor miss (`R1=2`) and safety evidence `EQ1`; adjusted score is irrelevant to acceptance. |
| `CAL-C` | `CONDITIONAL-ACCEPT` | Floors pass with score `78.35`; owner and due date required for remediation items. |

## 11. Downstream Contract

| Consumer | Required output from this rubric |
| --- | --- |
| `C-05` lifecycle policy | Deterministic promotion thresholds and hard-fail gating inputs. |
| `C-09` test obligations | Evidence-quality requirements and fail-closed behavior for safety and portability claims. |
| `C-10` board operating model | Deterministic tie-break and defer behavior for tied dispositions. |

No downstream consumer may reinterpret `HF-*` or criterion floors as advisory.
