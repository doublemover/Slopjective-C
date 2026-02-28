# LLVM Capability Discovery Expectations (M144)

Contract ID: `objc3c-llvm-capability-discovery-contract/m144-v1`

## Scope

M144 hardens deterministic LLVM capability discovery and fail-closed backend routing for CLI/C API parity flows.

## Required Contract Surface

| Check ID | Requirement |
| --- | --- |
| `M144-CAP-01` | `scripts/probe_objc3c_llvm_capabilities.py` emits mode `objc3c-llvm-capabilities-v2`, reports `clang`/`llc` capability metadata, and fail-closes when sema/type-system parity capability is unavailable. |
| `M144-CAP-02` | Driver routing surface (`native/objc3c/src/driver/objc3_llvm_capability_routing.cpp` + `native/objc3c/src/driver/objc3_cli_options.cpp`) supports `--llvm-capabilities-summary` and `--objc3-route-backend-from-capabilities` with deterministic fail-closed diagnostics. |
| `M144-CAP-03` | `scripts/check_objc3c_library_cli_parity.py` consumes capability summaries and routes CLI object backend deterministically from capability probes when requested. |
| `M144-CAP-04` | `package.json` wires `check:objc3c:llvm-capabilities`, `check:objc3c:library-cli-parity:source:m144`, `test:objc3c:m144-llvm-capability-discovery`, and `check:compiler-closeout:m144`; `check:task-hygiene` includes `check:compiler-closeout:m144`. |
| `M144-CAP-05` | `docs/objc3c-native` source fragments (`10-cli`, `30-semantics`, `50-artifacts`, `60-tests`) and this contract doc describe capability summary mode, artifact outputs, and closeout commands. |

## Verification Commands

- `python scripts/check_m144_llvm_capability_discovery_contract.py`
- `npm run test:objc3c:m144-llvm-capability-discovery`
- `npm run check:compiler-closeout:m144`

## Drift Remediation

1. Restore missing M144 LLVM capability discovery snippets across source/docs/package surfaces.
2. Re-run `python scripts/check_m144_llvm_capability_discovery_contract.py`.
3. Re-run `npm run check:compiler-closeout:m144`.
