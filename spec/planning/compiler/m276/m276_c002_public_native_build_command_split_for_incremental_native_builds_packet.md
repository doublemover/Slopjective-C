# M276-C002 Public Native Build Command Split For Incremental Native Builds Packet

Issue: `#7388`
Milestone: `M276`
Lane: `C`

## Objective

Expose the internal `M276-C003` packet-family decomposition through the public npm command surface so the default build path becomes a truthful fast binary build while contracts/full remain available explicitly.

## Dependency handoff

- Depends on `M276-A001`, `M276-A002`, `M276-C001`, and `M276-C003`.
- Hands off to `M276-D001`, which will migrate active runners/checkers onto the fast-vs-full policy using a shared helper path.

## Public command mapping

- `npm run build:objc3c-native`
  - `scripts/build_objc3c_native.ps1 -ExecutionMode binaries-only`
- `npm run build:objc3c-native:contracts`
  - `scripts/build_objc3c_native.ps1 -ExecutionMode packets-binary`
- `npm run build:objc3c-native:full`
  - `scripts/build_objc3c_native.ps1 -ExecutionMode full`

## Truthful boundary

- Direct script callers still default to `full` until the runner/helper migration tranche closes.
- The public command split must not silently claim packet generation on the fast default path.
- Historical frontend build-invocation contracts that mention a command path for packet regeneration must be updated to point at the new truthful public command.

## Proof model

- Backdate packet mtimes.
- Run `npm run build:objc3c-native` and prove packet mtimes do not change.
- Run `npm run build:objc3c-native:contracts` and prove only source-derived plus binary-derived packets change while binary mtimes remain stable.
- Run `npm run build:objc3c-native:full` and prove the wrapper reports native build plus full packet generation coherently.

## Exit condition

The public npm command surface is truthful, packet regeneration is no longer implied by the fast default path, and historical build-invocation docs/contracts point at the correct command for the packet family they describe.
