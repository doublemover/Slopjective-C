# Sema Pass Manager and Diagnostics Bus Contract Expectations (M139)

Contract ID: `objc3c-sema-pass-manager-diagnostics-bus-contract/m139-v1`

## Scope

This contract defines fail-closed integration requirements for sema pass-manager extraction and deterministic diagnostics-bus transport.

## Deterministic requirements

| ID | Requirement |
| --- | --- |
| `M139-SEM-01` | `sema/objc3_sema_contract.h` + `sema/objc3_sema_pass_manager_contract.h` remain the canonical sema pass-manager contract packet (`Objc3SemaPassId`, `kObjc3SemaPassOrder`, diagnostics bus payload types). |
| `M139-SEM-02` | `sema/objc3_sema_pass_manager.h` + `sema/objc3_sema_pass_manager.cpp` remain the canonical sema orchestration boundary and publish pass diagnostics through `Objc3SemaDiagnosticsBus`. |
| `M139-SEM-03` | `sema/objc3_semantic_passes.cpp` and `sema/objc3_pure_contract.cpp` preserve split ownership: integration/body validation in semantic passes, pure-contract validation in pure-contract module. |
| `M139-SEM-04` | `parse/objc3_diagnostics_bus.h`, `pipeline/objc3_frontend_types.h`, and `pipeline/objc3_frontend_pipeline.cpp` preserve deterministic diagnostics-bus transport (`lexer` -> `parser` -> `semantic`) and deterministic aggregation into parsed-program diagnostics. |
| `M139-SEM-05` | Build wiring remains deterministic in both `native/objc3c/CMakeLists.txt` and `scripts/build_objc3c_native.ps1` for sema extraction sources (`objc3_sema_diagnostics_bus.cpp`, `objc3_sema_pass_manager.cpp`, `objc3_semantic_passes.cpp`, `objc3_static_analysis.cpp`, `objc3_pure_contract.cpp`). |
| `M139-SEM-06` | CI/release gates fail closed on sema pass-manager + diagnostics-bus drift via `python scripts/check_m139_sema_pass_manager_contract.py` and `npm run check:compiler-closeout:m139`. |

## Validation commands

- `python scripts/check_m139_sema_pass_manager_contract.py`
- `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
- `npm run check:compiler-closeout:m139`

## Operator checklist

1. Run `python scripts/check_m139_sema_pass_manager_contract.py`.
2. Run `npm run test:objc3c:sema-pass-manager-diagnostics-bus`.
3. Run `npm run check:compiler-closeout:m139`.
4. If any check fails, treat it as release-blocking drift and restore sema pass-manager + diagnostics-bus contract wiring before promotion.
