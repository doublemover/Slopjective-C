# CMake Targetization and Linkage Topology Expectations (M141)

Contract ID: `objc3c-cmake-target-topology-contract/m141-v1`

## Scope

M141 hardens native build topology so stage modules, semantic/type-system surface, runtime ABI adapters, and executable entrypoints are wired through explicit CMake target boundaries.

## Required Contract Surface

| Check ID | Requirement |
| --- | --- |
| `M141-CMK-01` | `native/objc3c/CMakeLists.txt` defines stage-forward links (`objc3c_parse -> objc3c_lex`, `objc3c_sema -> objc3c_parse`, `objc3c_lower -> objc3c_sema_type_system`, `objc3c_ir -> objc3c_lower`). |
| `M141-CMK-02` | `objc3c_sema_type_system` INTERFACE target exists and carries sema/type-system linkage for downstream consumers. |
| `M141-CMK-03` | Runtime ABI process adapter is split via `objc3c_runtime_abi` and linked from `objc3c_io`. |
| `M141-CMK-04` | `objc3c-native` executable links through `objc3c_driver` aggregate target. |
| `M141-DRV-01` | `main.cpp` delegates to `driver/objc3_driver_main.h`; `driver/objc3_driver_main.cpp` owns parse+driver dispatch. |
| `M141-BLD-01` | `scripts/build_objc3c_native.ps1` includes `native/objc3c/src/driver/objc3_driver_main.cpp`. |
| `M141-GATE-01` | `package.json` wires `test:objc3c:m141-target-topology` and `check:compiler-closeout:m141`; `check:task-hygiene` includes `check:compiler-closeout:m141`. |
| `M141-DOC-01` | Docs fragments and this contract doc reference M141 commands and fail-closed checker. |

## Verification Commands

- `python scripts/check_m141_cmake_target_topology_contract.py`
- `npm run test:objc3c:m141-target-topology`
- `npm run check:compiler-closeout:m141`

## Drift Remediation

1. Restore missing CMake target/link snippets for M141 topology boundaries.
2. Restore driver entrypoint/build script and tooling test coverage snippets.
3. Restore docs/package/workflow M141 gate references.
4. Re-run `npm run check:compiler-closeout:m141`.
