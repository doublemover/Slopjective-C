# Objective-C 3.0 Next Milestone Issue Drafts

This is the reviewed and tightened durable issue backlog for the next Objective-C 3.0 work. It replaces lane-style decomposition with concrete implementation issues.

Review improvements folded in:
- Issue bodies now include concrete implementation scope, non-goals, source references, function/entrypoint hints, validation commands, and closure criteria.
- Sub-issues are implementation checklists in the main issue body, not separate GitHub issues. This avoids creating 1,000 extra issues while keeping each main issue well under the 100 sub-issue cap.
- Labels are normalized to labels that already exist in `doublemover/Slopjective-C`.
- Dependencies are represented by durable draft IDs here and should be created as GitHub `blocked_by` relationships after GitHub assigns issue numbers.
- No source-of-truth data should live under `tmp/`.

Counts:
- Milestone drafts: 40
- Main issue drafts: 100
- Sub-issues per main issue: 10
- Total checklist sub-issues: 1000
- Native dependency relationships to publish: 98

## Published GitHub Mapping

- Repository: `doublemover/Slopjective-C`
- Published at: `2026-04-13T17:24:49.821Z`
- GitHub milestones: 40 (`414` through `453`)
- GitHub issues: 100 (#8007 through #8106)
- GitHub `blocked_by` relationships: 98
- Publication failures: 0
- Full durable mapping: `reports/planning/objc3c_3_next_40_publication_report.md` and `reports/planning/objc3c_3_next_40_publication_report.json`.

## Milestones

### OC3-T01: OC3-T01 Claimable Runtime Surface Audit
- Create order: 1
- Objective: Define the public support/claim contract and make implementation claims fail closed until backed by runnable evidence.
- Main issues: OC3-T01-001, OC3-T01-002, OC3-T01-003

### OC3-T02: OC3-T02 Parser And AST Completeness For Draft Syntax
- Create order: 2
- Objective: Implement parser and AST coverage for the Objective-C 3.0 draft syntax surface.
- Main issues: OC3-T02-001, OC3-T02-002, OC3-T02-003

### OC3-T03: OC3-T03 Typed Semantic Model Closure
- Create order: 3
- Objective: Close the typed semantic model for objects, effects, ownership, generics, modules, and diagnostics.
- Main issues: OC3-T03-001, OC3-T03-002, OC3-T03-003

### OC3-T04: OC3-T04 Lowering And IR Emission Completeness
- Create order: 4
- Objective: Lower supported language/runtime constructs into executable IR and emitted metadata.
- Main issues: OC3-T04-001, OC3-T04-002, OC3-T04-003

### OC3-T05: OC3-T05 Full Class Metaclass Runtime Realization
- Create order: 5
- Objective: Realize live class/metaclass identity, allocation, registration, and replay behavior.
- Main issues: OC3-T05-001, OC3-T05-002, OC3-T05-003

### OC3-T06: OC3-T06 Protocol And Category Runtime Semantics
- Create order: 6
- Objective: Implement runtime protocol conformance and category attachment semantics.
- Main issues: OC3-T06-001, OC3-T06-002, OC3-T06-003

### OC3-T07: OC3-T07 Property Ivar Reflection Closure
- Create order: 7
- Objective: Close executable property, ivar, accessor, and reflection behavior.
- Main issues: OC3-T07-001, OC3-T07-002, OC3-T07-003

### OC3-T08: OC3-T08 Message Dispatch And Method Cache Completeness
- Create order: 8
- Objective: Complete message lookup, dispatch ABI, method cache invalidation, and fallback removal.
- Main issues: OC3-T08-001, OC3-T08-002, OC3-T08-003

### OC3-T09: OC3-T09 Escaping Blocks And Byref Runtime Automation
- Create order: 9
- Objective: Implement escaping block descriptors, byref forwarding, copy/dispose, and capture ownership.
- Main issues: OC3-T09-001, OC3-T09-002, OC3-T09-003

### OC3-T10: OC3-T10 ARC Automation Beyond Helper-Backed Slice
- Create order: 10
- Objective: Complete ARC insertion and runtime-backed ownership behavior beyond helper-only demonstrations.
- Main issues: OC3-T10-001, OC3-T10-002, OC3-T10-003

### OC3-T11: OC3-T11 Throws Error Propagation And Cleanup Runtime
- Create order: 11
- Objective: Implement runtime-backed throws, cleanup/unwind, and bridged error propagation.
- Main issues: OC3-T11-001, OC3-T11-002, OC3-T11-003

### OC3-T12: OC3-T12 Async Await And Continuation Runtime
- Create order: 12
- Objective: Implement async/await lowering, continuations, suspension, resume, and cancellation integration.
- Main issues: OC3-T12-001, OC3-T12-002, OC3-T12-003

### OC3-T13: OC3-T13 Task Runtime And Structured Concurrency
- Create order: 13
- Objective: Implement task spawning, groups, cancellation, scheduler, and structured concurrency runtime semantics.
- Main issues: OC3-T13-001, OC3-T13-002, OC3-T13-003

### OC3-T14: OC3-T14 Actor Runtime And Isolation Enforcement
- Create order: 14
- Objective: Implement actor runtime identity, mailbox/executor binding, isolation, sendability, and reentrancy checks.
- Main issues: OC3-T14-001, OC3-T14-002, OC3-T14-003

### OC3-T15: OC3-T15 Metaprogramming Macro Host And Property Behaviors
- Create order: 15
- Objective: Implement macro host execution, provenance, caching, sandboxing, and property behavior hooks.
- Main issues: OC3-T15-001, OC3-T15-002, OC3-T15-003

### OC3-T16: OC3-T16 Broader C Cpp Swift Interop And Package Loader
- Create order: 16
- Objective: Implement broader C, ObjC, ObjC++, Swift-facing, and mixed-image package interop.
- Main issues: OC3-T16-001, OC3-T16-002, OC3-T16-003

### OC3-T17: OC3-T17 Standard Library Runtime Backing
- Create order: 17
- Objective: Back stdlib APIs with runtime behavior and executable package/conformance coverage.
- Main issues: OC3-T17-001, OC3-T17-002, OC3-T17-003

### OC3-T18: OC3-T18 Conformance Corpus From Breadth To Claimability
- Create order: 18
- Objective: Convert support claims into executable conformance evidence and longitudinal regression coverage.
- Main issues: OC3-T18-001, OC3-T18-002, OC3-T18-003

### OC3-T19: OC3-T19 Production Performance Stress And Runtime Hardening
- Create order: 19
- Objective: Add runtime/compiler performance, stress, fuzz, and stability gates for production claims.
- Main issues: OC3-T19-001, OC3-T19-002, OC3-T19-003

### OC3-T20: OC3-T20 Release Grade Public ABI Packaging Security And Governance
- Create order: 20
- Objective: Close public ABI, packaging, attestation, security, and governance requirements for release-grade claims.
- Main issues: OC3-T20-001, OC3-T20-002, OC3-T20-003

### OC3-N21: OC3-N21 Deterministic Bootstrap And Build Reproducibility
- Create order: 21
- Objective: Make bootstrap and build artifacts reproducible without environment- or tmp-derived state.
- Main issues: OC3-N21-001, OC3-N21-002

### OC3-N22: OC3-N22 Developer Tooling LSP Navigation And Workspace Semantics
- Create order: 22
- Objective: Implement editor navigation, indexing, and workspace semantic tooling.
- Main issues: OC3-N22-001, OC3-N22-002

### OC3-N23: OC3-N23 Formatter Refactor And Source Rewrite Tools
- Create order: 23
- Objective: Complete formatter, refactor, and source rewrite tooling for Objective-C 3.0 surfaces.
- Main issues: OC3-N23-001, OC3-N23-002

### OC3-N24: OC3-N24 Debugger Source Maps And Runtime Inspection
- Create order: 24
- Objective: Expose debugger source maps and runtime inspection across object/concurrency surfaces.
- Main issues: OC3-N24-001, OC3-N24-002

### OC3-N25: OC3-N25 Diagnostic Taxonomy And Fix-It Quality
- Create order: 25
- Objective: Make diagnostics structured, stable, actionable, and fix-it backed.
- Main issues: OC3-N25-001, OC3-N25-002

### OC3-N26: OC3-N26 ObjC2 Swift Cpp Migration Tooling
- Create order: 26
- Objective: Build migration analysis and rewrite workflows for ObjC2, Swift, and C++ users.
- Main issues: OC3-N26-001, OC3-N26-002

### OC3-N27: OC3-N27 Package Manager Registry And Offline Mirror
- Create order: 27
- Objective: Complete package resolver, registry, lockfile, and offline mirror behavior.
- Main issues: OC3-N27-001, OC3-N27-002

### OC3-N28: OC3-N28 Incremental Compilation Module Cache And Invalidations
- Create order: 28
- Objective: Implement module cache invalidation and incremental compilation proof for runtime metadata.
- Main issues: OC3-N28-001, OC3-N28-002

### OC3-N29: OC3-N29 Cross Platform Toolchain Matrix
- Create order: 29
- Objective: Expand supported platform matrix and packaged-runtime acceptance coverage.
- Main issues: OC3-N29-001, OC3-N29-002

### OC3-N30: OC3-N30 Installer Update And User-Facing Release UX
- Create order: 30
- Objective: Implement signed installers, update channels, rollback, and release diagnostics.
- Main issues: OC3-N30-001, OC3-N30-002

### OC3-N31: OC3-N31 Observability Compile Tracing And Runtime Telemetry
- Create order: 31
- Objective: Expose compile-stage tracing and opt-in runtime telemetry with privacy controls.
- Main issues: OC3-N31-001, OC3-N31-002

### OC3-N32: OC3-N32 Fuzzing Crash Minimization And Differential Testing
- Create order: 32
- Objective: Expand fuzzing, crash minimization, and differential testing into durable gates.
- Main issues: OC3-N32-001, OC3-N32-002

### OC3-N33: OC3-N33 Codegen Optimization And Direct Dispatch Policy
- Create order: 33
- Objective: Implement optimization passes and direct-dispatch policy without semantic regressions.
- Main issues: OC3-N33-001, OC3-N33-002

### OC3-N34: OC3-N34 Memory Safety UB And Runtime Sanitizer Audits
- Create order: 34
- Objective: Add sanitizer-backed memory safety and UB audits, then fix findings behind blockers.
- Main issues: OC3-N34-001, OC3-N34-002

### OC3-N35: OC3-N35 Docs Site And Claim Synchronization
- Create order: 35
- Objective: Generate docs from durable support evidence and reject unsupported public claims.
- Main issues: OC3-N35-001, OC3-N35-002

### OC3-N36: OC3-N36 Showcase And Canonical Application Expansion
- Create order: 36
- Objective: Grow canonical apps and showcases that exercise real runtime-backed features.
- Main issues: OC3-N36-001, OC3-N36-002

### OC3-N37: OC3-N37 API ABI Compatibility Governance
- Create order: 37
- Objective: Govern API/ABI diffs, compatibility windows, and deprecation policy with release blockers.
- Main issues: OC3-N37-001, OC3-N37-002

### OC3-N38: OC3-N38 External Validation Partners And Repro Corpus
- Create order: 38
- Objective: Build external reproducibility corpus and validation gates for claim credibility.
- Main issues: OC3-N38-001, OC3-N38-002

### OC3-N39: OC3-N39 Security Threat Model And Macro Supply Chain
- Create order: 39
- Objective: Close security threat model, macro supply-chain policy, sandboxing, signing, and revocation.
- Main issues: OC3-N39-001, OC3-N39-002

### OC3-N40: OC3-N40 Maintainer Operations And Issue Publishing Automation
- Create order: 40
- Objective: Automate durable GitHub publication, labels, milestones, blockers, and drift audits.
- Main issues: OC3-N40-001, OC3-N40-002

## Issues

### OC3-T01-001: Implement support classification contract and generator

- Suggested milestone: OC3-T01 Claimable Runtime Surface Audit
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:governance`, `area:conformance`, `area:release`
- Draft blocked by: none
- Draft blocks: OC3-T01-002 (same-milestone sequencing); OC3-N40-001 (publisher automation depends on stable support/draft IDs)

## Objective
[OC3-T01-001] Implement support classification contract and generator.

## Implementation Scope
- Implement [OC3-T01-001] Implement support classification contract and generator with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T01-001] Implement support classification contract and generator with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T01-001`
- Intended milestone title: `OC3-T01 Claimable Runtime Surface Audit`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T01-002: Enforce public claim drift gates across docs and releases

- Suggested milestone: OC3-T01 Claimable Runtime Surface Audit
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:governance`, `area:conformance`, `area:release`
- Draft blocked by: OC3-T01-001 (same-milestone sequencing)
- Draft blocks: OC3-T01-003 (same-milestone sequencing); OC3-T18-001 (conformance claimability needs claim gates); OC3-N35-001 (docs sync depends on claim drift gates)

## Objective
[OC3-T01-002] Enforce public claim drift gates across docs and releases.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T01-002`
- Intended milestone title: `OC3-T01 Claimable Runtime Surface Audit`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T01-003: Wire claimability dashboard into release blockers

- Suggested milestone: OC3-T01 Claimable Runtime Surface Audit
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:governance`, `area:conformance`, `area:release`
- Draft blocked by: OC3-T01-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T01-003] Wire claimability dashboard into release blockers.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T01-003`
- Intended milestone title: `OC3-T01 Claimable Runtime Surface Audit`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T02-001: Parse Objective-C 3.0 containers, protocols, categories, properties, and ivars

- Suggested milestone: OC3-T02 Parser And AST Completeness For Draft Syntax
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:syntax`, `area:structure`
- Draft blocked by: none
- Draft blocks: OC3-T02-002 (same-milestone sequencing); OC3-T03-001 (semantics needs parsed declarations)

## Objective
[OC3-T02-001] Parse Objective-C 3.0 containers, protocols, categories, properties, and ivars.

## Implementation Scope
- Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- Primary source references: `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h`, `native/objc3c/src/lex/objc3_lexer.cpp`, `tests/conformance/parser/README.md`, `tests/tooling/fixtures/native/recovery/negative/`.
- First entrypoints/functions to inspect: `objc3c::parse::Parser`, `ParseIntegerLiteralValue`, `objc3_ast_builder_contract`, `objc3_diagnostic_grammar_hooks_core_feature`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- [ ] Add positive fixtures under `tests/conformance/parser/README.md` and `tests/tooling/fixtures/native/recovery/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:fixture-matrix`, `npm run test:objc3c:negative-expectations`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T02-001`
- Intended milestone title: `OC3-T02 Parser And AST Completeness For Draft Syntax`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T02-002: Parse blocks, throws, async, actor, macro, property-behavior, and interop syntax

- Suggested milestone: OC3-T02 Parser And AST Completeness For Draft Syntax
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:syntax`, `area:structure`
- Draft blocked by: OC3-T02-001 (same-milestone sequencing)
- Draft blocks: OC3-T02-003 (same-milestone sequencing)

## Objective
[OC3-T02-002] Parse blocks, throws, async, actor, macro, property-behavior, and interop syntax.

## Implementation Scope
- Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h`, `native/objc3c/src/lex/objc3_lexer.cpp`, `tests/conformance/parser/README.md`, `tests/tooling/fixtures/native/recovery/negative/`.
- First entrypoints/functions to inspect: `objc3c::parse::Parser`, `ParseIntegerLiteralValue`, `objc3_ast_builder_contract`, `objc3_diagnostic_grammar_hooks_core_feature`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `tests/conformance/parser/README.md` and `tests/tooling/fixtures/native/recovery/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T02-002`
- Intended milestone title: `OC3-T02 Parser And AST Completeness For Draft Syntax`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T02-003: Add parser conformance fixtures and negative diagnostics for every draft syntax surface

- Suggested milestone: OC3-T02 Parser And AST Completeness For Draft Syntax
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:syntax`, `area:structure`
- Draft blocked by: OC3-T02-002 (same-milestone sequencing)
- Draft blocks: OC3-N23-001 (formatter/rewrite needs parser fixture coverage)

## Objective
[OC3-T02-003] Add parser conformance fixtures and negative diagnostics for every draft syntax surface.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h`, `native/objc3c/src/lex/objc3_lexer.cpp`, `tests/conformance/parser/README.md`, `tests/tooling/fixtures/native/recovery/negative/`.
- First entrypoints/functions to inspect: `objc3c::parse::Parser`, `ParseIntegerLiteralValue`, `objc3_ast_builder_contract`, `objc3_diagnostic_grammar_hooks_core_feature`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/parse/objc3_parser.cpp`, `native/objc3c/src/parse/objc3_ast_builder.cpp`, `native/objc3c/src/ast/objc3_ast.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `tests/conformance/parser/README.md` and `tests/tooling/fixtures/native/recovery/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:fixture-matrix`, `npm run test:objc3c:negative-expectations`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:fixture-matrix`
- `npm run test:objc3c:negative-expectations`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T02-003`
- Intended milestone title: `OC3-T02 Parser And AST Completeness For Draft Syntax`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T03-001: Implement typed object, nullability, protocol, and generic semantic model

- Suggested milestone: OC3-T03 Typed Semantic Model Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:semantics`, `area:type-system`
- Draft blocked by: OC3-T02-001 (semantics needs parsed declarations)
- Draft blocks: OC3-T03-002 (same-milestone sequencing); OC3-T04-001 (lowering needs semantic model)

## Objective
[OC3-T03-001] Implement typed object, nullability, protocol, and generic semantic model.

## Implementation Scope
- Implement [OC3-T03-001] Implement typed object, nullability, protocol, and generic semantic model with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T03-001] Implement typed object, nullability, protocol, and generic semantic model with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T03-001`
- Intended milestone title: `OC3-T03 Typed Semantic Model Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T03-002: Implement effects and ownership semantic analysis for ARC, throws, async, blocks, and actors

- Suggested milestone: OC3-T03 Typed Semantic Model Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:semantics`, `area:type-system`
- Draft blocked by: OC3-T03-001 (same-milestone sequencing)
- Draft blocks: OC3-T03-003 (same-milestone sequencing); OC3-T15-001 (macro/property behavior checks need effects model)

## Objective
[OC3-T03-002] Implement effects and ownership semantic analysis for ARC, throws, async, blocks, and actors.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T03-002`
- Intended milestone title: `OC3-T03 Typed Semantic Model Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T03-003: Preserve cross-module semantic contracts and diagnostics

- Suggested milestone: OC3-T03 Typed Semantic Model Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:semantics`, `area:type-system`
- Draft blocked by: OC3-T03-002 (same-milestone sequencing)
- Draft blocks: OC3-N22-001 (LSP needs cross-module semantic contracts); OC3-N25-001 (diagnostic taxonomy needs semantic diagnostic contracts); OC3-N28-001 (incremental cache depends on serialized semantic contracts)

## Objective
[OC3-T03-003] Preserve cross-module semantic contracts and diagnostics.

## Implementation Scope
- Implement [OC3-T03-003] Preserve cross-module semantic contracts and diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T03-003] Preserve cross-module semantic contracts and diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T03-003`
- Intended milestone title: `OC3-T03 Typed Semantic Model Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T04-001: Lower object-model metadata, properties, methods, and ivars to IR

- Suggested milestone: OC3-T04 Lowering And IR Emission Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:codegen`, `area:abi`, `area:runtime`
- Draft blocked by: OC3-T03-001 (lowering needs semantic model)
- Draft blocks: OC3-T04-002 (same-milestone sequencing); OC3-T05-001 (runtime realization needs emitted metadata)

## Objective
[OC3-T04-001] Lower object-model metadata, properties, methods, and ivars to IR.

## Implementation Scope
- Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T04-001`
- Intended milestone title: `OC3-T04 Lowering And IR Emission Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T04-002: Lower blocks, ARC, errors, async, task, and actor constructs to runtime-backed IR

- Suggested milestone: OC3-T04 Lowering And IR Emission Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:codegen`, `area:abi`, `area:runtime`
- Draft blocked by: OC3-T04-001 (same-milestone sequencing)
- Draft blocks: OC3-T04-003 (same-milestone sequencing); OC3-T09-001 (blocks need lowering path); OC3-T11-001 (error lowering needs runtime-backed IR path); OC3-T12-001 (async lowering needs runtime-backed IR path)

## Objective
[OC3-T04-002] Lower blocks, ARC, errors, async, task, and actor constructs to runtime-backed IR.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T04-002`
- Intended milestone title: `OC3-T04 Lowering And IR Emission Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T04-003: Bind manifests, emitted objects, and IR evidence into one truth gate

- Suggested milestone: OC3-T04 Lowering And IR Emission Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:codegen`, `area:abi`, `area:runtime`
- Draft blocked by: OC3-T04-002 (same-milestone sequencing)
- Draft blocks: OC3-T16-001 (interop needs emitted object/manifest truth gate)

## Objective
[OC3-T04-003] Bind manifests, emitted objects, and IR evidence into one truth gate.

## Implementation Scope
- Implement [OC3-T04-003] Bind manifests, emitted objects, and IR evidence into one truth gate with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T04-003] Bind manifests, emitted objects, and IR evidence into one truth gate with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T04-003`
- Intended milestone title: `OC3-T04 Lowering And IR Emission Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T05-001: Implement live class/metaclass graph and root-class realization

