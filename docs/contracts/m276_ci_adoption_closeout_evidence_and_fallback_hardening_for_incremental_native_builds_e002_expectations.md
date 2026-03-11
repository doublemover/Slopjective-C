# M276 CI Adoption, Closeout Evidence, And Fallback Hardening For Incremental Native Builds Expectations (E002)

Contract ID: `objc3c-incremental-native-build-closeout/m276-e002-v1`

## Required outcomes

- CI must use the public incremental build command surface truthfully:
  - `build:objc3c-native` for active fast binary acquisition
  - `build:objc3c-native:contracts` where packet-only proof is sufficient
  - `build:objc3c-native:full` for manual closeout proof
- The docs must explain that local persistent-build semantics and CI ephemeral-workspace semantics are different.
- The fallback path for a missing or invalid persistent build tree must remain deterministic and non-destructive.
- The issue-local evidence must live under `tmp/reports/m276/M276-E002/`.

## Required implementation surface

- `.github/workflows/task-hygiene.yml`
- `.github/workflows/compiler-closeout.yml`
- `package.json`
- `README.md`
- `docs/objc3c-native/src/50-artifacts.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Closeout proof obligations

- cold build proof from a missing persistent build tree
- warm incremental rebuild proof from an already-configured persistent build tree
- invalid-fingerprint fallback proof
- independent contracts-only packet-generation proof
- full-build proof

## Implementation truths

- Cold-build proof may move the existing `tmp/build-objc3c-native` tree aside into `tmp/` evidence storage, but must not delete anything.
- Warm-build proof must show the binary path is incremental and does not rewrite native outputs when no work is required.
- Contracts-only proof must leave native binary mtimes unchanged while refreshing the source-derived plus binary-derived packet family.
- Full-build proof must refresh the complete packet family coherently.
