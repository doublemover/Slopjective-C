# M258 Imported Metadata Conformance, Effect, And Dispatch Preservation Rules Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1`
Issue: `#7161`

## Required outcomes

1. The frontend accepts repeated `--objc3-import-runtime-surface <path>` inputs as a real compiler capability.
2. Imported runtime-import-surface artifacts are loaded and validated before IR/object emission.
3. Imported conformance shape, dispatch traits, and effect traits are preserved as deterministic semantic facts in:
   - `frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules`
4. Duplicate imported surface paths fail closed.
5. Duplicate imported module names fail closed.
6. Malformed or contract-invalid import-surface payloads fail closed.
7. Code/spec/package anchors remain explicit and deterministic.
8. Validation evidence lands at:
   - `tmp/reports/m258/M258-B002/imported_runtime_metadata_semantic_rules_summary.json`

## Proof obligations

- Happy path compiles with two emitted `module.runtime-import-surface.json` inputs and publishes the imported semantic-rule surface.
- The imported semantic-rule surface reports:
  - imported module names
  - imported input-path count
  - runtime-owned declaration counts
  - conformance shape counts
  - dispatch-trait counts
  - effect-trait counts
- The emitted import-surface artifact now includes the property and ivar fields needed to preserve imported effect and binding traits.
- IR remains explicit that imported runtime metadata payloads are not lowered into IR in this lane.
- Public embedding remains explicit that imported metadata stays filesystem-artifact based and does not expose a live imported-module semantic ABI.
