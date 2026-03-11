# M276-E001 Package Scripts, Operator Workflow, And Developer Runbook Migration For Incremental Native Builds Packet

Issue: `#7390`
Milestone: `M276`
Lane: `E`

## Objective

Make the incremental native build surface understandable and safe to use from package scripts, README guidance, native docs, and operator runbooks.

## Dependency handoff

- Depends on `M276-A001`, `M276-A002`, `M276-C001`, `M276-C002`, and `M276-D002`.
- Consumes the active-runner migration in `M276-D001` and the historical-compatibility contract in `M276-D003`.
- Hands off CI adoption and closeout proof to `M276-E002`.

## Implementation truths

- The public operator command surface is:
  - `npm run build:objc3c-native`
  - `npm run build:objc3c-native:contracts`
  - `npm run build:objc3c-native:full`
  - `npm run build:objc3c-native:reconfigure`
- The persistent build tree is `tmp/build-objc3c-native`.
- `compile_commands.json` and `native_build_backend_fingerprint.json` both live in that build tree.
- Reconfigure means forcing a fresh configure against the existing build tree; it does not require or suggest deletion.
- Active issue work prefers helper `fast`; the raw public commands remain operator-facing entrypoints.

## Proof model

- Statistically verify the package/docs/runbook surface is synchronized.
- Execute `npm run build:objc3c-native:reconfigure`.
- Verify the compile database and fingerprint file exist after reconfigure.
- Emit stable E001 evidence under `tmp/reports/m276/M276-E001/`.

## Exit condition

The public operator surface is truthful, documented, and sufficient to stand up and maintain the incremental native build tree without falling back to deletion workflows or monolithic full-rebuild guidance.
