# Hard-Cutover Issue Evidence: #8132-#8150

This is a local closeout evidence map for the hard-cutover branch. It is based
on committed branch work as of 2026-05-09. No validation, GitHub commands, push,
or remote issue updates were run while writing this artifact.

Canonical behavior and no-compatibility indexes:

- `tests/conformance/hard_cutover_issue_index.json`
- `tests/conformance/hard_cutover_retired_surface_absence.json`
- `tests/conformance/hard_cutover_catalog.json`
- `tests/fixtures/canonical/manifest.json`
- `tests/native/retired_surface_matrix.json`
- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`

Retired hard-cutover surfaces are documented this way:

- old-mode literals: parser/e2e rejection fixtures with `O3C002`
- removed compatibility mode flag: parser rejection fixture
- removed parser fallback flag: parser rejection fixture
- compatibility shim gate: semantic rejection fixture
- runtime dispatch fallback: lowering, IR, runtime, and e2e strict-error fixtures
- migration lane as behavior support: absent from public support and not a
  positive fixture class

| Issue | Evidence Status | Local Evidence Summary |
| --- | --- | --- |
| `#8132` | evidence-ready | Compiler architecture decomposition is indexed from root, frontend, driver, AST, IR, pipeline, ownership, and schema split commits. |
| `#8133` | evidence-ready | Runtime strict typed dispatch is indexed from dispatch result, selector/keypath/cache/state, and wrapper commits; runtime fallback remains strict-error evidence. |
| `#8134` | evidence-ready | Parser, lexer, token, and AST ownership splits are indexed; old-mode and parser fallback flags are rejection fixtures. |
| `#8135` | evidence-ready | Semantic owner splits are tied to typed-flow, unsupported-feature, and compatibility-shim rejection fixtures. |
| `#8136` | evidence-ready | Lowering owner splits are tied to strict runtime-dispatch and removed fallback fixtures. |
| `#8137` | evidence-ready | IR emitter, message-send validation, and runtime metadata splits are tied to canonical IR fixture evidence. |
| `#8138` | evidence-ready | Pipeline, IO, artifact, and config splits are indexed as internal ownership evidence, not public compatibility support. |
| `#8139` | evidence-ready | Native target-family splits are indexed as internal topology evidence. |
| `#8140` | evidence-ready | Driver, frontend, and runner splits are indexed without direct helper entrypoints becoming public commands. |
| `#8141` | evidence-ready | Public C API result/string/diagnostic/artifact ownership is indexed as internal contract truth. |
| `#8142` | evidence-ready | Workflow command-surface evidence is indexed around `npm run objc3c -- <action>`; direct helpers, aliases, and registry facades are retired. |
| `#8143` | evidence-ready | Runtime acceptance split evidence is indexed across acceptance domains and strict runtime behavior fixtures. |
| `#8144` | evidence-ready | Behavior-first fixtures now include canonical, retired-surface, absence, and issue evidence maps. |
| `#8145` | evidence-ready | Capability docs and evidence maps explicitly reject shim, fallback, migration-lane, and compatibility-mode support claims. |
| `#8146` | evidence-ready | Frontend type surfaces are indexed against canonical typed-flow fixture evidence. |
| `#8147` | evidence-ready | Deep sema/lowering/runtime metadata split evidence is indexed with canonical ownership and strict unsupported-feature behavior. |
| `#8148` | evidence-ready | JSON/schema infrastructure evidence is indexed, including schema registry id normalization. |
| `#8149` | evidence-ready | Source hygiene and command-surface guardrails are indexed with allowlist/report-only surfaces retired. |
| `#8150` | local-evidence-ready-not-remotely-closed | Closure evidence is local. Remote issue closure, push, and validation are deferred by current constraints. |

Primary local commit evidence is enumerated per issue in
`docs/issues/hard_cutover_8132_8150_evidence.json` and mirrored into
`tests/conformance/hard_cutover_issue_index.json` for test/fixture ownership.
