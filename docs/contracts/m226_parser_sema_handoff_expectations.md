# Parser-to-Sema Handoff Architecture Freeze Expectations (M226-B001)

Contract ID: `objc3c-parser-sema-handoff-contract/m226-b001-v1`

## Scope

This contract freezes the parser-to-sema handoff architecture for Lane B (`M226-B001`) and enforces fail-closed drift detection over canonical handoff surfaces.

## Spec References

- `docs/contracts/parser_ast_contract_expectations.md` (`M138`)
- `docs/contracts/sema_pass_manager_diagnostics_bus_expectations.md` (`M139`)
- `docs/objc3c-native/src/30-semantics.md` section `Sema pass manager + diagnostics bus contract (M139-E001)`

## Deterministic Invariants

| ID                    | Requirement |
| --------------------- | ----------- |
| `M226-B001-INV-01`    | `native/objc3c/src/parse/objc3_parser_contract.h` remains the canonical wrapper boundary (`Objc3ParsedProgram`) between parser output and downstream sema consumption. |
| `M226-B001-INV-02`    | `native/objc3c/src/parse/objc3_parser.h` and `native/objc3c/src/parse/objc3_ast_builder_contract.{h,cpp}` preserve parser result structure (`Objc3ParseResult`) and builder translation (`BuildObjc3AstFromTokens(...)`) without bypassing the contract wrapper. |
| `M226-B001-INV-03`    | `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h` preserves parser-to-sema input shape (`const Objc3ParsedProgram *program`) and fixed pass order (`BuildIntegrationSurface`, `ValidateBodies`, `ValidatePureContract`). |
| `M226-B001-INV-04`    | `native/objc3c/src/sema/objc3_sema_pass_manager.{h,cpp}` preserves deterministic orchestration through `RunObjc3SemaPassManager(...)`, publishes pass diagnostics via the diagnostics bus, and records per-pass counters in stable pass order. |
| `M226-B001-INV-05`    | `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves canonical handoff wiring from parser output to sema input (`sema_input.program = &result.program`) with deterministic diagnostics transport wiring (`sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic`). |
| `M226-B001-INV-06`    | `native/objc3c/src/parse/objc3_diagnostics_bus.h` preserves deterministic diagnostics aggregation order (`lexer`, `parser`, `semantic`) via `TransportObjc3DiagnosticsToParsedProgram(...)`. |
| `M226-B001-INV-07`    | Pipeline and sema pass-manager boundaries remain fail-closed against parser-internal bypasses: parser entry in pipeline is `BuildObjc3AstFromTokens(...)` rather than direct `ParseObjc3Program(...)` or parser implementation includes. |
| `M226-B001-INV-08`    | The freeze remains discoverable via this contract plus M138/M139 references so future packet work can validate parser->sema architecture intent from stable docs. |

## Acceptance Checks

- `python scripts/check_m226_b001_parser_sema_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b001_parser_sema_handoff_contract.py -q`

The checker writes deterministic JSON output to `tmp/reports/m226_b001_parser_sema_handoff_contract_summary.json` by default and exits non-zero on drift.

