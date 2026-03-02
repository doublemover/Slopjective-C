# Parse-Lowering Readiness Expectations (M226-C001)

Contract ID: `objc3c-parse-lowering-readiness-contract/m226-c001-v1`

## Scope

M226-C001 freezes parse-to-lowering readiness surfaces so parser contracts, frontend pipeline handoff wiring, artifact metadata projection, and lowering boundary validation remain deterministic and fail-closed.

## Scope Anchors

- `native/objc3c/src/parse/*`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`

## Deterministic Invariants

| ID | Requirement |
| --- | --- |
| `M226-C001-INV-01` | Parse contract surfaces remain wrapper-based (`Objc3ParsedProgram`, parser snapshot, diagnostics transport helpers) and cannot drift from the anchored `native/objc3c/src/parse/*` file set. |
| `M226-C001-INV-02` | `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves canonical handoff ordering through `BuildObjc3AstFromTokens(...)`, parser snapshot propagation, sema pass execution, symbol/scope summary derivation, and diagnostics transport to parsed program state. |
| `M226-C001-INV-03` | `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` continues deriving lowering-facing readiness contracts (message-send selector lowering, dispatch ABI marshalling, runtime shim host link) and emits IR from `pipeline_result.program.ast` with deterministic handoff metadata wiring. |
| `M226-C001-INV-04` | `native/objc3c/src/lower/objc3_lowering_contract.cpp` preserves fail-closed normalization of lowering contract inputs, deterministic IR boundary/replay-key computation, and validation/replay-key helpers for lowering contract lanes consumed by frontend artifact generation. |
| `M226-C001-INV-05` | The fail-closed checker and tooling tests remain the canonical freeze gate for this packet and write JSON summaries under `tmp/reports/m226`. |

## Acceptance Checks

- `python scripts/check_m226_c001_parse_lowering_readiness_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c001_parse_lowering_readiness_contract.py -q`

The checker writes deterministic JSON output to `tmp/reports/m226/m226_c001_parse_lowering_readiness_contract_summary.json` by default and exits non-zero on drift.
