# M275 Toolchain Operations And Evidence Publication - Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1`

## Required outcomes

- The native validation path publishes `module.objc3-release-evidence-operation.json`.
- The native validation path publishes `module.objc3-dashboard-status.json`.
- Both sidecars are derived from the emitted report/publication/validation chain rather than from docs-only constants.
- The release-evidence operation sidecar carries the gate command, targeted advanced profiles, shard ids, and release-evidence artifact ids.
- The dashboard sidecar carries dashboard-ready profile status truth without promoting advanced profiles into runnable public claims.

## Dynamic proof

- Run the native CLI with `--emit-objc3-conformance --emit-objc3-conformance-format json` against the positive fixture.
- Run `--validate-objc3-conformance <report.json>` on the emitted report.
- The validation out-dir must contain:
  - `module.objc3-conformance-validation.json`
  - `module.objc3-release-evidence-operation.json`
  - `module.objc3-dashboard-status.json`
- The release-evidence operation sidecar must report:
  - `contract_id = "objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1"`
  - `command_tokens = ["python", "scripts/check_release_evidence.py"]`
  - `targeted_profile_ids = ["strict", "strict-concurrency", "strict-system"]`
  - `corpus_shard_ids = ["parser", "semantic", "lowering_abi", "module_roundtrip", "diagnostics"]`
- The dashboard sidecar must report:
  - `contract_id = "objc3c-part12-dashboard-status-publication/m275-d002-v1"`
  - `dashboard_schema_path = "schemas/objc3-conformance-dashboard-status-v1.schema.json"`
  - `core => pass`
  - `strict => blocked`
  - `strict-concurrency => blocked`
  - `strict-system => blocked`
