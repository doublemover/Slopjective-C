# Validation

Validation links support claims to executable evidence. Unsupported public
claims fail docs validation.
Capability validation fails closed: missing evidence, unsupported alternate-name
claims, old-behavior escape paths, or success-without-evidence wording must stay
out of implemented support rows.
Generated reports, static summaries, and workflow owner files are evidence
inputs only; they do not complete a capability without a matching implemented
matrix row.

Validation prose must preserve the same ownership split as the capability
matrix:

- behavior rows need executable evidence or canonical diagnostic evidence,
- internal rows may name source, schema, or generated-doc owners without
  becoming language support,
- machine-readable capability truth lives in `docs/support/capability_matrix.json`
  against `schemas/objc3c-capability-matrix-v1.schema.json`,
- machine-readable evidence truth lives in `docs/support/evidence_map.json`
  against `schemas/objc3c-capability-evidence-map-v1.schema.json`,
- replayable commands must use `npm run objc3c -- <action>`,
- direct script, PowerShell, CMake, or native helper names are implementation
  evidence only,
- runtime dispatch claims must remain tied to the strict public C API result
  surface and the canonical `objc3_runtime_dispatch_i32` dispatch symbol.

Schema examples and anti-examples are maintained in
`docs/support/capability_schema_examples.md`.
