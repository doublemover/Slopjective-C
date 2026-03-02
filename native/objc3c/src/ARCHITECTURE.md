# objc3c Native Frontend Architecture Contract

Status: Accepted (M132-A001)
Scope: `native/objc3c/src/*`

This document defines dependency rules for the modular native frontend.

## Layer Model

The frontend is split into five layers plus integration adapters:

1. `lex/*`
2. `parse/*`
3. `sema/*`
4. `lower/*`
5. `ir/*`

Adapter and integration modules:

- `pipeline/*`: stage orchestration, contracts, stage result passing.
- `libobjc3c_frontend/*`: public embedding API over `pipeline/*`.
- `driver/*`: CLI argument parsing and exit-code mapping only.
- `io/*`: filesystem/process adapters for manifests, artifacts, and tool calls.

Current-state note:

- `main.cpp` is now driver-only and delegates to `driver/objc3_driver_main.h`.
- Parser/sema boundaries are enforced through explicit contracts in
  `parse/objc3_parser_contract.h`, `parse/objc3_ast_builder_contract.h`, and
  `sema/objc3_sema_contract.h`.
- M226 architecture-freeze work builds on this extracted layout and hardens
  parser completeness and parser-to-sema handoff determinism.
- M227 extends the sema boundary with pass-order and symbol-flow freeze rules
  in `sema/objc3_sema_pass_manager_contract.h` (`Objc3SemaPassFlowSummary`).
- M227 lane-B type-system freeze anchors canonical ObjC form sets in
  `sema/objc3_sema_contract.h` (`kObjc3CanonicalReferenceTypeForms`,
  `kObjc3CanonicalScalarMessageSendTypeForms`,
  `kObjc3CanonicalBridgeTopReferenceTypeForms`) to keep semantic checking
  deterministic across `id`/`Class`/`SEL`/`Protocol`/`instancetype` and
  object-pointer forms.
- M250 lane-A frontend stability freeze anchors long-tail grammar closure to
  parser contract snapshots (`parse/objc3_parser_contract.h`) and parse/lowering
  readiness replay gates (`pipeline/objc3_parse_lowering_readiness_surface.h`)
  so parser determinism and recovery coverage remain fail-closed for GA
  readiness.
- M250 lane-A A003 core feature implementation anchors long-tail grammar
  closure identity (`long_tail_grammar_*`) in
  `parse/objc3_parser_contract.h` and wires fail-closed handoff enforcement in
  `pipeline/objc3_parse_lowering_readiness_surface.h`.
