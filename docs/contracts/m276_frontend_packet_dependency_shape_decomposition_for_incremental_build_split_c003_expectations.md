# M276 Frontend Packet Dependency-Shape Decomposition For Incremental Build Split Expectations (C003)

Contract ID: `objc3c-frontend-packet-dependency-shape-decomposition/m276-c003-v1`

## Required outcomes

- `scripts/build_objc3c_native.ps1` must classify the frontend packet family into exactly these dependency-shape groups:
  - `source-derived`
  - `binary-derived`
  - `closeout-derived`
- The wrapper must expose internal execution modes that can regenerate those families without silently re-triggering the persistent native binary build.
- The packet family rooted at `tmp/artifacts/objc3c-native/` must remain deterministic and stable in path.
- `M276-C002` must consume this internal family map when it exposes the public command surface.

## Family map required by this contract

- `source-derived`
  - `frontend_modular_scaffold.json`
- `binary-derived`
  - `frontend_invocation_lock.json`
  - `frontend_core_feature_expansion.json`
- `closeout-derived`
  - `frontend_edge_compat.json`
  - `frontend_edge_robustness.json`
  - `frontend_diagnostics_hardening.json`
  - `frontend_recovery_determinism_hardening.json`
  - `frontend_conformance_matrix.json`
  - `frontend_conformance_corpus.json`
  - `frontend_integration_closeout.json`

## Proof obligations

- `binaries-only` must build/publish native binaries and skip packet generation.
- `packets-source` must skip native binary compilation and regenerate only the source-derived packet family.
- `packets-binary` must skip native binary compilation and regenerate the source-derived plus binary-derived packet family.
- `packets-closeout` must skip native binary compilation and regenerate all packet families needed to satisfy closeout dependencies.
- Packet-only modes must leave native binary mtimes unchanged.
- The issue-local checker must emit evidence under `tmp/reports/m276/M276-C003/`.

## Handoff

- `M276-C002` is the explicit next issue after this decomposition lands.
