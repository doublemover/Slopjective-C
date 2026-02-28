# Frontend Library Boundary Contract Expectations (M140)

Contract ID: `objc3c-frontend-library-boundary-contract/m140-v1`

## Scope

M140 establishes extracted, reusable frontend library entry points and deterministic handoff boundaries between frontend, sema, lowering, and IR emission surfaces.

## Required Contract Surface

| Check ID | Requirement |
| --- | --- |
| `M140-LIB-01` | `libobjc3c_frontend/frontend_anchor.cpp` routes compile entrypoints through `CompileObjc3SourceWithPipeline(...)` and no longer uses scaffold-only compile behavior. |
| `M140-LIB-02` | `libobjc3c_frontend/objc3_cli_frontend.h/.cpp` expose `Objc3FrontendCompileProduct` for reusable pipeline + artifact handoff. |
| `M140-SEM-01` | `sema/objc3_sema_contract.h` defines deterministic semantic type-metadata handoff structures and exports deterministic handoff helpers. |
| `M140-SEM-02` | `sema/objc3_sema_pass_manager_contract.h` and `sema/objc3_sema_pass_manager.cpp` include pass-level diagnostics counters and deterministic metadata handoff fields. |
| `M140-LOW-01` | `lower/objc3_lowering_contract.h/.cpp` define `Objc3LoweringIRBoundary` and replay-key helpers used by IR emission. |
| `M140-IR-01` | `ir/objc3_ir_emitter.cpp` validates lowering IR boundary construction and enforces message-send arity against boundary arg-slot limits. |
| `M140-GATE-01` | `package.json` wires `test:objc3c:m140-boundary-contract` and `check:compiler-closeout:m140`; `check:task-hygiene` includes `check:compiler-closeout:m140`. |
| `M140-DOC-01` | Docs fragments include M140 semantics/tests/artifacts sections and reference `python scripts/check_m140_frontend_library_boundary_contract.py` + `npm run check:compiler-closeout:m140`. |

## Verification Commands

- `python scripts/check_m140_frontend_library_boundary_contract.py`
- `npm run test:objc3c:m140-boundary-contract`
- `npm run check:compiler-closeout:m140`

## Drift Remediation

1. Restore missing M140 boundary snippets in native source surfaces.
2. Restore docs and package script wiring for M140 closeout checks.
3. Re-run `python scripts/check_m140_frontend_library_boundary_contract.py`.
4. Re-run `npm run check:compiler-closeout:m140`.
