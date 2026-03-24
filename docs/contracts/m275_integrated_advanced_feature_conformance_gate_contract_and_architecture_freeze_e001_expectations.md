# M275 Integrated Advanced-Feature Conformance Gate - Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-part12-integrated-advanced-feature-gate/m275-e001-v1`

## Required outcomes

- The native CLI path publishes `module.objc3-advanced-feature-gate.json`.
- The frontend C API runner publishes the same gate sidecar with `surface_kind = "frontend-c-api"`.
- The gate sidecar freezes the integrated artifact family over report, publication, validation, release-evidence, and dashboard sidecars.
- The gate sidecar remains bounded to readiness/publication truth and does not bypass native validation.

## Dynamic proof

- Run the native CLI with `--emit-objc3-conformance --emit-objc3-conformance-format json` against the positive fixture.
- Run the frontend C API runner on the same fixture with `--no-emit-ir --no-emit-object`.
- Run the native validation path on the emitted report.
- The native compile out-dir and frontend out-dir must both contain `module.objc3-advanced-feature-gate.json`.
- The native gate sidecar must report:
  - `contract_id = "objc3c-part12-integrated-advanced-feature-gate/m275-e001-v1"`
  - `surface_kind = "native-cli"`
  - `validation_artifact_expected = "module.objc3-conformance-validation.json"`
  - `release_evidence_operation_artifact_expected = "module.objc3-release-evidence-operation.json"`
  - `dashboard_artifact_expected = "module.objc3-dashboard-status.json"`
- The frontend gate sidecar must preserve the same contract and expected artifact names with `surface_kind = "frontend-c-api"`.
