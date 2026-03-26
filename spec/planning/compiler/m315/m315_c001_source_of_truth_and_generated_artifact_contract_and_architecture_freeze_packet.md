# M315-C001 Planning Packet

Issue: `#7801`  
Title: `Source-of-truth and generated-artifact contract`

## Objective

Freeze the replacement contract for the last milestone-coded source-of-truth residue
still present in native source and generated artifact schemas after `M315-B005`.

## Contract summary

This issue defines:
- the archival-versus-durable source-of-truth boundary;
- stable replacement field names for generated artifact references and readiness-gate
  identifiers;
- the rule that `next_issue` and other planning-only references must be removed from
  durable generated truth unless a real durable forward reference is required;
- the exact legacy inventories that downstream implementation issues must rewrite.

## Residual classes owned here

- `legacy_m248_surface_identifier`
- `dependency_issue_array`
- `next_issue_schema_field`
- `issue_key_schema_field`
- `transitional_source_model`

## Replacement rules

- `next_issue` is planning-only residue.
  - Durable generated surfaces must either omit it or replace it with
    `next_surface_id` using a stable `surface_id`.
- `*_issue` emitted keys become `*_surface_id`.
- `portability_dependency_issue_ids` becomes `portability_dependency_surface_ids`.
- `m248_*` readiness identifiers become `advanced_integration_*`.
- milestone-coded transitional source-model literals become stable durable
  `surface_id` values.

## Validation posture

Static verification is justified because this issue freezes the contract and complete
legacy inventory that later implementation work must consume. The checker must verify:
- the contract covers every residual class still owned by `M315-C001`;
- the frozen inventories match the currently observed legacy refs and literals;
- stable replacements do not reintroduce milestone, lane, or issue-era residue;
- durable replacement surface identifiers follow the `M315-B001` grammar.

Next issue: `M315-C002`.
