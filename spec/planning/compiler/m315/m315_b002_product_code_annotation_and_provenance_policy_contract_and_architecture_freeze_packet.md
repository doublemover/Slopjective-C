# M315-B002 Planning Packet

Issue: `#7797`  
Title: `Product-code annotation and provenance policy`

## Objective

Freeze the placement and meaning of stable annotations so later decontamination work
can strip issue-era residue from product code without also stripping the minimal
durable metadata that the runtime, reports, and authenticity work still need.

## Policy summary

Allowed product/generated annotation families:
- `surface_id`
- `artifact_family_id`
- `provenance_class`
- `provenance_mode`

Archival-only family:
- `historical_ref`

Prohibited in product and generated truth surfaces:
- milestone ids
- lane tags
- issue ids
- planning narrative comments that double as product annotations

## Provenance classes

- `synthetic_fixture`
- `sample_or_example`
- `generated_report`
- `generated_replay`
- `schema_policy_contract`
- `historical_archive`

## Validation posture

Static verification is justified because this issue freezes the allowed annotation
families, placement rules, and provenance-class vocabulary that later `M315` issues
must implement.

Next issue: `M315-B003`.