- Suggested milestone: OC3-T05 Full Class Metaclass Runtime Realization
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T04-001 (runtime realization needs emitted metadata)
- Draft blocks: OC3-T05-002 (same-milestone sequencing); OC3-T06-001 (protocol/category runtime needs class graph); OC3-T08-001 (dispatch needs class/metaclass runtime); OC3-T17-001 (stdlib runtime APIs need object runtime)

## Objective
[OC3-T05-001] Implement live class/metaclass graph and root-class realization.

## Implementation Scope
- Implement [OC3-T05-001] Implement live class/metaclass graph and root-class realization with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T05-001] Implement live class/metaclass graph and root-class realization with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T05-001`
- Intended milestone title: `OC3-T05 Full Class Metaclass Runtime Realization`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T05-002: Implement instance allocation, initialization, storage layout, and object identity

- Suggested milestone: OC3-T05 Full Class Metaclass Runtime Realization
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T05-001 (same-milestone sequencing)
- Draft blocks: OC3-T05-003 (same-milestone sequencing); OC3-T07-001 (storage/reflection needs realized instance layout)

## Objective
[OC3-T05-002] Implement instance allocation, initialization, storage layout, and object identity.

## Implementation Scope
- Implement [OC3-T05-002] Implement instance allocation, initialization, storage layout, and object identity with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T05-002] Implement instance allocation, initialization, storage layout, and object identity with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T05-002`
- Intended milestone title: `OC3-T05 Full Class Metaclass Runtime Realization`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T05-003: Implement deterministic multi-image registration, reset, and replay

- Suggested milestone: OC3-T05 Full Class Metaclass Runtime Realization
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T05-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T05-003] Implement deterministic multi-image registration, reset, and replay.

## Implementation Scope
- Implement [OC3-T05-003] Implement deterministic multi-image registration, reset, and replay with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T05-003] Implement deterministic multi-image registration, reset, and replay with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T05-003`
- Intended milestone title: `OC3-T05 Full Class Metaclass Runtime Realization`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T06-001: Implement protocol inheritance and conformance query semantics

- Suggested milestone: OC3-T06 Protocol And Category Runtime Semantics
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T05-001 (protocol/category runtime needs class graph)
- Draft blocks: OC3-T06-002 (same-milestone sequencing)

## Objective
[OC3-T06-001] Implement protocol inheritance and conformance query semantics.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T06-001`
- Intended milestone title: `OC3-T06 Protocol And Category Runtime Semantics`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T06-002: Implement category attachment and merged dispatch tables