- M250 lane-A A004 core feature expansion anchors explicit long-tail grammar
  expansion accounting/replay gates (`long_tail_grammar_expansion_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so parse/lowering
  readiness can fail closed on expansion drift with deterministic replay keys.
- M250 lane-A A005 edge compatibility completion anchors long-tail grammar
  compatibility handoff and edge-case readiness
  (`long_tail_grammar_edge_case_compatibility_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so compatibility drift
  fails closed before lowering readiness is reported.
- M250 lane-A A006 edge-case expansion and robustness anchors explicit
  expansion/robustness guardrails (`long_tail_grammar_edge_case_*_robustness*`)
  in `pipeline/objc3_parse_lowering_readiness_surface.h` so edge-case expansion
  drift fails closed before conformance matrix readiness is reported.
- M250 lane-A A007 diagnostics hardening anchors explicit long-tail grammar
  diagnostics guardrails (`long_tail_grammar_diagnostics_hardening_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so diagnostics drift fails
  closed before recovery/determinism and conformance-readiness gates are
  reported.
- M250 lane-A A008 recovery/determinism hardening anchors explicit long-tail
  grammar replay-hardening gates (`long_tail_grammar_recovery_determinism_*`)
  in `pipeline/objc3_parse_lowering_readiness_surface.h` so replay drift fails
  closed before conformance and guardrail readiness are reported.
- M250 lane-A A009 conformance matrix implementation anchors explicit long-tail
  grammar matrix gates (`long_tail_grammar_conformance_matrix_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so matrix drift fails
  closed before corpus and guardrail readiness are reported.
- M250 lane-A A010 integration closeout and gate sign-off anchors explicit
  closeout/sign-off gates (`long_tail_grammar_integration_closeout_*`,
  `long_tail_grammar_gate_signoff_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-A cannot report
  ready-for-lowering before deterministic integration closeout is satisfied.
- M250 lane-B semantic stability freeze closes spec delta between
  `pipeline/objc3_typed_sema_to_lowering_contract_surface.h` and
  `pipeline/objc3_parse_lowering_readiness_surface.h` so semantic handoff
  determinism, conformance corpus closure, and performance guardrail gating are
  fail-closed under a single readiness boundary.
- M250 lane-B B002 modular split scaffolding anchors semantic-stability closure
  in `pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h` so
  typed sema handoff and parse/lowering readiness surfaces stay split while
  sharing a deterministic fail-closed scaffold key.
- M250 lane-B B003 core feature implementation anchors semantic stability
  readiness in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so typed semantic case accounting and parse conformance accounting remain
  deterministic and fail-closed behind the B002 scaffold.
- M250 lane-B B006 edge-case expansion and robustness anchors explicit
  semantic edge-case expansion/robustness guardrails
  (`edge_case_*_robustness*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so edge-case expansion drift fails closed before semantic stability
  readiness is reported.
- M250 lane-B B007 diagnostics hardening anchors explicit semantic diagnostics
  hardening guardrails (`diagnostics_hardening_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so diagnostics drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B008 recovery/determinism hardening anchors explicit semantic
  recovery/determinism guardrails (`recovery_determinism_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so replay drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B009 conformance matrix anchors explicit semantic
  conformance-matrix guardrails (`conformance_matrix_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so matrix drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B010 conformance corpus expansion anchors explicit semantic
  conformance-corpus guardrails (`conformance_corpus_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so corpus drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B011 performance and quality guardrails anchors explicit semantic
  performance/quality guardrails (`performance_quality_guardrails_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so guardrail drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B012 integration closeout and gate sign-off anchors explicit semantic
  closeout/sign-off guardrails (`integration_closeout_*`, `gate_signoff_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so semantic stability cannot report final readiness before deterministic
  integration closeout is satisfied.
- M250 lane-C C002 modular split scaffolding anchors lowering/runtime stability
  and invariant-proof closure in
  `pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h` so typed
  sema handoff and parse/lowering readiness surfaces share deterministic
  fail-closed runtime-proof gates.
- M250 lane-C C004 core feature expansion anchors explicit lowering/runtime
  expansion-accounting and replay-key guardrails in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so core feature readiness can fail closed on accounting drift.
- M250 lane-C C005 edge compatibility completion anchors parse/runtime
  compatibility handoff and edge-robustness gates in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so edge-case compatibility drift fails closed before runtime readiness.
- M250 lane-C C006 edge-case expansion and robustness anchors explicit
  lowering/runtime edge-case expansion and robustness guardrails
  (`edge_case_*_robustness*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so edge-case expansion drift fails closed before runtime readiness is
  reported.
- M250 lane-C C007 diagnostics hardening anchors explicit lowering/runtime
  diagnostics guardrails (`diagnostics_hardening_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so diagnostics drift fails closed before runtime readiness is reported.
- M250 lane-C C008 recovery/determinism hardening anchors explicit lowering/runtime
  recovery guardrails (`recovery_determinism_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so replay drift fails closed before runtime readiness is reported.
- M250 lane-C C009 conformance matrix anchors explicit lowering/runtime
  conformance-matrix guardrails (`conformance_matrix_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so conformance-matrix drift fails closed before runtime readiness is
  reported.
- M250 lane-C C010 conformance corpus expansion anchors explicit lowering/runtime
  conformance-corpus guardrails (`conformance_corpus_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so conformance-corpus drift fails closed before runtime readiness is
  reported.
- M250 lane-C C011 performance and quality guardrails anchors explicit
  lowering/runtime performance guardrails (`performance_quality_guardrails_*`)
  in `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so performance/quality drift fails closed before runtime readiness is
  reported.
- M250 lane-C C012 cross-lane integration sync anchors explicit lowering/runtime
  cross-lane synchronization guardrails (`cross_lane_integration_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so lane-C cannot report readiness before deterministic lane-A/lane-B/lane-C
  integration alignment is satisfied.
- M250 lane-C C013 integration closeout and gate sign-off anchors explicit
  lowering/runtime closeout/sign-off guardrails
  (`integration_closeout_*`, `gate_signoff_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so lane-C final readiness cannot report green before deterministic closeout
  evidence is satisfied.
- M250 lane-D D002 modular split scaffolding anchors toolchain/runtime GA
  operations readiness in
  `io/objc3_toolchain_runtime_ga_operations_scaffold.h` so backend routing and
  IR/object artifact compile gating stay deterministic and fail-closed before
  runtime object emission dispatch.
- M250 lane-D D003 core feature implementation anchors toolchain/runtime GA
  operations readiness closure in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so scaffold
  readiness, compile dispatch outcomes, and backend marker recording stay
  deterministic and fail-closed before success exit status is returned.
- M250 lane-D D004 core feature expansion anchors explicit backend marker-path
  and marker-payload guardrails in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` plus
  `pipeline/objc3_frontend_types.h` so toolchain/runtime GA readiness fails
  closed on marker determinism drift before success exit status is returned.
- M250 lane-D D005 edge-case compatibility completion anchors explicit
  edge-case compatibility guardrails (`edge_case_compatibility_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on backend route compatibility
  drift before success exit status is returned.
- M250 lane-D D006 edge-case expansion and robustness anchors explicit
  edge-case expansion/robustness guardrails (`edge_case_expansion_*`,
  `edge_case_robustness_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on backend output robustness
  drift before success exit status is returned.
- M250 lane-D D007 diagnostics hardening anchors explicit diagnostics
  hardening guardrails (`diagnostics_hardening_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on diagnostics drift before
  success exit status is returned.
- M250 lane-D D008 recovery/determinism hardening anchors explicit
  toolchain/runtime recovery guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D recovery and
  replay determinism drift fails closed before conformance and sign-off gates.
- M250 lane-D D009 conformance matrix implementation anchors explicit
  toolchain/runtime conformance-matrix guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D conformance
  matrix drift fails closed before corpus and sign-off gates.
- M250 lane-D D010 conformance corpus expansion anchors explicit
  toolchain/runtime conformance-corpus guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D conformance
  corpus drift fails closed before performance guardrails and sign-off gates.
- M250 lane-D D011 performance and quality guardrails anchor explicit
  toolchain/runtime performance/quality guardrail checks in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D guardrail
  drift fails closed before integration closeout and sign-off gates.
- M250 lane-D D012 cross-lane integration sync anchors explicit
  toolchain/runtime cross-lane integration guards in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D integration
  drift fails closed before integration closeout and gate sign-off.
- M250 lane-D D013 docs and operator runbook synchronization anchors explicit
  toolchain/runtime docs/runbook synchronization guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D docs-signoff
  drift fails closed before final gate sign-off.
- M250 lane-E E003 core feature implementation anchors explicit final readiness
  core-feature dependency guardrails in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E sign-off fails closed when E002/A003/B003/C003/D003 dependency
  readiness drifts.
- M250 lane-E E004 core feature expansion anchors explicit final readiness
  expansion guardrails (`core_feature_expansion_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when upstream lane expansion readiness drifts before
  final sign-off can remain green.
- M250 lane-E E005 edge-case compatibility completion anchors explicit final
  readiness edge-case compatibility guardrails
  (`edge_case_compatibility_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E004/A005/B005/C005/D005 compatibility readiness
  drifts before final sign-off can remain green.
## Ownership Map

- Lane A: `lex/*`, `parse/*`, `lower/*`, `ir/*`, `driver/*`, `io/*`
- Lane B: `pipeline/*`, `sema/*`, `libobjc3c_frontend/*`
- Lane C: generated docs/site tooling and policies (outside `src` tree)
- Lane D: parity/perf/determinism/fuzz gates (tests + scripts)
- Lane E: workflow/governance docs and CI policy wiring

## Dependency Rules

Rules apply to both include direction and link direction.

Allowed dependencies:

- `driver -> libobjc3c_frontend, io`
- `libobjc3c_frontend -> pipeline`
- `pipeline -> lex, parse, sema, lower, ir`
- `lower -> sema`
- `ir -> lower`
- `io -> (no frontend layer dependencies)`
- `lex -> (none)`
- `parse -> lex`
- `sema -> parse`

Forbidden dependencies:

- No stage module may depend on `driver/*`.
- No stage module may depend on `io/*`.
- `lex/*` may not depend on `parse/*`, `sema/*`, `lower/*`, `ir/*`.
- `parse/*` may not depend on `sema/*`, `lower/*`, `ir/*`.
- `sema/*` may not depend on `lower/*`, `ir/*`.
- `lower/*` may not depend on `ir/*`.
- `pipeline/*` may not depend on `driver/*` or `io/*`.
- `libobjc3c_frontend/*` may not depend on `driver/*` or `io/*`.

## Contract Checks (Automation Targets)

The future boundary-check script (`scripts/check_objc3c_dependency_boundaries.py`)
shall enforce:

- Include graph constraints for `#include "..."` within `native/objc3c/src`.
- CMake target link-direction constraints aligned with this document.
- A denylist for reverse dependencies violating stage order.

## Examples

Allowed:

- `parse/objc3_parser.cpp` including `lex/objc3_lexer.h`
- `pipeline/frontend_pipeline.cpp` including `sema/semantic_passes.h`
- `driver/main_driver.cpp` including `libobjc3c_frontend/api.h`

Forbidden:

- `sema/semantic_passes.cpp` including `lower/lower_to_ir.h`
- `ir/ir_emitter.cpp` including `driver/cli_options.h`
- `lex/objc3_lexer.cpp` including `parse/objc3_parser.h`

## Refactor Safety Invariants

- Behavior parity is enforced outside this file by Lane D gates.
- New modules must keep deterministic diagnostics ordering.
- `main.cpp` remains orchestration-only and must not absorb parser/sema logic.
- Parser recovery behavior must remain replay-proof and deterministic.
