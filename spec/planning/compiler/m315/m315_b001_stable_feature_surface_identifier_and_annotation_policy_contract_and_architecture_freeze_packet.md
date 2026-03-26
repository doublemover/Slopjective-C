# M315-B001 Planning Packet

Issue: `#7796`  
Title: `Stable feature-surface identifier and annotation policy`

## Objective

Freeze a single durable identifier scheme so later `M315` cleanup work replaces
milestone-coded residue with stable names instead of inventing another generation of
ad hoc labels.

## Policy summary

Durable product and generated source-of-truth identifiers must use:
- `objc3c.<domain>.<subsystem>.<surface>.vN`

Durable identifiers must not embed:
- milestone ids such as `m315` or `m228`
- lane tags such as `lane-a` or `lane-d`
- issue-local tokens such as `a001`, `d002`, or `issue7796`

## Allowed annotation families

- `surface_id`
- `artifact_family_id`
- `fixture_family_id`
- `report_family_id`
- `historical_ref` (archival-only)

## Archival boundary

Historical milestone references remain allowed only in planning and contract-history
surfaces such as:
- `spec/planning/**`
- `docs/contracts/**`

They are prohibited for durable identifiers in:
- `native/objc3c/src/**`
- generated manifests and reports that claim product/runtime truth
- future source-of-truth registries introduced by `M315-C001` and `M315-C002`

## Validation posture

Static verification is justified because this issue freezes the durable identifier
contract and example set. The checker must verify the grammar, example identifiers,
prohibited token patterns, and downstream ownership map.

Next issue: `M315-B002`.