- Suggested milestone: OC3-T06 Protocol And Category Runtime Semantics
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T06-001 (same-milestone sequencing)
- Draft blocks: OC3-T06-003 (same-milestone sequencing)

## Objective
[OC3-T06-002] Implement category attachment and merged dispatch tables.

## Implementation Scope
- Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T06-002`
- Intended milestone title: `OC3-T06 Protocol And Category Runtime Semantics`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T06-003: Implement protocol/category conflict, visibility, and availability diagnostics

- Suggested milestone: OC3-T06 Protocol And Category Runtime Semantics
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T06-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T06-003] Implement protocol/category conflict, visibility, and availability diagnostics.

## Implementation Scope
- Implement [OC3-T06-003] Implement protocol/category conflict, visibility, and availability diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T06-003] Implement protocol/category conflict, visibility, and availability diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T06-003`
- Intended milestone title: `OC3-T06 Protocol And Category Runtime Semantics`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T07-001: Implement ivar offsets, inherited storage layout, and runtime ivar reflection

- Suggested milestone: OC3-T07 Property Ivar Reflection Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T05-002 (storage/reflection needs realized instance layout)
- Draft blocks: OC3-T07-002 (same-milestone sequencing)

## Objective
[OC3-T07-001] Implement ivar offsets, inherited storage layout, and runtime ivar reflection.

## Implementation Scope
- Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include inherited ivar offsets, alignment/padding, offset stability across replay, and invalid layout diagnostics.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T07-001`
- Intended milestone title: `OC3-T07 Property Ivar Reflection Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T07-002: Implement property metadata reflection and attribute query APIs

- Suggested milestone: OC3-T07 Property Ivar Reflection Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T07-001 (same-milestone sequencing)
- Draft blocks: OC3-T07-003 (same-milestone sequencing)

## Objective
[OC3-T07-002] Implement property metadata reflection and attribute query APIs.

## Implementation Scope
- Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T07-002`
- Intended milestone title: `OC3-T07 Property Ivar Reflection Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T07-003: Implement synthesized property accessor execution backed by runtime storage

- Suggested milestone: OC3-T07 Property Ivar Reflection Closure
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:semantics`
- Draft blocked by: OC3-T07-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T07-003] Implement synthesized property accessor execution backed by runtime storage.

## Implementation Scope
- Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T07-003`
- Intended milestone title: `OC3-T07 Property Ivar Reflection Closure`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T08-001: Implement full message lookup across instance, class, super, category, and protocol surfaces

- Suggested milestone: OC3-T08 Message Dispatch And Method Cache Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:abi`, `area:performance`
- Draft blocked by: OC3-T05-001 (dispatch needs class/metaclass runtime)
- Draft blocks: OC3-T08-002 (same-milestone sequencing)

## Objective
[OC3-T08-001] Implement full message lookup across instance, class, super, category, and protocol surfaces.

## Implementation Scope
- Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T08-001`
- Intended milestone title: `OC3-T08 Message Dispatch And Method Cache Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T08-002: Version typed dispatch ABI beyond i32 return/value assumptions

- Suggested milestone: OC3-T08 Message Dispatch And Method Cache Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:abi`, `area:performance`
- Draft blocked by: OC3-T08-001 (same-milestone sequencing)
- Draft blocks: OC3-T08-003 (same-milestone sequencing); OC3-N33-001 (optimization depends on typed dispatch ABI)

## Objective
[OC3-T08-002] Version typed dispatch ABI beyond i32 return/value assumptions.

## Implementation Scope
- Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T08-002`
- Intended milestone title: `OC3-T08 Message Dispatch And Method Cache Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T08-003: Implement method cache invalidation and remove fallback-only dispatch paths

- Suggested milestone: OC3-T08 Message Dispatch And Method Cache Completeness
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:abi`, `area:performance`
- Draft blocked by: OC3-T08-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T08-003] Implement method cache invalidation and remove fallback-only dispatch paths.

## Implementation Scope
- Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T08-003`
- Intended milestone title: `OC3-T08 Message Dispatch And Method Cache Completeness`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T09-001: Implement block descriptors and runtime invoke thunks

- Suggested milestone: OC3-T09 Escaping Blocks And Byref Runtime Automation
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:memory`
- Draft blocked by: OC3-T04-002 (blocks need lowering path)
- Draft blocks: OC3-T09-002 (same-milestone sequencing)

## Objective
[OC3-T09-001] Implement block descriptors and runtime invoke thunks.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T09-001`
- Intended milestone title: `OC3-T09 Escaping Blocks And Byref Runtime Automation`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T09-002: Implement byref forwarding cells and heap promotion

- Suggested milestone: OC3-T09 Escaping Blocks And Byref Runtime Automation
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:memory`
- Draft blocked by: OC3-T09-001 (same-milestone sequencing)
- Draft blocks: OC3-T09-003 (same-milestone sequencing)

## Objective
[OC3-T09-002] Implement byref forwarding cells and heap promotion.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T09-002`
- Intended milestone title: `OC3-T09 Escaping Blocks And Byref Runtime Automation`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T09-003: Implement escaping block copy/dispose and captured-object ownership

- Suggested milestone: OC3-T09 Escaping Blocks And Byref Runtime Automation
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:runtime`, `area:memory`
- Draft blocked by: OC3-T09-002 (same-milestone sequencing)
- Draft blocks: OC3-T10-001 (ARC integration needs escaping ownership model)

## Objective
[OC3-T09-003] Implement escaping block copy/dispose and captured-object ownership.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T09-003`
- Intended milestone title: `OC3-T09 Escaping Blocks And Byref Runtime Automation`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T10-001: Implement ARC insertion and Objective-C method-family inference

- Suggested milestone: OC3-T10 ARC Automation Beyond Helper-Backed Slice
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:memory`, `area:runtime`
- Draft blocked by: OC3-T09-003 (ARC integration needs escaping ownership model)
- Draft blocks: OC3-T10-002 (same-milestone sequencing)

## Objective
[OC3-T10-001] Implement ARC insertion and Objective-C method-family inference.

## Implementation Scope
- Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T10-001`
- Intended milestone title: `OC3-T10 ARC Automation Beyond Helper-Backed Slice`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T10-002: Implement weak references, autorelease pools, and deterministic destruction order

- Suggested milestone: OC3-T10 ARC Automation Beyond Helper-Backed Slice
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:memory`, `area:runtime`
- Draft blocked by: OC3-T10-001 (same-milestone sequencing)
- Draft blocks: OC3-T10-003 (same-milestone sequencing); OC3-N34-001 (sanitizers need ownership/destruction semantics)

## Objective
[OC3-T10-002] Implement weak references, autorelease pools, and deterministic destruction order.

## Implementation Scope
- Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T10-002`
- Intended milestone title: `OC3-T10 ARC Automation Beyond Helper-Backed Slice`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T10-003: Integrate ARC cleanup with blocks, properties, errors, async, and interop

- Suggested milestone: OC3-T10 ARC Automation Beyond Helper-Backed Slice
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:memory`, `area:runtime`
- Draft blocked by: OC3-T10-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T10-003] Integrate ARC cleanup with blocks, properties, errors, async, and interop.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Include retain/release insertion, method-family inference, weak zeroing, autoreleasepool scope, and cleanup order at exits.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T10-003`
- Intended milestone title: `OC3-T10 ARC Automation Beyond Helper-Backed Slice`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T11-001: Implement throws, try, catch, and rethrow semantic checks

- Suggested milestone: OC3-T11 Throws Error Propagation And Cleanup Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:errors`, `area:runtime`
- Draft blocked by: OC3-T04-002 (error lowering needs runtime-backed IR path)
- Draft blocks: OC3-T11-002 (same-milestone sequencing)

## Objective
[OC3-T11-001] Implement throws, try, catch, and rethrow semantic checks.

## Implementation Scope
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T11-001`
- Intended milestone title: `OC3-T11 Throws Error Propagation And Cleanup Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T11-002: Lower cleanup, unwind, and error ABI paths

- Suggested milestone: OC3-T11 Throws Error Propagation And Cleanup Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:errors`, `area:runtime`
- Draft blocked by: OC3-T11-001 (same-milestone sequencing)
- Draft blocks: OC3-T11-003 (same-milestone sequencing)

## Objective
[OC3-T11-002] Lower cleanup, unwind, and error ABI paths.

## Implementation Scope
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T11-002`
- Intended milestone title: `OC3-T11 Throws Error Propagation And Cleanup Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T11-003: Implement NSError, status-code, and foreign exception bridges

- Suggested milestone: OC3-T11 Throws Error Propagation And Cleanup Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:errors`, `area:runtime`
- Draft blocked by: OC3-T11-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T11-003] Implement NSError, status-code, and foreign exception bridges.

## Implementation Scope
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T11-003`
- Intended milestone title: `OC3-T11 Throws Error Propagation And Cleanup Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T12-001: Implement async function and await lowering

- Suggested milestone: OC3-T12 Async Await And Continuation Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T04-002 (async lowering needs runtime-backed IR path)
- Draft blocks: OC3-T12-002 (same-milestone sequencing)

## Objective
[OC3-T12-001] Implement async function and await lowering.

## Implementation Scope
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T12-001`
- Intended milestone title: `OC3-T12 Async Await And Continuation Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T12-002: Implement continuation runtime ABI and suspension/resume semantics

- Suggested milestone: OC3-T12 Async Await And Continuation Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T12-001 (same-milestone sequencing)
- Draft blocks: OC3-T12-003 (same-milestone sequencing); OC3-T13-001 (tasks need continuation runtime); OC3-N24-001 (debug async frames need continuation metadata)

## Objective
[OC3-T12-002] Implement continuation runtime ABI and suspension/resume semantics.

## Implementation Scope
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T12-002`
- Intended milestone title: `OC3-T12 Async Await And Continuation Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T12-003: Integrate async cancellation, errors, executors, imports, and runtime traces

- Suggested milestone: OC3-T12 Async Await And Continuation Runtime
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T12-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T12-003] Integrate async cancellation, errors, executors, imports, and runtime traces.

## Implementation Scope
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T12-003`
- Intended milestone title: `OC3-T12 Async Await And Continuation Runtime`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T13-001: Implement task spawn, wait, groups, and cancellation semantics

- Suggested milestone: OC3-T13 Task Runtime And Structured Concurrency
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T12-002 (tasks need continuation runtime)
- Draft blocks: OC3-T13-002 (same-milestone sequencing)

## Objective
[OC3-T13-001] Implement task spawn, wait, groups, and cancellation semantics.

## Implementation Scope
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T13-001`
- Intended milestone title: `OC3-T13 Task Runtime And Structured Concurrency`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T13-002: Implement deterministic executor scheduler runtime

- Suggested milestone: OC3-T13 Task Runtime And Structured Concurrency
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T13-001 (same-milestone sequencing)
- Draft blocks: OC3-T13-003 (same-milestone sequencing); OC3-T14-001 (actors need scheduler/executor runtime)

## Objective
[OC3-T13-002] Implement deterministic executor scheduler runtime.

## Implementation Scope
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T13-002`
- Intended milestone title: `OC3-T13 Task Runtime And Structured Concurrency`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T13-003: Replace stdlib concurrency token helpers with runtime-backed tasks

- Suggested milestone: OC3-T13 Task Runtime And Structured Concurrency
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:runtime`
- Draft blocked by: OC3-T13-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T13-003] Replace stdlib concurrency token helpers with runtime-backed tasks.

## Implementation Scope
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T13-003`
- Intended milestone title: `OC3-T13 Task Runtime And Structured Concurrency`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T14-001: Implement actor identity, mailbox, and executor binding

- Suggested milestone: OC3-T14 Actor Runtime And Isolation Enforcement
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:semantics`, `area:runtime`
- Draft blocked by: OC3-T13-002 (actors need scheduler/executor runtime)
- Draft blocks: OC3-T14-002 (same-milestone sequencing)

## Objective
[OC3-T14-001] Implement actor identity, mailbox, and executor binding.

## Implementation Scope
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T14-001`
- Intended milestone title: `OC3-T14 Actor Runtime And Isolation Enforcement`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T14-002: Enforce actor isolation, sendability, and reentrancy semantics

