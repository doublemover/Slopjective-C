# M315-A003 Planning Packet

Issue: `#7795`  
Title: `Artifact authenticity and provenance classification inventory`

## Objective

Freeze a truthful inventory for the tracked artifact-authenticity slice so later
`M315` work can replace milestone-era residue and fake-looking proof surfaces with
stable identifiers, explicit provenance rules, and reproducible regeneration paths.

## Scope

- Include tracked `.ll` files.
- Include authenticity-sensitive tracked `.json` files.
- Exclude configuration-only JSON surfaces:
  - `.prettierrc.json`
  - `package-lock.json`
  - `package.json`

## Frozen baseline

- Artifact candidates: `2514`
- `.ll` files: `78`
- `.json` files: `2436`
- Authenticity classes:
  - `schema_policy_contract`: `66`
  - `generated_or_report_artifact`: `25`
  - `sample_or_example_artifact`: `62`
  - `synthetic_fixture`: `2209`
  - `replay_candidate_missing_provenance`: `152`
- Replay candidates split:
  - `.ll`: `76`
  - `.json`: `76`
  - replay `.ll` with frontend header: `30`
  - replay `.ll` without frontend header: `46`

## Why this issue exists

`M315-A001` froze the repo-wide residue baseline and `M315-A002` narrowed the native
source slice. This issue freezes the artifact-authenticity inventory so later `M315`
policy and implementation work cannot hide behind vague claims about “fixtures” or
“replay proof” without naming what is synthetic, what is sample/example-only, and
what still lacks embedded provenance.

## Downstream ownership

- `M315-B001`: stable feature-surface identifier and annotation policy
- `M315-B002`: product-code annotation and provenance policy
- `M315-B004`: synthetic-versus-genuine IR fixture compatibility semantics
- `M315-C001`: source-of-truth and generated-artifact contract
- `M315-C002`: artifact authenticity schema and evidence contract
- `M315-C003`: genuine replay artifact regeneration and provenance capture
- `M315-C004`: synthetic fixture relocation labeling and parity-test updates

## Validation posture

Static verification is justified here because the issue is an inventory freeze. The
checker must recompute the candidate set and verify the class counts, provenance-signal
counts, and flagged example paths from the tracked repository state.

Next issue: `M315-B001`.
