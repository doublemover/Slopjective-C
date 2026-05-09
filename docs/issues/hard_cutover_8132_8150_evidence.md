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
- `tests/conformance/hard_cutover_acceptance_area_owners.json`
- `tests/conformance/hard_cutover_positive_residue_audit.json`

Retired hard-cutover surfaces are documented this way:

- old-mode literals: parser/e2e rejection fixtures with `O3C002`
- removed compatibility mode flag: parser rejection fixture
- removed parser fallback flag: parser rejection fixture
- compatibility shim gate: semantic rejection fixture
- runtime dispatch fallback: lowering, IR, runtime, and e2e strict-error fixtures
- migration lane as behavior support: absent from public support and not a
  positive fixture class

Acceptance area ownership is now indexed in
`tests/conformance/hard_cutover_acceptance_area_owners.json`. The residue audit
in `tests/conformance/hard_cutover_positive_residue_audit.json` records that the
remaining positive-fixture lexical hits for `fallback`, `shim`, and `migrator`
are ordinary variable/function/symbol names or diagnostic inventory labels, not
old-mode, compatibility-shim, fallback-dispatch, or migration-lane acceptance.

Additional local commits folded into this evidence map after the first index
pass:

- `645e9c25f` indexes issue evidence and retired-surface absence for `#8144`
  and `#8150`.
- `7e743549d` adds parser recovery diagnostics and AST type owner evidence for
  `#8132`, `#8134`, and `#8146`.
- `062da3dbc`, `1da2a515b`, `68f793396`, and `3c335bfae` add IR
  serialization, synthesized-property accessor, control-flow lowering, and
  runtime-helper owner evidence for `#8137` and `#8147`.
- `ffb2a715d` adds config truth-table evidence for `#8138` and `#8148`.
- `e435cdea9` and `e04c6cf4a` add parser contract/shard evidence for `#8134`.
- `8dead58b3` adds diagnostic catalog table evidence for `#8135` and `#8145`.
- `239871c25` adds tracker-ready closeout payloads for `#8132`-`#8150`.
- `4b5ca540d` and `94c0f0709` add parser expression and statement node
  owner evidence for `#8134`.
- `dae4e84b4` adds lower control-flow contract evidence for `#8136` and
  `#8147`.
- `6a9b0b23f` adds class graph rebuild owner evidence for `#8133` and
  `#8143`.
- `f0ba25851` adds public API ownership contract evidence for `#8141` and
  `#8145`.
- `db4fe91cf` adds JSON schema support owner evidence for `#8148`.
- `c64fe1597` adds frontend header ownership boundary evidence for `#8140`,
  `#8141`, and `#8146`.
- `f35a96292` adds the refreshed closeout payload index for `#8144` and
  `#8150`.
- `31da18e08` adds frontend C API runner result-owner evidence for `#8140` and
  `#8141`.
- `25bde4f7f` adds native contract owner surface evidence for `#8138`,
  `#8141`, and `#8148`.
- `5788dcf91` folds those latest local commits into the issue evidence maps for
  `#8144` and `#8150`.
- `ebe18944b` adds method-resolution owner evidence for `#8133` and `#8143`.
- `99cab5b8a` adds frontend C API runner option-owner evidence for `#8140`,
  `#8141`, and `#8142`.
- `6063e4f32` adds README support onboarding evidence for `#8145`.
- `4653e8d07` adds property support owner modules and carried the prior issue
  evidence fold-in for `#8138`, `#8144`, and `#8150`.
- `c1c990e16` adds JSON schema validation owner evidence for `#8148`.
- `b1f1ba7a7` adds frontend C API runner command-owner evidence for `#8140`,
  `#8141`, and `#8142`.
- `b0e6a33a5` adds artifact JSON publication contract evidence for `#8138`
  and `#8148`.
- `ed3273640` adds Objective-C type support profile evidence for `#8138` and
  `#8146`.
- `12787c606` adds receiver support profile evidence for `#8138` and `#8146`.
- `1897efd7a` adds runtime registration owner evidence for `#8133` and
  `#8143`.
- `faf7363ae` adds frontend C API runner session-owner evidence for `#8140`
  and `#8141`.
- `1854669de` adds artifact schema contract table evidence for `#8138` and
  `#8148`.
- `2f1a954ad` adds frontend pipeline stage contract evidence for `#8138` and
  `#8140`.
- `be79d15ce` renames stdlib text shape surfaces as docs/support evidence for
  `#8145`.
- `e23236ea0` adds concurrency support profile owner evidence for `#8138` and
  `#8147`.
- `5354a91b0` adds config feature-state owner evidence for `#8138` and
  `#8148`.
- `e74371e88` adds frontend C API runner output-contract owner evidence for
  `#8140` and `#8141`.
- `3334f4a59` adds checked dispatch owner evidence for `#8133` and `#8143`.
- `9bbd9a143` adds diagnostic catalog contract evidence for `#8135` and
  `#8145`.
- `106006a74` adds canonical acceptance-area ownership and positive-residue
  audit indexes for `#8144` and `#8150`.
- `202409388`, `6f529ddfd`, `ef2b4e7ad`, `e63ed1189`, `a8a4f9f34`,
  `d7d299698`, `8ceaf5917`, `d713e3655`, and `2728c61d5` extend the
  acceptance-area evidence across artifacts, property/accessor support, driver
  runtime registration commands, contract helpers, stdlib docs, native contract
  IDs, JSON helpers, public result mapping, and drained include-shard removal.

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
Per-issue tracker-ready closeout notes live in
`docs/issues/hard_cutover_8132_8150_closeout/payloads.md`.
The machine-readable closeout lookup is
`docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`.