- Suggested milestone: OC3-T14 Actor Runtime And Isolation Enforcement
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:semantics`, `area:runtime`
- Draft blocked by: OC3-T14-001 (same-milestone sequencing)
- Draft blocks: OC3-T14-003 (same-milestone sequencing)

## Objective
[OC3-T14-002] Enforce actor isolation, sendability, and reentrancy semantics.

## Implementation Scope
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T14-002`
- Intended milestone title: `OC3-T14 Actor Runtime And Isolation Enforcement`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T14-003: Integrate actors with storage, tasks, errors, blocks, and imported APIs

- Suggested milestone: OC3-T14 Actor Runtime And Isolation Enforcement
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:concurrency`, `area:semantics`, `area:runtime`
- Draft blocked by: OC3-T14-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T14-003] Integrate actors with storage, tasks, errors, blocks, and imported APIs.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Include success, thrown, bridged, nested cleanup, and foreign-boundary cases with stable diagnostics.
- [ ] Include spawn/wait/group/cancel semantics, executor selection, queue drain determinism, and deadlock/race guards.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T14-003`
- Intended milestone title: `OC3-T14 Actor Runtime And Isolation Enforcement`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T15-001: Implement macro host execution, provenance, hygiene, and sandboxing

- Suggested milestone: OC3-T15 Metaprogramming Macro Host And Property Behaviors
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:macros`, `area:semantics`, `area:security`
- Draft blocked by: OC3-T03-002 (macro/property behavior checks need effects model)
- Draft blocks: OC3-T15-002 (same-milestone sequencing); OC3-N39-001 (macro supply-chain depends on macro host model)

## Objective
[OC3-T15-001] Implement macro host execution, provenance, hygiene, and sandboxing.

## Implementation Scope
- Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- [ ] Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T15-001`
- Intended milestone title: `OC3-T15 Metaprogramming Macro Host And Property Behaviors`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T15-002: Implement property behavior expansion and runtime hooks

- Suggested milestone: OC3-T15 Metaprogramming Macro Host And Property Behaviors
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:macros`, `area:semantics`, `area:security`
- Draft blocked by: OC3-T15-001 (same-milestone sequencing)
- Draft blocks: OC3-T15-003 (same-milestone sequencing)

## Objective
[OC3-T15-002] Implement property behavior expansion and runtime hooks.

## Implementation Scope
- Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include `@property` attributes, synthesized accessors, storage slots, inherited lookup, and reflection query output.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T15-002`
- Intended milestone title: `OC3-T15 Metaprogramming Macro Host And Property Behaviors`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T15-003: Implement macro cache, invalidation, and expansion diagnostics

- Suggested milestone: OC3-T15 Metaprogramming Macro Host And Property Behaviors
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:macros`, `area:semantics`, `area:security`
- Draft blocked by: OC3-T15-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T15-003] Implement macro cache, invalidation, and expansion diagnostics.

## Implementation Scope
- Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-T15-003`
- Intended milestone title: `OC3-T15 Metaprogramming Macro Host And Property Behaviors`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T16-001: Implement C and Objective-C header import/export semantics

- Suggested milestone: OC3-T16 Broader C Cpp Swift Interop And Package Loader
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:interop`, `area:ecosystem`, `area:abi`
- Draft blocked by: OC3-T04-003 (interop needs emitted object/manifest truth gate)
- Draft blocks: OC3-T16-002 (same-milestone sequencing); OC3-N26-001 (migration analysis depends on interop import/export semantics)

## Objective
[OC3-T16-001] Implement C and Objective-C header import/export semantics.

## Implementation Scope
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py`, `registries/experimental_extensions/README.md`, `tests/tooling/`.
- First entrypoints/functions to inspect: `build_objc3c_package_lock.main`, `build_objc3c_package_mirror.main`, `build_objc3c_package_channels.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `registries/experimental_extensions/README.md` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:package-ecosystem:e2e`, `npm run test:objc3c:package-mirror`, `npm run package:objc3c-native:runnable-toolchain` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:package-ecosystem:e2e`
- `npm run test:objc3c:package-mirror`
- `npm run package:objc3c-native:runnable-toolchain`

## GitHub Tracking
- Stable draft ID: `OC3-T16-001`
- Intended milestone title: `OC3-T16 Broader C Cpp Swift Interop And Package Loader`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T16-002: Implement ObjC++ and Swift metadata bridge surfaces

- Suggested milestone: OC3-T16 Broader C Cpp Swift Interop And Package Loader
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:interop`, `area:ecosystem`, `area:abi`
- Draft blocked by: OC3-T16-001 (same-milestone sequencing)
- Draft blocks: OC3-T16-003 (same-milestone sequencing)

## Objective
[OC3-T16-002] Implement ObjC++ and Swift metadata bridge surfaces.

## Implementation Scope
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py`, `registries/experimental_extensions/README.md`, `tests/tooling/`.
- First entrypoints/functions to inspect: `build_objc3c_package_lock.main`, `build_objc3c_package_mirror.main`, `build_objc3c_package_channels.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `registries/experimental_extensions/README.md` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:package-ecosystem:e2e`, `npm run test:objc3c:package-mirror`, `npm run package:objc3c-native:runnable-toolchain` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:package-ecosystem:e2e`
- `npm run test:objc3c:package-mirror`
- `npm run package:objc3c-native:runnable-toolchain`

## GitHub Tracking
- Stable draft ID: `OC3-T16-002`
- Intended milestone title: `OC3-T16 Broader C Cpp Swift Interop And Package Loader`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T16-003: Implement mixed-image package loader ABI and interop execution tests

- Suggested milestone: OC3-T16 Broader C Cpp Swift Interop And Package Loader
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:interop`, `area:ecosystem`, `area:abi`
- Draft blocked by: OC3-T16-002 (same-milestone sequencing)
- Draft blocks: OC3-N27-001 (package manager depends on mixed-image loader ABI)

## Objective
[OC3-T16-003] Implement mixed-image package loader ABI and interop execution tests.

## Implementation Scope
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py`, `registries/experimental_extensions/README.md`, `tests/tooling/`.
- First entrypoints/functions to inspect: `build_objc3c_package_lock.main`, `build_objc3c_package_mirror.main`, `build_objc3c_package_channels.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `registries/experimental_extensions/README.md` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:package-ecosystem:e2e`, `npm run test:objc3c:package-mirror`, `npm run package:objc3c-native:runnable-toolchain` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:package-ecosystem:e2e`
- `npm run test:objc3c:package-mirror`
- `npm run package:objc3c-native:runnable-toolchain`

## GitHub Tracking
- Stable draft ID: `OC3-T16-003`
- Intended milestone title: `OC3-T16 Broader C Cpp Swift Interop And Package Loader`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T17-001: Replace stdlib placeholders with runtime-backed core APIs

- Suggested milestone: OC3-T17 Standard Library Runtime Backing
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:stdlib`, `area:runtime`
- Draft blocked by: OC3-T05-001 (stdlib runtime APIs need object runtime)
- Draft blocks: OC3-T17-002 (same-milestone sequencing)

## Objective
[OC3-T17-001] Replace stdlib placeholders with runtime-backed core APIs.

## Implementation Scope
- Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T17-001`
- Intended milestone title: `OC3-T17 Standard Library Runtime Backing`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T17-002: Define stdlib v1 ABI and semantic compatibility gates

- Suggested milestone: OC3-T17 Standard Library Runtime Backing
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:stdlib`, `area:runtime`
- Draft blocked by: OC3-T17-001 (same-milestone sequencing)
- Draft blocks: OC3-T17-003 (same-milestone sequencing)

## Objective
[OC3-T17-002] Define stdlib v1 ABI and semantic compatibility gates.

## Implementation Scope
- Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runtime-acceptance`, `npm run test:objc3c:runnable-object-model`, `npm run test:objc3c:execution-replay-proof` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T17-002`
- Intended milestone title: `OC3-T17 Standard Library Runtime Backing`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T17-003: Wire stdlib into runnable packages, docs, and conformance tests

- Suggested milestone: OC3-T17 Standard Library Runtime Backing
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:stdlib`, `area:runtime`
- Draft blocked by: OC3-T17-002 (same-milestone sequencing)
- Draft blocks: OC3-N36-001 (showcases depend on runnable stdlib packages)

## Objective
[OC3-T17-003] Wire stdlib into runnable packages, docs, and conformance tests.

## Implementation Scope
- Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`, `tests/tooling/fixtures/native/execution/positive/`, `tests/tooling/fixtures/native/execution/negative/`.
- First entrypoints/functions to inspect: `objc3_runtime_register_image`, `objc3_runtime_reset_for_testing`, `objc3_runtime_lookup_selector`, `objc3_msgSend_i32`, `objc3_runtime_describe_*`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `tests/tooling/fixtures/native/execution/positive/` and `tests/tooling/fixtures/native/execution/negative/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runtime-acceptance`
- `npm run test:objc3c:runnable-object-model`
- `npm run test:objc3c:execution-replay-proof`

## GitHub Tracking
- Stable draft ID: `OC3-T17-003`
- Intended milestone title: `OC3-T17 Standard Library Runtime Backing`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T18-001: Promote modeled conformance into executable tests

- Suggested milestone: OC3-T18 Conformance Corpus From Breadth To Claimability
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:conformance`, `area:validation`
- Draft blocked by: OC3-T01-002 (conformance claimability needs claim gates)
- Draft blocks: OC3-T18-002 (same-milestone sequencing); OC3-T19-001 (production performance needs executable conformance base); OC3-N32-001 (fuzz promotion depends on executable conformance base)

## Objective
[OC3-T18-001] Promote modeled conformance into executable tests.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py`, `scripts/check_conformance_corpus_surface.py`, `scripts/check_objc3c_public_conformance_reporting_end_to_end.py`, `reports/`.
- First entrypoints/functions to inspect: `generate_conformance_corpus_index.main`, `generate_conformance_evidence_index.main`, `check_conformance_corpus_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `scripts/check_objc3c_public_conformance_reporting_end_to_end.py` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-conformance-corpus`, `npm run test:objc3c:public-conformance:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-conformance-corpus`
- `npm run test:objc3c:public-conformance:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T18-001`
- Intended milestone title: `OC3-T18 Conformance Corpus From Breadth To Claimability`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T18-002: Enforce traceability from support claims to runnable evidence

- Suggested milestone: OC3-T18 Conformance Corpus From Breadth To Claimability
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:conformance`, `area:validation`
- Draft blocked by: OC3-T18-001 (same-milestone sequencing)
- Draft blocks: OC3-T18-003 (same-milestone sequencing); OC3-T20-001 (release ABI claims need traceability); OC3-N38-001 (external validation depends on traceable evidence)

## Objective
[OC3-T18-002] Enforce traceability from support claims to runnable evidence.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py`, `scripts/check_conformance_corpus_surface.py`, `scripts/check_objc3c_public_conformance_reporting_end_to_end.py`, `reports/`.
- First entrypoints/functions to inspect: `generate_conformance_corpus_index.main`, `generate_conformance_evidence_index.main`, `check_conformance_corpus_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `scripts/check_objc3c_public_conformance_reporting_end_to_end.py` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-conformance-corpus`, `npm run test:objc3c:public-conformance:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-conformance-corpus`
- `npm run test:objc3c:public-conformance:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T18-002`
- Intended milestone title: `OC3-T18 Conformance Corpus From Breadth To Claimability`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T18-003: Add longitudinal regression suites for every supported surface

