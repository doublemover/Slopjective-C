# Hard-Cutover Behavior Outcome Owner Index

This is the human-readable companion to
`tests/conformance/hard_cutover_behavior_outcome_owner_index.json`. It is local
evidence only; no validation, GitHub edits, push, build, generator, formatter,
lint, npm, cmake, or test command was run while preparing it.

The index groups hard-cutover evidence by result: canonical support, rejection,
strict error, generated provenance, residue-audit disposition, or issue
closeout. This keeps support claims separate from evidence artifacts that only
prove absence or traceability.

| Outcome | Kind | Evidence Owner | Boundary |
| --- | --- | --- | --- |
| Canonical positive behavior | positive | `tests/fixtures/canonical/manifest.json` and native positive fixtures | supported behavior is canonical Objective-C 3 only |
| Phase owner contracts | phase-owned behavior authority | `tests/conformance/hard_cutover_behavior_phase_owner_contracts.json` | phase support claims resolve to hand-authored native fixture families, not generated artifacts |
| Parser rejection | rejection | parser negative fixtures and retired-surface indexes | legacy literals and removed parser flags reject old-mode/fallback surfaces |
| Semantic rejection | rejection | sema errors and acceptance-area owner index | compatibility-shim and unsupported-feature claims are diagnostics |
| Lowering or link strict error | strict-error | lowering/IR strict-error fixtures | runtime fallback lowering remains removed or unresolved, not supported |
| Runtime strict error | strict-error | runtime dispatch/error fixtures | runtime dispatch fallback and unresolved symbols are strict errors |
| Negative execution | negative-execution | e2e negative execution fixtures | retired behavior remains rejected at runnable boundaries |
| Generated provenance only | provenance-only | generated manifest and objc3c contract artifacts | generated artifacts do not define behavior support |
| Positive residue false positive | audit-disposition | positive residue audit | lexical hits are identifiers or inventory labels, not support claims |
| Issue closeout only | closeout-index | docs/issues evidence and payload indexes | issue docs summarize traceability; they do not create support |
| Latest local owner refresh | closeout-index | `docs/issues/hard_cutover_latest_local_commit_refresh.md` | post-outcome local commits refresh owner traceability only; validation and remote closure remain deferred |

Outcome ownership is deliberately stricter than file ownership. A fixture may
live under a broad behavior family, but the expected result determines whether
it can support a public capability, reject a retired surface, document strict
failure, or serve only as generated provenance.
