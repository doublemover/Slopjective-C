# Hard-Cutover Behavior Phase Owner Contracts

This is the human-readable companion to
`tests/conformance/hard_cutover_behavior_phase_owner_contracts.json`. It is
local evidence only; no validation, GitHub edits, push, build, generator,
formatter, lint, npm, cmake, script, or test command was run while preparing it.

The contract is intentionally behavior-first. Parser, sema, lowering, IR,
runtime, and e2e owners each have one canonical support claim, a bounded native
fixture root, the required behavior families for that root, and explicit retired
surface evidence. Generated fixtures are recorded as provenance-only and cannot
satisfy a phase support claim.

| Phase | Owner | Support Claim | Boundary |
| --- | --- | --- | --- |
| Parser | `parser_lexer_ast` | `objc3c.behavior.parser.canonical-syntax` | canonical parser positives plus old-mode/removed-flag rejections |
| Sema | `semantic_diagnostics` | `objc3c.behavior.sema.typed-flow` | canonical type/control/objective behavior plus retired adapter and unsupported-feature diagnostics |
| Lowering | `lowering_and_ir` | `objc3c.behavior.lowering.strict-runtime-dispatch` | canonical nil-elision/lowering positives plus runtime-dispatch strict errors |
| IR | `lowering_and_ir` | `objc3c.behavior.ir.module-emission` | canonical module/function/metadata/runtime-call positives plus non-nil helper strict-error linkage |
| Runtime | `runtime_dispatch_registration` | `objc3c.behavior.runtime.strict-dispatch-error` | canonical object/storage/ARC/block/concurrency positives plus dispatch strict errors |
| E2E | `canonical_behavior_fixtures` | `objc3c.behavior.e2e.runnable-smoke` | runnable positives plus negative execution for legacy literals and runtime dispatch misses |

This keeps issue closeout and generated artifacts from widening support. A
retired surface is either represented by rejection/strict-error fixture
metadata or by an absent-support contract; it is never a positive fixture
semantic.