- Suggested milestone: OC3-T18 Conformance Corpus From Breadth To Claimability
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:conformance`, `area:validation`
- Draft blocked by: OC3-T18-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T18-003] Add longitudinal regression suites for every supported surface.

## Implementation Scope
- Implement [OC3-T18-003] Add longitudinal regression suites for every supported surface with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py`, `scripts/check_conformance_corpus_surface.py`, `scripts/check_objc3c_public_conformance_reporting_end_to_end.py`, `reports/`.
- First entrypoints/functions to inspect: `generate_conformance_corpus_index.main`, `generate_conformance_evidence_index.main`, `check_conformance_corpus_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T18-003] Add longitudinal regression suites for every supported surface with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/check_objc3c_public_conformance_reporting_end_to_end.py` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-conformance-corpus`, `npm run test:objc3c:public-conformance:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-conformance-corpus`
- `npm run test:objc3c:public-conformance:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T18-003`
- Intended milestone title: `OC3-T18 Conformance Corpus From Breadth To Claimability`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T19-001: Benchmark runtime features and compile throughput

- Suggested milestone: OC3-T19 Production Performance Stress And Runtime Hardening
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:performance`, `area:runtime`, `area:validation`
- Draft blocked by: OC3-T18-001 (production performance needs executable conformance base)
- Draft blocks: OC3-T19-002 (same-milestone sequencing); OC3-N31-001 (observability depends on benchmark/stress surface)

## Objective
[OC3-T19-001] Benchmark runtime features and compile throughput.

## Implementation Scope
- Capture baseline, threshold, machine profile, workload source, and regression policy so performance claims are reproducible.
- Primary source references: `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py`, `scripts/run_objc3c_stress_crash_triage.py`, `tests/tooling/fixtures/runtime_performance/`.
- First entrypoints/functions to inspect: `benchmark_compile_workload`, `benchmark_runtime_workload`, `run_objc3c_stress_minimization.main`, `run_objc3c_stress_crash_triage.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Capture baseline, threshold, machine profile, workload source, and regression policy so performance claims are reproducible.
- [ ] Add positive fixtures under `scripts/run_objc3c_stress_crash_triage.py` and `tests/tooling/fixtures/runtime_performance/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:performance-governance:e2e`, `npm run test:objc3c:stress:e2e`, `npm run inspect:objc3c:performance-dashboard` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:performance-governance:e2e`
- `npm run test:objc3c:stress:e2e`
- `npm run inspect:objc3c:performance-dashboard`

## GitHub Tracking
- Stable draft ID: `OC3-T19-001`
- Intended milestone title: `OC3-T19 Production Performance Stress And Runtime Hardening`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T19-002: Expand stress, fuzz, minimization, and crash triage coverage

- Suggested milestone: OC3-T19 Production Performance Stress And Runtime Hardening
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:performance`, `area:runtime`, `area:validation`
- Draft blocked by: OC3-T19-001 (same-milestone sequencing)
- Draft blocks: OC3-T19-003 (same-milestone sequencing)

## Objective
[OC3-T19-002] Expand stress, fuzz, minimization, and crash triage coverage.

## Implementation Scope
- Implement [OC3-T19-002] Expand stress, fuzz, minimization, and crash triage coverage with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py`, `scripts/run_objc3c_stress_crash_triage.py`, `tests/tooling/fixtures/runtime_performance/`.
- First entrypoints/functions to inspect: `benchmark_compile_workload`, `benchmark_runtime_workload`, `run_objc3c_stress_minimization.main`, `run_objc3c_stress_crash_triage.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T19-002] Expand stress, fuzz, minimization, and crash triage coverage with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/run_objc3c_stress_crash_triage.py` and `tests/tooling/fixtures/runtime_performance/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:performance-governance:e2e`, `npm run test:objc3c:stress:e2e`, `npm run inspect:objc3c:performance-dashboard` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:performance-governance:e2e`
- `npm run test:objc3c:stress:e2e`
- `npm run inspect:objc3c:performance-dashboard`

## GitHub Tracking
- Stable draft ID: `OC3-T19-002`
- Intended milestone title: `OC3-T19 Production Performance Stress And Runtime Hardening`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T19-003: Gate performance and stability regressions in release governance

- Suggested milestone: OC3-T19 Production Performance Stress And Runtime Hardening
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:performance`, `area:runtime`, `area:validation`
- Draft blocked by: OC3-T19-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T19-003] Gate performance and stability regressions in release governance.

## Implementation Scope
- Capture baseline, threshold, machine profile, workload source, and regression policy so performance claims are reproducible.
- Primary source references: `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py`, `scripts/run_objc3c_stress_crash_triage.py`, `tests/tooling/fixtures/runtime_performance/`.
- First entrypoints/functions to inspect: `benchmark_compile_workload`, `benchmark_runtime_workload`, `run_objc3c_stress_minimization.main`, `run_objc3c_stress_crash_triage.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Capture baseline, threshold, machine profile, workload source, and regression policy so performance claims are reproducible.
- [ ] Add positive fixtures under `scripts/run_objc3c_stress_crash_triage.py` and `tests/tooling/fixtures/runtime_performance/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:performance-governance:e2e`, `npm run test:objc3c:stress:e2e`, `npm run inspect:objc3c:performance-dashboard` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:performance-governance:e2e`
- `npm run test:objc3c:stress:e2e`
- `npm run inspect:objc3c:performance-dashboard`

## GitHub Tracking
- Stable draft ID: `OC3-T19-003`
- Intended milestone title: `OC3-T19 Production Performance Stress And Runtime Hardening`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T20-001: Version public runtime ABI and graduate private helpers

- Suggested milestone: OC3-T20 Release Grade Public ABI Packaging Security And Governance
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:release`, `area:security`, `area:abi`
- Draft blocked by: OC3-T18-002 (release ABI claims need traceability)
- Draft blocks: OC3-T20-002 (same-milestone sequencing); OC3-N21-001 (bootstrap reproducibility follows release artifact rules); OC3-N37-001 (ABI governance depends on versioned public runtime ABI)

## Objective
[OC3-T20-001] Version public runtime ABI and graduate private helpers.

## Implementation Scope
- Implement [OC3-T20-001] Version public runtime ABI and graduate private helpers with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-T20-001] Version public runtime ABI and graduate private helpers with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T20-001`
- Intended milestone title: `OC3-T20 Release Grade Public ABI Packaging Security And Governance`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T20-002: Attest packages, SBOMs, update metadata, and runtime artifacts

- Suggested milestone: OC3-T20 Release Grade Public ABI Packaging Security And Governance
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:release`, `area:security`, `area:abi`
- Draft blocked by: OC3-T20-001 (same-milestone sequencing)
- Draft blocks: OC3-T20-003 (same-milestone sequencing); OC3-N30-001 (installer updates depend on attested artifacts)

## Objective
[OC3-T20-002] Attest packages, SBOMs, update metadata, and runtime artifacts.

## Implementation Scope
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T20-002`
- Intended milestone title: `OC3-T20 Release Grade Public ABI Packaging Security And Governance`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-T20-003: Run security, governance, deprecation, and compatibility release gates

- Suggested milestone: OC3-T20 Release Grade Public ABI Packaging Security And Governance
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P0`, `area:release`, `area:security`, `area:abi`
- Draft blocked by: OC3-T20-002 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-T20-003] Run security, governance, deprecation, and compatibility release gates.

## Implementation Scope
- Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-T20-003`
- Intended milestone title: `OC3-T20 Release Grade Public ABI Packaging Security And Governance`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N21-001: Make bootstrap outputs deterministic and reproducible

- Suggested milestone: OC3-N21 Deterministic Bootstrap And Build Reproducibility
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:platform`, `area:provenance`
- Draft blocked by: OC3-T20-001 (bootstrap reproducibility follows release artifact rules)
- Draft blocks: OC3-N21-002 (same-milestone sequencing); OC3-N29-001 (platform matrix depends on reproducible bootstrap)

## Objective
[OC3-N21-001] Make bootstrap outputs deterministic and reproducible.

## Implementation Scope
- Implement [OC3-N21-001] Make bootstrap outputs deterministic and reproducible with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/CMakeLists.txt`, `scripts/build_objc3c_native.ps1`, `scripts/ensure_objc3c_native_build.py`, `scripts/run_bootstrap_readiness.py`, `tests/tooling/test_run_bootstrap_readiness.py`.
- First entrypoints/functions to inspect: `ensure_objc3c_native_build.main`, `run_bootstrap_readiness.main`, `build_objc3c_native.ps1`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/CMakeLists.txt`, `scripts/build_objc3c_native.ps1`, `scripts/ensure_objc3c_native_build.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N21-001] Make bootstrap outputs deterministic and reproducible with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/run_bootstrap_readiness.py` and `tests/tooling/test_run_bootstrap_readiness.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run build:objc3c-native:full`, `npm run test:objc3c:runnable-bootstrap`, `npm run check:objc3c:source-hygiene` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run build:objc3c-native:full`
- `npm run test:objc3c:runnable-bootstrap`
- `npm run check:objc3c:source-hygiene`

## GitHub Tracking
- Stable draft ID: `OC3-N21-001`
- Intended milestone title: `OC3-N21 Deterministic Bootstrap And Build Reproducibility`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N21-002: Add clean-room bootstrap rebuild gates

- Suggested milestone: OC3-N21 Deterministic Bootstrap And Build Reproducibility
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:platform`, `area:provenance`
- Draft blocked by: OC3-N21-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N21-002] Add clean-room bootstrap rebuild gates.

## Implementation Scope
- Implement [OC3-N21-002] Add clean-room bootstrap rebuild gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/CMakeLists.txt`, `scripts/build_objc3c_native.ps1`, `scripts/ensure_objc3c_native_build.py`, `scripts/run_bootstrap_readiness.py`, `tests/tooling/test_run_bootstrap_readiness.py`.
- First entrypoints/functions to inspect: `ensure_objc3c_native_build.main`, `run_bootstrap_readiness.main`, `build_objc3c_native.ps1`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/CMakeLists.txt`, `scripts/build_objc3c_native.ps1`, `scripts/ensure_objc3c_native_build.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N21-002] Add clean-room bootstrap rebuild gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/run_bootstrap_readiness.py` and `tests/tooling/test_run_bootstrap_readiness.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run build:objc3c-native:full`, `npm run test:objc3c:runnable-bootstrap`, `npm run check:objc3c:source-hygiene` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run build:objc3c-native:full`
- `npm run test:objc3c:runnable-bootstrap`
- `npm run check:objc3c:source-hygiene`

## GitHub Tracking
- Stable draft ID: `OC3-N21-002`
- Intended milestone title: `OC3-N21 Deterministic Bootstrap And Build Reproducibility`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N22-001: Implement LSP navigation for Objective-C 3.0 declarations and symbols

- Suggested milestone: OC3-N22 Developer Tooling LSP Navigation And Workspace Semantics
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:ux`
- Draft blocked by: OC3-T03-003 (LSP needs cross-module semantic contracts)
- Draft blocks: OC3-N22-002 (same-milestone sequencing)

## Objective
[OC3-N22-001] Implement LSP navigation for Objective-C 3.0 declarations and symbols.

## Implementation Scope
- Implement [OC3-N22-001] Implement LSP navigation for Objective-C 3.0 declarations and symbols with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N22-001] Implement LSP navigation for Objective-C 3.0 declarations and symbols with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N22-001`
- Intended milestone title: `OC3-N22 Developer Tooling LSP Navigation And Workspace Semantics`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N22-002: Implement workspace indexing and cross-package semantic navigation

- Suggested milestone: OC3-N22 Developer Tooling LSP Navigation And Workspace Semantics
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:ux`
- Draft blocked by: OC3-N22-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N22-002] Implement workspace indexing and cross-package semantic navigation.

## Implementation Scope
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N22-002`
- Intended milestone title: `OC3-N22 Developer Tooling LSP Navigation And Workspace Semantics`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N23-001: Implement formatter coverage for Objective-C 3.0 syntax

- Suggested milestone: OC3-N23 Formatter Refactor And Source Rewrite Tools
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:syntax`
- Draft blocked by: OC3-T02-003 (formatter/rewrite needs parser fixture coverage)
- Draft blocks: OC3-N23-002 (same-milestone sequencing)

## Objective
[OC3-N23-001] Implement formatter coverage for Objective-C 3.0 syntax.

## Implementation Scope
- Implement [OC3-N23-001] Implement formatter coverage for Objective-C 3.0 syntax with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N23-001] Implement formatter coverage for Objective-C 3.0 syntax with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N23-001`
- Intended milestone title: `OC3-N23 Formatter Refactor And Source Rewrite Tools`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N23-002: Implement safe refactor and source rewrite tooling

