# M275 CI/Runbook/Dashboard Contract - Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1`

## Required outcomes

- The existing conformance publication and validation sidecars carry one explicit advanced-feature operator reference surface.
- The surface stays bounded to operator references only:
  - CI release-evidence gate script path
  - release-evidence maintenance runbook path
  - dashboard schema path
  - targeted advanced profile ids
  - dependency contract ids from `M275-C002` and `M275-C003`
- The issue does not introduce a new report format, dashboard export, or promoted runnable advanced profile.
- The frontend C API runner and native CLI publication paths emit the same advanced-feature operator references.
- The native validation path echoes the same references after consuming the emitted report/publication pair.

## Dynamic proof

- Run the native CLI with `--emit-objc3-conformance --emit-objc3-conformance-format json` against the positive fixture.
- The emitted publication must report:
  - `advanced_feature_ops_contract_id = "objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1"`
  - `advanced_feature_reporting_contract_id = "objc3c-part12-feature-aware-conformance-report-emission/m275-c002-v1"`
  - `advanced_feature_release_evidence_contract_id = "objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1"`
  - `ci_release_evidence_gate_script_path = "scripts/check_release_evidence.py"`
  - `runbook_reference_path = "spec/conformance/release_evidence_gate_maintenance.md"`
  - `dashboard_schema_path = "schemas/objc3-conformance-dashboard-status-v1.schema.json"`
  - `advanced_feature_targeted_profile_ids = ["strict", "strict-concurrency", "strict-system"]`
- Run the native validation path on the emitted report; the validation artifact must echo the same fields.
- Run the frontend C API runner on the same fixture with `--no-emit-ir --no-emit-object`; the emitted publication must preserve the same advanced-feature operator references with `publication_surface_kind = "frontend-c-api"`.
