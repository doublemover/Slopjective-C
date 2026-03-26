# M315-C002 Planning Packet

Issue: `#7802`  
Title: `Artifact authenticity schema and evidence contract`

## Objective

Freeze a single authenticity-envelope schema so replay, fixture, report, and archive
artifacts can be labeled truthfully and later regenerated without ambiguity.

## Contract summary

This issue defines:
- the stable authenticity-class vocabulary replacing the broad `M315-A003` inventory
  labels where needed;
- the allowed provenance modes and proof-eligibility rules;
- the required envelope fields for JSON artifacts and `.ll` header transport;
- example envelopes that later implementation issues must satisfy.

## Key corrections

- `replay_candidate_missing_provenance` is not proof. It becomes
  `legacy_generated_replay_bridge` and remains bridge-only until `M315-C003`
  regenerates it with provenance.
- generated replay evidence is proof-eligible only when it carries both a durable
  generator surface id and a regeneration recipe.
- synthetic fixtures and examples are legitimate inputs, but they must never claim to
  be genuine generated replay evidence.

## Validation posture

Static verification is justified because this issue freezes the schema, class aliases,
proof rules, and example envelopes. The checker must verify:
- alignment with the `M315-A003`, `M315-B004`, and `M315-C001` baselines;
- completeness of the stable class vocabulary and alias map;
- proof-eligibility rules for generated replay versus bridge-only legacy replay;
- schema compliance of the example envelopes.

Next issue: `M315-C003`.