- Suggested milestone: OC3-N23 Formatter Refactor And Source Rewrite Tools
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:syntax`
- Draft blocked by: OC3-N23-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N23-002] Implement safe refactor and source rewrite tooling.

## Implementation Scope
- Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include actor identity, mailbox ordering, isolation diagnostics, sendability checks, reentrancy policy, and imported actor APIs.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N23-002`
- Intended milestone title: `OC3-N23 Formatter Refactor And Source Rewrite Tools`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N24-001: Emit debugger source maps and async/runtime frame metadata

- Suggested milestone: OC3-N24 Debugger Source Maps And Runtime Inspection
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:runtime`
- Draft blocked by: OC3-T12-002 (debug async frames need continuation metadata)
- Draft blocks: OC3-N24-002 (same-milestone sequencing)

## Objective
[OC3-N24-001] Emit debugger source maps and async/runtime frame metadata.

## Implementation Scope
- Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- Primary source references: `scripts/benchmark_objc3c_runtime_inspector.py`, `scripts/build_developer_tooling_debug_semantics_summary.py`, `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/test_benchmark_objc3c_runtime_inspector.py`.
- First entrypoints/functions to inspect: `benchmark_objc3c_runtime_inspector.main`, `objc3_runtime_describe_*`, `Objc3IREmitter`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_runtime_inspector.py`, `scripts/build_developer_tooling_debug_semantics_summary.py`, `native/objc3c/src/runtime/objc3_runtime.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include suspension state shape, continuation resume rules, cancellation propagation, error paths, and deterministic replay.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/test_benchmark_objc3c_runtime_inspector.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run inspect:objc3c:runtime`, `npm run inspect:objc3c:benchmark`, `npm run test:objc3c:runnable-developer-tooling` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run inspect:objc3c:runtime`
- `npm run inspect:objc3c:benchmark`
- `npm run test:objc3c:runnable-developer-tooling`

## GitHub Tracking
- Stable draft ID: `OC3-N24-001`
- Intended milestone title: `OC3-N24 Debugger Source Maps And Runtime Inspection`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N24-002: Implement runtime inspector commands for object-model and concurrency state

- Suggested milestone: OC3-N24 Debugger Source Maps And Runtime Inspection
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:runtime`
- Draft blocked by: OC3-N24-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N24-002] Implement runtime inspector commands for object-model and concurrency state.

## Implementation Scope
- Implement [OC3-N24-002] Implement runtime inspector commands for object-model and concurrency state with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/benchmark_objc3c_runtime_inspector.py`, `scripts/build_developer_tooling_debug_semantics_summary.py`, `native/objc3c/src/runtime/objc3_runtime.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/test_benchmark_objc3c_runtime_inspector.py`.
- First entrypoints/functions to inspect: `benchmark_objc3c_runtime_inspector.main`, `objc3_runtime_describe_*`, `Objc3IREmitter`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_runtime_inspector.py`, `scripts/build_developer_tooling_debug_semantics_summary.py`, `native/objc3c/src/runtime/objc3_runtime.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N24-002] Implement runtime inspector commands for object-model and concurrency state with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/test_benchmark_objc3c_runtime_inspector.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run inspect:objc3c:runtime`, `npm run inspect:objc3c:benchmark`, `npm run test:objc3c:runnable-developer-tooling` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run inspect:objc3c:runtime`
- `npm run inspect:objc3c:benchmark`
- `npm run test:objc3c:runnable-developer-tooling`

## GitHub Tracking
- Stable draft ID: `OC3-N24-002`
- Intended milestone title: `OC3-N24 Debugger Source Maps And Runtime Inspection`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N25-001: Implement diagnostic code taxonomy and structured output

- Suggested milestone: OC3-N25 Diagnostic Taxonomy And Fix-It Quality
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:semantics`
- Draft blocked by: OC3-T03-003 (diagnostic taxonomy needs semantic diagnostic contracts)
- Draft blocks: OC3-N25-002 (same-milestone sequencing)

## Objective
[OC3-N25-001] Implement diagnostic code taxonomy and structured output.

## Implementation Scope
- Implement [OC3-N25-001] Implement diagnostic code taxonomy and structured output with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N25-001] Implement diagnostic code taxonomy and structured output with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N25-001`
- Intended milestone title: `OC3-N25 Diagnostic Taxonomy And Fix-It Quality`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N25-002: Raise fix-it coverage and correctness gates

- Suggested milestone: OC3-N25 Diagnostic Taxonomy And Fix-It Quality
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:semantics`
- Draft blocked by: OC3-N25-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N25-002] Raise fix-it coverage and correctness gates.

## Implementation Scope
- Implement [OC3-N25-002] Raise fix-it coverage and correctness gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N25-002] Raise fix-it coverage and correctness gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N25-002`
- Intended milestone title: `OC3-N25 Diagnostic Taxonomy And Fix-It Quality`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N26-001: Implement ObjC2/Swift/C++ migration analyzer

- Suggested milestone: OC3-N26 ObjC2 Swift Cpp Migration Tooling
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:adoption`, `area:interop`
- Draft blocked by: OC3-T16-001 (migration analysis depends on interop import/export semantics)
- Draft blocks: OC3-N26-002 (same-milestone sequencing)

## Objective
[OC3-N26-001] Implement ObjC2/Swift/C++ migration analyzer.

## Implementation Scope
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N26-001`
- Intended milestone title: `OC3-N26 ObjC2 Swift Cpp Migration Tooling`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N26-002: Implement migration rewrite and report workflow

- Suggested milestone: OC3-N26 ObjC2 Swift Cpp Migration Tooling
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:adoption`, `area:interop`
- Draft blocked by: OC3-N26-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N26-002] Implement migration rewrite and report workflow.

## Implementation Scope
- Implement [OC3-N26-002] Implement migration rewrite and report workflow with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N26-002] Implement migration rewrite and report workflow with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N26-002`
- Intended milestone title: `OC3-N26 ObjC2 Swift Cpp Migration Tooling`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N27-001: Implement package resolver, lockfile, and registry metadata semantics

- Suggested milestone: OC3-N27 Package Manager Registry And Offline Mirror
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:ecosystem`, `area:release`
- Draft blocked by: OC3-T16-003 (package manager depends on mixed-image loader ABI)
- Draft blocks: OC3-N27-002 (same-milestone sequencing)

## Objective
[OC3-N27-001] Implement package resolver, lockfile, and registry metadata semantics.

## Implementation Scope
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py`, `registries/experimental_extensions/README.md`, `tests/tooling/`.
- First entrypoints/functions to inspect: `build_objc3c_package_lock.main`, `build_objc3c_package_mirror.main`, `build_objc3c_package_channels.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `registries/experimental_extensions/README.md` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:package-ecosystem:e2e`, `npm run test:objc3c:package-mirror`, `npm run package:objc3c-native:runnable-toolchain` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:package-ecosystem:e2e`
- `npm run test:objc3c:package-mirror`
- `npm run package:objc3c-native:runnable-toolchain`

## GitHub Tracking
- Stable draft ID: `OC3-N27-001`
- Intended milestone title: `OC3-N27 Package Manager Registry And Offline Mirror`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N27-002: Implement offline mirror install and verification workflow

- Suggested milestone: OC3-N27 Package Manager Registry And Offline Mirror
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:ecosystem`, `area:release`
- Draft blocked by: OC3-N27-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N27-002] Implement offline mirror install and verification workflow.

## Implementation Scope
- Implement [OC3-N27-002] Implement offline mirror install and verification workflow with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py`, `registries/experimental_extensions/README.md`, `tests/tooling/`.
- First entrypoints/functions to inspect: `build_objc3c_package_lock.main`, `build_objc3c_package_mirror.main`, `build_objc3c_package_channels.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_package_lock.py`, `scripts/build_objc3c_package_mirror.py`, `scripts/build_objc3c_package_channels.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N27-002] Implement offline mirror install and verification workflow with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `registries/experimental_extensions/README.md` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:package-ecosystem:e2e`, `npm run test:objc3c:package-mirror`, `npm run package:objc3c-native:runnable-toolchain` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:package-ecosystem:e2e`
- `npm run test:objc3c:package-mirror`
- `npm run package:objc3c-native:runnable-toolchain`

## GitHub Tracking
- Stable draft ID: `OC3-N27-002`
- Intended milestone title: `OC3-N27 Package Manager Registry And Offline Mirror`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N28-001: Implement module-interface cache and invalidation semantics

- Suggested milestone: OC3-N28 Incremental Compilation Module Cache And Invalidations
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:modules`, `area:tooling`
- Draft blocked by: OC3-T03-003 (incremental cache depends on serialized semantic contracts)
- Draft blocks: OC3-N28-002 (same-milestone sequencing)

## Objective
[OC3-N28-001] Implement module-interface cache and invalidation semantics.

## Implementation Scope
- Implement [OC3-N28-001] Implement module-interface cache and invalidation semantics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N28-001] Implement module-interface cache and invalidation semantics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N28-001`
- Intended milestone title: `OC3-N28 Incremental Compilation Module Cache And Invalidations`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N28-002: Prove incremental runtime metadata consistency

- Suggested milestone: OC3-N28 Incremental Compilation Module Cache And Invalidations
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:modules`, `area:tooling`
- Draft blocked by: OC3-N28-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N28-002] Prove incremental runtime metadata consistency.

## Implementation Scope
- Implement [OC3-N28-002] Prove incremental runtime metadata consistency with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N28-002] Prove incremental runtime metadata consistency with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N28-002`
- Intended milestone title: `OC3-N28 Incremental Compilation Module Cache And Invalidations`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N29-001: Expand CI platform matrix for compiler, runtime, stdlib, and packaging

- Suggested milestone: OC3-N29 Cross Platform Toolchain Matrix
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:platform`, `area:validation`
- Draft blocked by: OC3-N21-001 (platform matrix depends on reproducible bootstrap)
- Draft blocks: OC3-N29-002 (same-milestone sequencing)

## Objective
[OC3-N29-001] Expand CI platform matrix for compiler, runtime, stdlib, and packaging.

## Implementation Scope
- Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include package module manifests, runtime-backed implementations, examples, docs, and capability-driven support claims.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N29-001`
- Intended milestone title: `OC3-N29 Cross Platform Toolchain Matrix`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N29-002: Add per-platform packaged runtime acceptance tests

- Suggested milestone: OC3-N29 Cross Platform Toolchain Matrix
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:platform`, `area:validation`
- Draft blocked by: OC3-N29-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N29-002] Add per-platform packaged runtime acceptance tests.

## Implementation Scope
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N29-002`
- Intended milestone title: `OC3-N29 Cross Platform Toolchain Matrix`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N30-001: Implement signed installer and update-channel validation

- Suggested milestone: OC3-N30 Installer Update And User-Facing Release UX
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:release`, `area:ux`
- Draft blocked by: OC3-T20-002 (installer updates depend on attested artifacts)
- Draft blocks: OC3-N30-002 (same-milestone sequencing)

## Objective
[OC3-N30-001] Implement signed installer and update-channel validation.

## Implementation Scope
- Implement [OC3-N30-001] Implement signed installer and update-channel validation with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N30-001] Implement signed installer and update-channel validation with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N30-001`
- Intended milestone title: `OC3-N30 Installer Update And User-Facing Release UX`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N30-002: Implement rollback and user-facing release diagnostics

- Suggested milestone: OC3-N30 Installer Update And User-Facing Release UX
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:release`, `area:ux`
- Draft blocked by: OC3-N30-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N30-002] Implement rollback and user-facing release diagnostics.

