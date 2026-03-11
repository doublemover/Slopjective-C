# M276 Public Native Build Command Split For Incremental Native Builds Expectations (C002)

Contract ID: `objc3c-public-native-build-command-split/m276-c002-v1`

## Required outcomes

- The public npm build surface must expose three truthful command paths:
  - `npm run build:objc3c-native`
  - `npm run build:objc3c-native:contracts`
  - `npm run build:objc3c-native:full`
- `npm run build:objc3c-native` must be the fast binary-build default and must not regenerate frontend packets.
- `npm run build:objc3c-native:contracts` must regenerate the source-derived plus binary-derived packet family without silently invoking native binary compilation.
- `npm run build:objc3c-native:full` must still produce native binaries plus the full packet family coherently.
- Direct script callers may remain on the full wrapper path until the runner/helper migration tranche lands.
- `M276-D001` is the next issue after this public command split lands.

## Packet-family expectations

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

- `build:objc3c-native` leaves packet mtimes untouched.
- `build:objc3c-native:contracts` updates only the source-derived plus binary-derived packet family and leaves binary mtimes unchanged.
- `build:objc3c-native:full` runs the native build path and regenerates the entire packet family.
- Issue-local evidence must be emitted under `tmp/reports/m276/M276-C002/`.
