# M318-B001 Planning Packet

Issue: `#7837`  
Title: `Governance and budget enforcement policy`

## Objective

Freeze the durable anti-noise governance policy that later automation and review flows
will enforce.

## Scope

- Consume the budget map from `M318-A001`.
- Consume the durable exception-process model from `M318-A002`.
- Publish one stable policy vocabulary for allowed growth, prohibited growth, and
  budget-family ownership.

## Policy summary

Allowed without exception:
- acceptance-suite input growth under `tests/tooling/runtime` and
  `tests/tooling/fixtures/native`;
- governance docs and runbooks under `spec/governance` and `docs/runbooks`;
- transient planning/publish artifacts under `tmp/`.

Prohibited without exception:
- growth beyond the public command budget;
- additive milestone-local checker/readiness/pytest wrapper families;
- new milestone-coded product identifiers in product code;
- synthetic `.ll` fixtures outside the fenced parity root;
- replay/proof artifacts that lack provenance and regeneration metadata.

## Ownership map

- public command budget: `M314-C003` existing owner, `M318-C002` live enforcement
- validation-growth budget: `M313-B005` existing owner, `M318-C002` live enforcement
- source-hygiene and residue budget: `M315-D002` existing owner, `M318-C002` live enforcement
- artifact-authenticity budget: `M315-C003`/`M315-C004` existing owners, `M318-D001` reporting and governance closeout

## Validation posture

Static verification is justified because this issue freezes the policy contract and
ownership map. The checker must validate the policy against the `M318-A001` budget map
and the `M318-A002` exception-process model.

Next issue: `M318-B002`.