## Implementation Scope
- Implement [OC3-N30-002] Implement rollback and user-facing release diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N30-002] Implement rollback and user-facing release diagnostics with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N30-002`
- Intended milestone title: `OC3-N30 Installer Update And User-Facing Release UX`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N31-001: Implement compiler trace spans and build observability output

- Suggested milestone: OC3-N31 Observability Compile Tracing And Runtime Telemetry
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:runtime`
- Draft blocked by: OC3-T19-001 (observability depends on benchmark/stress surface)
- Draft blocks: OC3-N31-002 (same-milestone sequencing)

## Objective
[OC3-N31-001] Implement compiler trace spans and build observability output.

## Implementation Scope
- Implement [OC3-N31-001] Implement compiler trace spans and build observability output with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N31-001] Implement compiler trace spans and build observability output with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N31-001`
- Intended milestone title: `OC3-N31 Observability Compile Tracing And Runtime Telemetry`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N31-002: Implement opt-in runtime telemetry hooks and privacy gates

- Suggested milestone: OC3-N31 Observability Compile Tracing And Runtime Telemetry
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:tooling`, `area:runtime`
- Draft blocked by: OC3-N31-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N31-002] Implement opt-in runtime telemetry hooks and privacy gates.

## Implementation Scope
- Implement [OC3-N31-002] Implement opt-in runtime telemetry hooks and privacy gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N31-002] Implement opt-in runtime telemetry hooks and privacy gates with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N31-002`
- Intended milestone title: `OC3-N31 Observability Compile Tracing And Runtime Telemetry`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N32-001: Implement grammar, semantic, and runtime fuzz generators

- Suggested milestone: OC3-N32 Fuzzing Crash Minimization And Differential Testing
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:validation`, `area:safety`
- Draft blocked by: OC3-T18-001 (fuzz promotion depends on executable conformance base)
- Draft blocks: OC3-N32-002 (same-milestone sequencing)

## Objective
[OC3-N32-001] Implement grammar, semantic, and runtime fuzz generators.

## Implementation Scope
- Implement [OC3-N32-001] Implement grammar, semantic, and runtime fuzz generators with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py`, `scripts/run_objc3c_stress_crash_triage.py`, `tests/tooling/fixtures/runtime_performance/`.
- First entrypoints/functions to inspect: `benchmark_compile_workload`, `benchmark_runtime_workload`, `run_objc3c_stress_minimization.main`, `run_objc3c_stress_crash_triage.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N32-001] Implement grammar, semantic, and runtime fuzz generators with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/run_objc3c_stress_crash_triage.py` and `tests/tooling/fixtures/runtime_performance/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:performance-governance:e2e`, `npm run test:objc3c:stress:e2e`, `npm run inspect:objc3c:performance-dashboard` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:performance-governance:e2e`
- `npm run test:objc3c:stress:e2e`
- `npm run inspect:objc3c:performance-dashboard`

## GitHub Tracking
- Stable draft ID: `OC3-N32-001`
- Intended milestone title: `OC3-N32 Fuzzing Crash Minimization And Differential Testing`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N32-002: Implement crash minimization and differential corpus promotion

- Suggested milestone: OC3-N32 Fuzzing Crash Minimization And Differential Testing
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:validation`, `area:safety`
- Draft blocked by: OC3-N32-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N32-002] Implement crash minimization and differential corpus promotion.

## Implementation Scope
- Implement [OC3-N32-002] Implement crash minimization and differential corpus promotion with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py`, `scripts/run_objc3c_stress_crash_triage.py`, `tests/tooling/fixtures/runtime_performance/`.
- First entrypoints/functions to inspect: `benchmark_compile_workload`, `benchmark_runtime_workload`, `run_objc3c_stress_minimization.main`, `run_objc3c_stress_crash_triage.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/benchmark_objc3c_performance.py`, `scripts/benchmark_objc3c_runtime_performance.py`, `scripts/run_objc3c_stress_minimization.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N32-002] Implement crash minimization and differential corpus promotion with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/run_objc3c_stress_crash_triage.py` and `tests/tooling/fixtures/runtime_performance/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:performance-governance:e2e`, `npm run test:objc3c:stress:e2e`, `npm run inspect:objc3c:performance-dashboard` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:performance-governance:e2e`
- `npm run test:objc3c:stress:e2e`
- `npm run inspect:objc3c:performance-dashboard`

## GitHub Tracking
- Stable draft ID: `OC3-N32-002`
- Intended milestone title: `OC3-N32 Fuzzing Crash Minimization And Differential Testing`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N33-001: Implement codegen optimization passes guarded by semantic tests

- Suggested milestone: OC3-N33 Codegen Optimization And Direct Dispatch Policy
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:codegen`, `area:optimizer`, `area:performance`
- Draft blocked by: OC3-T08-002 (optimization depends on typed dispatch ABI)
- Draft blocks: OC3-N33-002 (same-milestone sequencing)

## Objective
[OC3-N33-001] Implement codegen optimization passes guarded by semantic tests.

## Implementation Scope
- Implement [OC3-N33-001] Implement codegen optimization passes guarded by semantic tests with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N33-001] Implement codegen optimization passes guarded by semantic tests with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N33-001`
- Intended milestone title: `OC3-N33 Codegen Optimization And Direct Dispatch Policy`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N33-002: Define and enforce direct-dispatch policy

- Suggested milestone: OC3-N33 Codegen Optimization And Direct Dispatch Policy
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:codegen`, `area:optimizer`, `area:performance`
- Draft blocked by: OC3-N33-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N33-002] Define and enforce direct-dispatch policy.

## Implementation Scope
- Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include nil receiver behavior, super sends, category/protocol lookup order, typed return/value marshalling, and cache invalidation.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N33-002`
- Intended milestone title: `OC3-N33 Codegen Optimization And Direct Dispatch Policy`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N34-001: Add sanitizer-backed runtime and compiler test jobs

- Suggested milestone: OC3-N34 Memory Safety UB And Runtime Sanitizer Audits
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:safety`, `area:security`, `area:memory`
- Draft blocked by: OC3-T10-002 (sanitizers need ownership/destruction semantics)
- Draft blocks: OC3-N34-002 (same-milestone sequencing)

## Objective
[OC3-N34-001] Add sanitizer-backed runtime and compiler test jobs.

## Implementation Scope
- Implement [OC3-N34-001] Add sanitizer-backed runtime and compiler test jobs with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py`, `scripts/check_security_hardening_supply_chain_audit.py`, `scripts/check_security_hardening_runtime_hardening.py`.
- First entrypoints/functions to inspect: `build_objc3c_security_posture.main`, `check_security_hardening_supply_chain_audit.main`, `check_security_hardening_runtime_hardening.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N34-001] Add sanitizer-backed runtime and compiler test jobs with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/check_security_hardening_supply_chain_audit.py` and `scripts/check_security_hardening_runtime_hardening.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:security-hardening:e2e`, `npm run check:objc3c:security-hardening:surface`, `npm run inspect:objc3c:security-posture` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:security-hardening:e2e`
- `npm run check:objc3c:security-hardening:surface`
- `npm run inspect:objc3c:security-posture`

## GitHub Tracking
- Stable draft ID: `OC3-N34-001`
- Intended milestone title: `OC3-N34 Memory Safety UB And Runtime Sanitizer Audits`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N34-002: Fix memory-safety and undefined-behavior findings behind blockers

- Suggested milestone: OC3-N34 Memory Safety UB And Runtime Sanitizer Audits
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:safety`, `area:security`, `area:memory`
- Draft blocked by: OC3-N34-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N34-002] Fix memory-safety and undefined-behavior findings behind blockers.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py`, `scripts/check_security_hardening_supply_chain_audit.py`, `scripts/check_security_hardening_runtime_hardening.py`.
- First entrypoints/functions to inspect: `build_objc3c_security_posture.main`, `check_security_hardening_supply_chain_audit.main`, `check_security_hardening_runtime_hardening.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `scripts/check_security_hardening_supply_chain_audit.py` and `scripts/check_security_hardening_runtime_hardening.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:security-hardening:e2e`, `npm run check:objc3c:security-hardening:surface`, `npm run inspect:objc3c:security-posture` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:security-hardening:e2e`
- `npm run check:objc3c:security-hardening:surface`
- `npm run inspect:objc3c:security-posture`

## GitHub Tracking
- Stable draft ID: `OC3-N34-002`
- Intended milestone title: `OC3-N34 Memory Safety UB And Runtime Sanitizer Audits`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N35-001: Generate docs pages from support and conformance manifests

- Suggested milestone: OC3-N35 Docs Site And Claim Synchronization
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:docs`, `area:governance`
- Draft blocked by: OC3-T01-002 (docs sync depends on claim drift gates)
- Draft blocks: OC3-N35-002 (same-milestone sequencing)

## Objective
[OC3-N35-001] Generate docs pages from support and conformance manifests.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `docs/`, `site/src/`, `scripts/build_objc3c_native_docs.py`, `scripts/build_pages.py`, `scripts/check_documentation_surface.py`, `README.md`.
- First entrypoints/functions to inspect: `build_objc3c_native_docs.main`, `build_pages.main`, `check_documentation_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `docs/`, `site/src/`, `scripts/build_objc3c_native_docs.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `scripts/check_documentation_surface.py` and `README.md` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:docs`, `npm run build:site`, `npm run check:docs:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:docs`
- `npm run build:site`
- `npm run check:docs:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N35-001`
- Intended milestone title: `OC3-N35 Docs Site And Claim Synchronization`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N35-002: Gate public docs against implementation support levels

- Suggested milestone: OC3-N35 Docs Site And Claim Synchronization
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:docs`, `area:governance`
- Draft blocked by: OC3-N35-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N35-002] Gate public docs against implementation support levels.

## Implementation Scope
- Implement [OC3-N35-002] Gate public docs against implementation support levels with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `docs/`, `site/src/`, `scripts/build_objc3c_native_docs.py`, `scripts/build_pages.py`, `scripts/check_documentation_surface.py`, `README.md`.
- First entrypoints/functions to inspect: `build_objc3c_native_docs.main`, `build_pages.main`, `check_documentation_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `docs/`, `site/src/`, `scripts/build_objc3c_native_docs.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N35-002] Gate public docs against implementation support levels with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/check_documentation_surface.py` and `README.md` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:docs`, `npm run build:site`, `npm run check:docs:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:docs`
- `npm run build:site`
- `npm run check:docs:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N35-002`
- Intended milestone title: `OC3-N35 Docs Site And Claim Synchronization`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N36-001: Expand canonical applications for object model, interop, and concurrency

- Suggested milestone: OC3-N36 Showcase And Canonical Application Expansion
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:applications`, `area:ux`
- Draft blocked by: OC3-T17-003 (showcases depend on runnable stdlib packages)
- Draft blocks: OC3-N36-002 (same-milestone sequencing)

## Objective
[OC3-N36-001] Expand canonical applications for object model, interop, and concurrency.

## Implementation Scope
- Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include header import/export, ABI alignment, foreign type diagnostics, mixed-image loading, and packaged execution cases.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N36-001`
- Intended milestone title: `OC3-N36 Showcase And Canonical Application Expansion`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N36-002: Add showcase smoke tests and reproducible demo packages

- Suggested milestone: OC3-N36 Showcase And Canonical Application Expansion
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:applications`, `area:ux`
- Draft blocked by: OC3-N36-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N36-002] Add showcase smoke tests and reproducible demo packages.

## Implementation Scope
- Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- Primary source references: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `tests/tooling/fixtures/objc3c/`.
- First entrypoints/functions to inspect: `objc3c::sema::SemaPassManager`, `RunSemanticPasses`, `ResolveGlobalInitializerValues`, `Objc3IREmitter`, `RuntimeMetadataValueType`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`, `native/objc3c/src/sema/objc3_semantic_passes.cpp`, `native/objc3c/src/sema/objc3_static_analysis.cpp` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include lockfile determinism, offline mirror verification, tamper rejection, and mixed-version compatibility cases.
- [ ] Add positive fixtures under `native/objc3c/src/ir/objc3_ir_emitter.cpp` and `tests/tooling/fixtures/objc3c/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:execution-replay-proof`, `npm run test:objc3c:lowering-runtime-stress`, `npm run test:objc3c:full` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:execution-replay-proof`
- `npm run test:objc3c:lowering-runtime-stress`
- `npm run test:objc3c:full`

