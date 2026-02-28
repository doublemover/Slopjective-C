# Conformance Coverage Map

This map ties required conformance families to issue lanes and buckets.
It is the source document for issue `#106` coverage-traceability requirements.

## Bucket Minima

Core minimums (Part 12 `§12.5.6`):

- parser: 15
- semantic: 25
- lowering_abi: 10
- module_roundtrip: 12
- diagnostics: 20

CI enforcement:

- workflow: `.github/workflows/conformance-minima.yml`
- checker: `scripts/check_conformance_suite.ps1`

## Family Mapping

| Family | Issue lane | Bucket(s) |
| --- | --- | --- |
| `TUV-01..05` | `#48/#49` language mode selection | `parser` |
| `CRPT-01..06` | `#106` conformance report coverage | `diagnostics` |
| `M02-D001..008` | `#1241..#1248` M02 lane D diagnostic mapping | `diagnostics` |
| `M03-D001..009` | `#1287..#1295` M03 lane D diagnostic mapping | `diagnostics` |
| `M04-D001..009` | `#1335..#1343` M04 lane D diagnostic mapping | `diagnostics` |
| `M05-D001..012` | `#1390..#1401` M05 lane D diagnostic mapping | `diagnostics` |
| `M06-D001..008` | `#1441..#1448` M06 lane D diagnostic mapping | `diagnostics` |
| `M07-D001..014` | `#1501..#1514` M07 lane D diagnostic mapping | `diagnostics` |
| `M08-D001..008` | `#1557..#1564` M08 lane D diagnostic mapping | `diagnostics` |
| `M145-D001` | `#4317` direct LLVM object-emission fail-closed matrix | `lowering_abi` |
| `M177-D001` | `#4477` namespace collision/shadowing lowering replay contract | `lowering_abi` |
| `M178-D001` | `#4482` public/private API partition lowering replay contract | `lowering_abi` |
| `M179-D001` | `#4487` incremental module cache/invalidation lowering replay contract | `lowering_abi` |
| `M180-D001` | `#4492` cross-module conformance lowering replay contract | `lowering_abi` |
| `SCM-01..06` | `#52/#44` strictness/sub-mode matrix | `diagnostics` |
| `EXE-01..05` | `#83` executor affinity | `semantic`, `lowering_abi`, `module_roundtrip` |
| `CAN-01..07` | `#82` cancellation propagation | `semantic`, `lowering_abi` |
| `ACT-01..09` | `#84` actor isolation | `semantic`, `lowering_abi`, `module_roundtrip` |
| `SND-01..08`, `SND-XM-01..02` | `#86` Sendable-like constraints | `semantic`, `diagnostics`, `module_roundtrip` |
| `BRW-NEG-01..05`, `BRW-POS-01..04` | `#91/#107` borrowed-pointer escape coverage | `semantic`, `diagnostics` |
| `PERF-DIRMEM-01..04` | `#96` legality checks | `semantic`, `module_roundtrip` |
| `PERF-DYN-01..04` | `#97` dynamic-dispatch misuse diagnostics | `diagnostics` |
| `INT-CXX-01..08` | `#102` ObjC++ interop coverage | `semantic`, `lowering_abi` |
| `INT-SWIFT-01..08` | `#103` Swift interop coverage | `semantic`, `module_roundtrip` |
| `DIAG-GRP-01..10` | `#104` minimum diagnostic groups | `diagnostics` |
| `MIG-01..08` | `#105` migrator/fix-it coverage | `diagnostics` |

## Strict-System Gate Set

Required strict-system family set for issue `#107`:

- `BRW-NEG-01..05`
- `BRW-POS-01..04`
- `AGR-NEG-01`
- `AGR-RT-01..03`
- `RES-01..06`
- `SYS-DIAG-01..08`

The CI checker fails when any required ID in this set is missing.
