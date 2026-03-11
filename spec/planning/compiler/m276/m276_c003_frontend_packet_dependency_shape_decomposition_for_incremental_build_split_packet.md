# M276-C003 Frontend Packet Dependency-Shape Decomposition For Incremental Build Split Packet

Issue: `#7393`
Milestone: `M276`
Lane: `C`

## Objective

Make the existing frontend packet family truthful by classifying it into dependency-shape groups and wiring the build wrapper so each packet group can regenerate independently of native binary compilation where allowed.

## Dependency handoff

- Depends on `M276-A001`, `M276-A002`, and `M276-C001`.
- Unblocks `M276-C002`, which will expose the public command surface on top of this decomposition.

## Implementation truths

- The native wrapper remains `scripts/build_objc3c_native.ps1`.
- Native binary compilation continues to flow through the persistent CMake/Ninja backend introduced by `M276-C001`.
- Packet-family decomposition is implemented inside the wrapper through internal execution modes.
- The authoritative packet family map is:
  - source-derived: scaffold
  - binary-derived: invocation lock, core feature expansion
  - closeout-derived: edge compat, edge robustness, diagnostics hardening, recovery determinism hardening, conformance matrix, conformance corpus, integration closeout

## Proof model

- Run `binaries-only` once to ensure native outputs exist.
- Backdate packet mtimes under `tmp/artifacts/objc3c-native/`.
- Run `packets-source` and prove only the source-derived packet is regenerated.
- Backdate packet mtimes again.
- Run `packets-binary` and prove only the source-derived plus binary-derived packets are regenerated.
- Backdate packet mtimes again.
- Run `packets-closeout` and prove the full closeout dependency graph is regenerated without a native rebuild.
- Prove binary mtimes do not change across packet-only modes.

## Required code/docs anchors

- `scripts/build_objc3c_native.ps1`
- `package.json`
- `README.md`
- `docs/objc3c-native.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Exit condition

The wrapper has a stable internal packet dependency-shape map, the issue-local checker proves each mode regenerates only its declared family scope, and `M276-C002` can safely expose the split as public commands.