## GitHub Tracking
- Stable draft ID: `OC3-N36-002`
- Intended milestone title: `OC3-N36 Showcase And Canonical Application Expansion`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N37-001: Implement ABI/API diff blockers

- Suggested milestone: OC3-N37 API ABI Compatibility Governance
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:abi`, `area:governance`
- Draft blocked by: OC3-T20-001 (ABI governance depends on versioned public runtime ABI)
- Draft blocks: OC3-N37-002 (same-milestone sequencing)

## Objective
[OC3-N37-001] Implement ABI/API diff blockers.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N37-001`
- Intended milestone title: `OC3-N37 API ABI Compatibility Governance`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N37-002: Implement compatibility and deprecation-window governance

- Suggested milestone: OC3-N37 API ABI Compatibility Governance
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:abi`, `area:governance`
- Draft blocked by: OC3-N37-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N37-002] Implement compatibility and deprecation-window governance.

## Implementation Scope
- Implement [OC3-N37-002] Implement compatibility and deprecation-window governance with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py`, `scripts/check_release_evidence.py`, `packaging/`, `reports/`.
- First entrypoints/functions to inspect: `build_objc3c_release_manifest.main`, `publish_objc3c_release_provenance.main`, `check_release_evidence.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/build_objc3c_release_manifest.py`, `scripts/publish_objc3c_release_provenance.py`, `scripts/build_objc3c_update_manifest.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N37-002] Implement compatibility and deprecation-window governance with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `packaging/` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-release-candidate`, `npm run test:objc3c:release-operations:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-release-candidate`
- `npm run test:objc3c:release-operations:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N37-002`
- Intended milestone title: `OC3-N37 API ABI Compatibility Governance`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N38-001: Build external reproducibility corpus

- Suggested milestone: OC3-N38 External Validation Partners And Repro Corpus
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:validation`, `area:adoption`
- Draft blocked by: OC3-T18-002 (external validation depends on traceable evidence)
- Draft blocks: OC3-N38-002 (same-milestone sequencing)

## Objective
[OC3-N38-001] Build external reproducibility corpus.

## Implementation Scope
- Implement [OC3-N38-001] Build external reproducibility corpus with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py`, `scripts/check_conformance_corpus_surface.py`, `scripts/check_objc3c_public_conformance_reporting_end_to_end.py`, `reports/`.
- First entrypoints/functions to inspect: `generate_conformance_corpus_index.main`, `generate_conformance_evidence_index.main`, `check_conformance_corpus_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N38-001] Build external reproducibility corpus with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/check_objc3c_public_conformance_reporting_end_to_end.py` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-conformance-corpus`, `npm run test:objc3c:public-conformance:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-conformance-corpus`
- `npm run test:objc3c:public-conformance:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N38-001`
- Intended milestone title: `OC3-N38 External Validation Partners And Repro Corpus`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N38-002: Gate support claims on external validation evidence

- Suggested milestone: OC3-N38 External Validation Partners And Repro Corpus
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:validation`, `area:adoption`
- Draft blocked by: OC3-N38-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N38-002] Gate support claims on external validation evidence.

## Implementation Scope
- Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- Primary source references: `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py`, `scripts/check_conformance_corpus_surface.py`, `scripts/check_objc3c_public_conformance_reporting_end_to_end.py`, `reports/`.
- First entrypoints/functions to inspect: `generate_conformance_corpus_index.main`, `generate_conformance_evidence_index.main`, `check_conformance_corpus_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `tests/conformance/README.md`, `scripts/generate_conformance_corpus_index.py`, `scripts/generate_conformance_evidence_index.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Map every new public claim to executable evidence and keep unsupported draft surfaces explicitly fail-closed.
- [ ] Add positive fixtures under `scripts/check_objc3c_public_conformance_reporting_end_to_end.py` and `reports/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:runnable-conformance-corpus`, `npm run test:objc3c:public-conformance:e2e`, `npm run check:release-evidence` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:runnable-conformance-corpus`
- `npm run test:objc3c:public-conformance:e2e`
- `npm run check:release-evidence`

## GitHub Tracking
- Stable draft ID: `OC3-N38-002`
- Intended milestone title: `OC3-N38 External Validation Partners And Repro Corpus`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N39-001: Implement language/runtime threat model and mitigation backlog

- Suggested milestone: OC3-N39 Security Threat Model And Macro Supply Chain
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:security`, `area:supply-chain`, `area:macros`
- Draft blocked by: OC3-T15-001 (macro supply-chain depends on macro host model)
- Draft blocks: OC3-N39-002 (same-milestone sequencing)

## Objective
[OC3-N39-001] Implement language/runtime threat model and mitigation backlog.

## Implementation Scope
- Implement [OC3-N39-001] Implement language/runtime threat model and mitigation backlog with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py`, `scripts/check_security_hardening_supply_chain_audit.py`, `scripts/check_security_hardening_runtime_hardening.py`.
- First entrypoints/functions to inspect: `build_objc3c_security_posture.main`, `check_security_hardening_supply_chain_audit.main`, `check_security_hardening_runtime_hardening.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N39-001] Implement language/runtime threat model and mitigation backlog with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `scripts/check_security_hardening_supply_chain_audit.py` and `scripts/check_security_hardening_runtime_hardening.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:security-hardening:e2e`, `npm run check:objc3c:security-hardening:surface`, `npm run inspect:objc3c:security-posture` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:security-hardening:e2e`
- `npm run check:objc3c:security-hardening:surface`
- `npm run inspect:objc3c:security-posture`

## GitHub Tracking
- Stable draft ID: `OC3-N39-001`
- Intended milestone title: `OC3-N39 Security Threat Model And Macro Supply Chain`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N39-002: Implement macro supply-chain sandbox, signing, and revocation

- Suggested milestone: OC3-N39 Security Threat Model And Macro Supply Chain
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:security`, `area:supply-chain`, `area:macros`
- Draft blocked by: OC3-N39-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N39-002] Implement macro supply-chain sandbox, signing, and revocation.

## Implementation Scope
- Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- Primary source references: `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py`, `scripts/check_security_hardening_supply_chain_audit.py`, `scripts/check_security_hardening_runtime_hardening.py`.
- First entrypoints/functions to inspect: `build_objc3c_security_posture.main`, `check_security_hardening_supply_chain_audit.main`, `check_security_hardening_runtime_hardening.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `spec/governance/macro_package_provenance_policy_v1.md`, `spec/governance/macro_security_incident_playbook_v1.md`, `scripts/build_objc3c_security_posture.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include expansion provenance, cache invalidation, sandbox policy, deterministic output, and actionable expansion diagnostics.
- [ ] Add deny-by-default tests, policy artifacts, signing/revocation behavior, and incident-response documentation.
- [ ] Add positive fixtures under `scripts/check_security_hardening_supply_chain_audit.py` and `scripts/check_security_hardening_runtime_hardening.py` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:security-hardening:e2e`, `npm run check:objc3c:security-hardening:surface`, `npm run inspect:objc3c:security-posture` plus the narrowest changed-file unit tests.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:security-hardening:e2e`
- `npm run check:objc3c:security-hardening:surface`
- `npm run inspect:objc3c:security-posture`

## GitHub Tracking
- Stable draft ID: `OC3-N39-002`
- Intended milestone title: `OC3-N39 Security Threat Model And Macro Supply Chain`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N40-001: Implement GitHub issue publisher with labels, milestones, and blockers

- Suggested milestone: OC3-N40 Maintainer Operations And Issue Publishing Automation
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:workflow`, `area:tooling`, `area:governance`
- Draft blocked by: OC3-T01-001 (publisher automation depends on stable support/draft IDs)
- Draft blocks: OC3-N40-002 (same-milestone sequencing)

## Objective
[OC3-N40-001] Implement GitHub issue publisher with labels, milestones, and blockers.

## Implementation Scope
- Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Include stack-to-heap promotion, `__block` forwarding cells, copy/dispose helper emission, and captured object lifetimes.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N40-001`
- Intended milestone title: `OC3-N40 Maintainer Operations And Issue Publishing Automation`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

### OC3-N40-002: Implement published issue drift audit and reference updater

- Suggested milestone: OC3-N40 Maintainer Operations And Issue Publishing Automation
- Suggested labels: `compiler-impl`, `type:roadmap`, `source:planning-checklist`, `publication:internal-first`, `kind:implementation`, `priority:P1`, `area:workflow`, `area:tooling`, `area:governance`
- Draft blocked by: OC3-N40-001 (same-milestone sequencing)
- Draft blocks: none

## Objective
[OC3-N40-002] Implement published issue drift audit and reference updater.

## Implementation Scope
- Implement [OC3-N40-002] Implement published issue drift audit and reference updater with source changes, runnable fixtures, durable evidence, and release-claim gating.
- Primary source references: `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py`, `native/objc3c/src/diag/objc3_diag_utils.cpp`, `tests/tooling/`.
- First entrypoints/functions to inspect: `objc3c_public_workflow_runner.main`, `format_objc3c_source.main`, `build_objc3c_editor_tooling_surface.main`.

## Non-Goals
- Do not satisfy this issue with prose-only, dashboard-only, checklist-only, or manifest-only changes.
- Do not add source-of-truth data under `tmp/`; use durable repository paths and generated reports that can be reproduced.
- Do not widen public support claims until the executable evidence and claim gate are updated in the same work.

## Concrete Sub-Issues
- [ ] Update `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_editor_tooling_surface.py`, `scripts/format_objc3c_source.py` for the implementation path; do not close this with documentation-only or manifest-only changes.
- [ ] Implement [OC3-N40-002] Implement published issue drift audit and reference updater with source changes, runnable fixtures, durable evidence, and release-claim gating.
- [ ] Add positive fixtures under `native/objc3c/src/diag/objc3_diag_utils.cpp` and `tests/tooling/` that compile or run through the public objc3c workflow.
- [ ] Add negative fixtures for malformed, unsupported, conflicting, or unsafe variants with stable diagnostic codes and source ranges.
- [ ] Thread the feature through durable evidence output under reports/ or the relevant support/conformance manifest; never use tmp/ as a source of truth.
- [ ] Add replay/determinism coverage so repeated runs produce stable diagnostics, manifests, IR/object metadata, and runtime output.
- [ ] Update affected docs only after executable proof exists, and make docs state partial/unsupported surfaces explicitly.
- [ ] Remove obsolete scaffolding, helper-only fallback, stale TODO, or fail-open behavior made obsolete by the implementation.
- [ ] Run and record `npm run test:objc3c:developer-tooling`, `npm run test:objc3c:runnable-developer-tooling`, `npm run check:objc3c:public-conformance:surface` plus the narrowest changed-file unit tests.
- [ ] Close the issue only when the implementation, fixtures, evidence manifest, documentation, and validation output are all in the closing PR or commit series.

## Acceptance Criteria
- The feature is implemented in durable source code or durable tooling, not just planning prose.
- At least one positive and one negative executable fixture exists unless the issue is explicitly release/tooling-only.
- Public claims are backed by generated support/conformance evidence and unsupported surfaces remain fail-closed.
- Validation commands listed in the issue pass locally or the closing notes explain a concrete external blocker.
- No source-of-truth input, generated publication state, or evidence manifest depends on `tmp/`.
- The implementation can be reviewed and closed independently without needing an unstated preparatory issue.

## Validation Commands
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`
- `npm run check:objc3c:public-conformance:surface`

## GitHub Tracking
- Stable draft ID: `OC3-N40-002`
- Intended milestone title: `OC3-N40 Maintainer Operations And Issue Publishing Automation`
- Dependencies should be represented with GitHub `blocked_by` relationships after publication, not by manually guessing issue numbers before creation.
- If GitHub assigns different milestone/issue numbers than expected, update durable mapping reports rather than renumbering the draft IDs.

