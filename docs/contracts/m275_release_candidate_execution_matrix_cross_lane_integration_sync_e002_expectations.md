# M275 Release-Candidate Execution Matrix - Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part12-release-candidate-execution-matrix/m275-e002-v1`

## Required outcomes

- The native CLI path publishes `module.objc3-release-candidate-matrix.json`.
- The frontend C API runner publishes the same closeout matrix sidecar with `surface_kind = "frontend-c-api"`.
- The matrix sidecar freezes the final Part 12 closeout dependency rows over the existing report, publication, gate, validation, release-evidence, and dashboard sidecars.
- The matrix remains bounded to emitted release evidence and does not invent a synthetic closeout authority outside the live sidecar family.

## Dynamic proof

- Run the native CLI with `--emit-objc3-conformance --emit-objc3-conformance-format json` against the positive fixture.
- Run the frontend C API runner on the same fixture with `--no-emit-ir --no-emit-object`.
- Run the native validation path on the emitted report.
- The native compile out-dir and frontend out-dir must both contain `module.objc3-release-candidate-matrix.json`.
- The native matrix sidecar must report:
  - `contract_id = "objc3c-part12-release-candidate-execution-matrix/m275-e002-v1"`
  - `surface_kind = "native-cli"`
  - `targeted_profile_ids = ["strict", "strict-concurrency", "strict-system"]`
  - `matrix_rows` length `5`
  - row contracts for `M275-A002`, `M275-B003`, `M275-C003`, `M275-D002`, and `M275-E001`
  - `ready = true`
- The frontend matrix sidecar must preserve the same contract and row set with `surface_kind = "frontend-c-api"`.
