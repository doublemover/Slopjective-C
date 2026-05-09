# Hard-Cutover Tracker Closeout Payloads

These are local, tracker-ready notes for `#8132`-`#8150`. They do not assert
remote closure. Validation, `gh`, push, and issue edits were intentionally not
run while preparing them.

Post-payload refresh commits are indexed in
`docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`. The refresh
set folds in parser expression/statement node owners, lower control-flow
contracts, runtime class graph rebuild ownership, public API ownership contract
clarifications, and JSON schema support owners without changing the no-validation
or no-GitHub status of these payloads.

The latest docs/issues-only refresh is
`docs/issues/hard_cutover_latest_local_commit_refresh.md`. It folds in 90 local
owner commits after `abc203478` through `6d6fa804d`, grouped by issue
acceptance owner, without changing the no-validation, no-GitHub, no-push,
local-only status of these payloads.

## Latest Local Owner Refresh

| Issue Area | Local Commits | Closeout Meaning |
| --- | --- | --- |
| `#8132`, `#8134`, `#8146`, `#8147` | `f1df8b644`, `86c31c6dc`, `f5619d174`, `11c20dec5`, `18e1f247b`, `cd5358bf6`, `1e1f314a8`, `bcf43808c`, `388a72716`, `fb54d008b`, `e1576ff03`, `64bf96d9b`, `578ade056`, `6a2b0336f`, `df2577005` | Parser, AST, ObjC reference, type, and frontend surfaces have newer owner evidence; retired parser/old-mode surfaces remain rejection evidence. |
| `#8136`, `#8137`, `#8147` | `399984eeb`, `c7339d28b`, `8e9465994` | Lowering handoff, message-send lowering, and deterministic IR publication have newer owner evidence; runtime fallback remains removed or strict-error behavior. |
| `#8133`, `#8143`, `#8147` | `8e7c9282d`, `87843840e`, `c11f3f403`, `bfbd99e34`, `77b4993cb`, `5767392ca`, `b1f019d23`, `236ff7a40`, `d0c187589`, `62247aec2`, `a16fd3725`, `bbf4a35da`, `f6366fb68`, `043a855c6`, `5aa53baa5`, `869c7aa51`, `dead8d47f`, `476b54e16`, `8c500be1b`, `a9675d948`, `166f0d1d6`, `17617d941`, `2f0ef73a4`, `f4a067c57`, `377d2abbc`, `f03cba094`, `3dcf928fe`, `9cf3601b5`, `4307f5156` | Runtime error, state, concurrency, block, storage, ARC, selector/keypath, and snapshot owners have newer evidence; dispatch fallback stays strict-error evidence. |
| `#8138`, `#8140`, `#8141`, `#8148` | `295b34b5a`, `a1d25ca68`, `3b1b9e789`, `8550309ea`, `3a14d3d9a`, `7cdb5e824`, `19b753126`, `f0f063934`, `13269c328`, `5cc21d8b1`, `54026487c`, `e43df52d1`, `d6d0cb785`, `c89daee3d`, `0ef0131d3`, `17ce89a87`, `4219dd9e9`, `2af7ffd1b`, `a7a353c87`, `8e9465994`, `3dcf928fe`, `c8060c3e3`, `531b53843`, `6d6fa804d` | Driver, frontend, publication, public C API, config, contracts, pipeline, and JSON/schema surfaces have newer owner evidence; none create helper-command or compatibility support. |
| `#8135`, `#8142`, `#8144`, `#8145`, `#8149`, `#8150` | `5af6c1b64`, `f1f2d999f`, `fda259576`, `372de733d`, `2b62a9872`, `d76f9e53a`, `e426ab91d`, `0ef6dd41f`, `8ec96d428`, `71d3e8c4c`, `2b4b66526`, `01a58e0ab`, `0da6806ec`, `4b41eeefc`, `9d337d188`, `7dc527d4e`, `ffe9b387d`, `0fb5ce0a0`, `699408fb7`, `a7a353c87`, `a16fd3725`, `1f419a98c`, `beeb1b22c`, `531b53843`, `c8060c3e3`, `3d90deeaf` | Support helpers, diagnostics, stdlib/support truth, retired fixture contracts, workflow, hygiene, and control-plane surfaces have newer local evidence; final closure still waits on validation, push, and remote issue updates. |

## #8132

Compiler architecture decomposition is indexed from local commits covering root
target topology, frontend/driver targets, AST ownership, IR/pipeline ownership,
native ownership/schema surfaces, and driver/C API runner owners.

Evidence: `tests/conformance/hard_cutover_issue_index.json`,
`docs/support/capability_matrix.json`, commits `1877caeb7`, `df28562af`,
`87f958c8c`, `d61aad21e`, `bd3905d5c`, `28a65a27a`, `fb443536f`,
`7e743549d`.

Retired-surface state: internal architecture evidence only; no shim, fallback,
compatibility mode, or migration lane is claimed as public behavior.

## #8133

Strict typed runtime dispatch is indexed from local dispatch-result, selector,
keypath, cache, state, and wrapper ownership commits. Runtime fallback behavior
is represented as strict-error fixture evidence.

Evidence: `tests/native/runtime/dispatch/message_send_runtime_dispatch.objc3`,
`tests/native/e2e/negative_execution/runtime_dispatch_unknown_receiver_strict_error.objc3`,
commits `d8552e07a`, `d53083a92`, `6d10280d0`, `790be44b6`, `8c35392ea`,
`228d345b9`, `61ccee955`.

Retired-surface state: runtime dispatch fallback is strict-error behavior, not a
positive acceptance lane.

## #8134

Parser, lexer, token, and AST ownership split evidence is indexed from parser
facade, expression/statement, message-send profile, C-style type, attribute,
contract fingerprint, recovery diagnostic, and parse-namespace commits.

Evidence: parser positive and negative fixtures under `tests/native/parser/`,
commits `28a65a27a`, `b0f031ef5`, `87f9a36af`, `29111e0c3`, `2d70b8e34`,
`1a130de7a`, `a91172c62`, `ffca53ae0`, `1d135dbf5`, `7e743549d`,
`e435cdea9`, `e04c6cf4a`.

Retired-surface state: old-mode and parser fallback flags are explicit rejection
fixtures.

## #8135

Semantic analysis owner evidence is indexed from sema pass helper, equivalence,
publication, frontend ownership, and diagnostic catalog table commits.

Evidence: `tests/native/sema/types/typed_i32_bool_flow.objc3`,
`tests/native/sema/errors/removed_compatibility_shim_gate_rejected.objc3`,
`tests/native/sema/concurrency/throws_feature_claim_rejected.objc3`, commits
`121c069aa`, `8b103d250`, `112256a5a`, `232997ba4`, `8dead58b3`.

Retired-surface state: compatibility shim and unsupported feature claims are
semantic rejection evidence.

## #8136

Lowering owner split evidence is indexed from runtime-call lowering, dispatch
ARC contracts, backend handoff, lower/IR dispatch, block validator, and IR
control-flow lowering commits.

Evidence: `tests/native/lowering/errors/runtime_dispatch_requires_link_strict_error.objc3`,
`tests/native/lowering/errors/removed_runtime_dispatch_fallback_flag_rejected.objc3`,
commits `73ce6b57f`, `f69c6388f`, `5464aa831`, `f42bd035f`, `668456c12`,
`68f793396`.

Retired-surface state: runtime fallback lowering is rejection or strict-error
evidence only.

## #8137

IR ownership split evidence is indexed from emitter, message-send validation,
runtime metadata emission, surface serialization, synthesized property accessor,
control-flow lowering, and runtime-helper owner commits.

Evidence: `tests/native/ir/module/basic_i32_return_main.objc3`,
`tests/native/ir/runtime_calls/non_nil_receiver_runtime_call_contract.objc3`,
commits `17f74aff3`, `183d5ff4b`, `aa410fda7`, `bd3905d5c`, `1da2a515b`,
`062da3dbc`, `68f793396`, `3c335bfae`.

Retired-surface state: unresolved runtime helper and dispatch calls remain
strict-error evidence when unavailable.

## #8138

Pipeline, IO, artifact, and config owner split evidence is indexed from
pipeline classification, IO/artifact support, artifact publication, config
diagnostics, pipeline/IO artifact contracts, and config truth-table commits.

Evidence: `tests/conformance/hard_cutover_issue_index.json`,
`docs/support/evidence_map.json`, commits `4c5fbe849`, `7a5412e51`,
`5fb992332`, `fd22db929`, `65abde135`, `feb4cee08`, `d57506a67`,
`ffb2a715d`.

Retired-surface state: internal support ownership does not create public
fallback or migration support.

## #8139

Native target-family evidence is indexed from root, frontend/driver, config,
CLI, support, and AST target split commits.

Evidence: `docs/support/capability_matrix.json`, `docs/support/evidence_map.json`,
commits `1877caeb7`, `df28562af`, `445494349`, `2e6b613b8`, `1e26582d3`,
`87f958c8c`.

Retired-surface state: target topology is internal and exposes no public
fallback lane.

## #8140

Driver, frontend, and runner split evidence is indexed from frontend/driver,
C API runner, result accessor, ADR, basic artifact publication, and driver/C API
runner owner commits.

Evidence: `tests/conformance/hard_cutover_issue_index.json`,
`docs/support/evidence_map.json`, commits `c576b6a22`, `c18135fcc`,
`d8bbebb2b`, `4e6046642`, `93ae37eab`, `fb443536f`, `232997ba4`.

Retired-surface state: direct helper entrypoints are not public command support.

## #8141

Public C API ownership evidence is indexed from C API ownership coverage,
frontend C API result accessor, runner owner, sema/frontend ownership, driver/C
API runner, and edge-case contract rename commits.

Evidence: `docs/support/capability_matrix.json`,
`docs/support/evidence_map.json`, commits `a51cac68d`, `d8bbebb2b`,
`c18135fcc`, `232997ba4`, `fb443536f`, `99f6692d5`.

Retired-surface state: C API evidence is strict contract truth, not a
compatibility wrapper.

## #8142

Workflow command-surface evidence is indexed around the single public npm bridge
and the retirement of direct helpers, package-script aliases, and registry
facades.

Evidence: `docs/workflows/commands.md`, `docs/support/capability_matrix.json`,
commits `b3401be67`, `d86f03a25`, `72b62d7ed`, `d5668d12e`, `5c025aa97`,
`3c4d6c973`, `6fef331c2`, `a612ccede`, `72919dc66`, `37e788a6b`,
`5ed60a087`.

Retired-surface state: public command truth is `npm run objc3c -- <action>`.

## #8143

Runtime acceptance split evidence is indexed from acceptance helpers and
acceptance-domain commits covering error, concurrency, interop, object model,
registration, storage reflection, and runtime package surfaces.

Evidence: `tests/tooling/runtime/README.md`,
`tests/native/runtime/dispatch/message_send_runtime_dispatch.objc3`,
`tests/native/e2e/negative_execution/runtime_dispatch_unknown_receiver_strict_error.objc3`,
commits `aeb02602b`, `6d03f1f01`, `4213ace21`, `eec3b5cb6`, `c5f3f933a`,
`1146e503f`, `eb25dbc34`, `3196d7571`, `2341a9bc4`, `93d70f907`.

Retired-surface state: unsupported dispatch remains strict-error runtime
behavior.

## #8144

Behavior-first fixture evidence is indexed from behavior catalog, cutover
boundary, strict fixture, wording, removed-mode fixture, retired-surface matrix,
and issue evidence commits.

Evidence: `tests/native/README.md`, `tests/native/retired_surface_matrix.json`,
`tests/conformance/hard_cutover_issue_index.json`,
`tests/conformance/hard_cutover_retired_surface_absence.json`, commits
`aee256872`, `e5b988129`, `99b93ccb2`, `e99c7d740`, `67aa58ad9`,
`16e517982`, `e157ebe20`, `e02bde405`, `645e9c25f`.

Retired-surface state: all retired behavior is rejection, strict-error, or
absent from support.

## #8145

Capability truth evidence is indexed from capability docs, command bridge docs,
schema truth docs, evidence-map tie-ins, support truth, and diagnostic catalog
table commits.

Evidence: `docs/support/capability_matrix.json`,
`docs/support/capability_matrix.md`, `docs/support/evidence_map.json`,
`docs/support/evidence_map.md`, commits `cb19d16ed`, `6d7c78009`,
`49c78c2ae`, `e782c6074`, `295984cb6`, `9034bf1f5`, `709148d75`,
`016683b19`, `8dead58b3`.

Retired-surface state: docs reject shim, fallback, migration-lane, and
compatibility-mode support claims.

## #8146

Frontend type-surface evidence is indexed from frontend type/dispatch helper,
AST/lowering owner surface, parser-source-model, C-style type surface, and AST
type owner commits.

Evidence: `tests/native/sema/types/typed_i32_bool_flow.objc3`,
`tests/native/sema/types/assignment_unknown_target_rejected.objc3`, commits
`6e2d0e987`, `f5af084ec`, `ffca53ae0`, `2d70b8e34`, `7e743549d`.

Retired-surface state: no compatibility aliases are represented as type support.

## #8147

Deep sema/lowering/runtime metadata evidence is indexed from compiler profile,
sema pass, equivalence, publication, backend handoff, runtime-call lowering,
block validation, message-send validation, runtime metadata emission,
control-flow lowering, and runtime-helper owner commits.

Evidence: `tests/native/sema/ownership/strong_id_assignment.objc3`,
`tests/native/lowering/ownership/block_capture_owned_value_lowering.objc3`,
`tests/native/ir/metadata/actor_executor_metadata_contract.objc3`, commits
`3d4c6c973`, `121c069aa`, `8b103d250`, `112256a5a`, `5464aa831`,
`73ce6b57f`, `668456c12`, `183d5ff4b`, `aa410fda7`, `68f793396`,
`3c335bfae`.

Retired-surface state: unsupported feature claims stay strict diagnostics.

## #8148

JSON/schema infrastructure evidence is indexed from native JSON helpers,
artifact JSON writers, manifest writers, schema infrastructure, artifact
registry owners, capability evidence schemas, evidence-map docs, schema id
normalization, and config truth tables.

Evidence: `docs/support/capability_matrix.schema.json`,
`docs/support/evidence_map.json`, `tests/tooling/test_objc3c_shared_json_schema.py`,
commits `01cf17064`, `3e95c4d73`, `a94330b26`, `6afa3278f`, `972d97906`,
`a87b2c92e`, `709148d75`, `22b8bb1c3`, `ffb2a715d`.

Retired-surface state: schemas classify unsupported states; they do not create
fallback support.

## #8149

Source-hygiene and command-surface evidence is indexed from command hygiene,
source root coverage, allowlist retirement, guardrail scanning, and workflow
hygiene owner commits.

Evidence: `docs/workflows/commands.md`, `docs/workflows/validation.md`,
`docs/support/capability_matrix.json`, commits `606775842`, `fd675f294`,
`058acec90`, `f00e2e871`, `6d0a19f84`, `f49c6f38f`.

Retired-surface state: direct helper commands and report-only/allowlist surfaces
are retired from public claims.

## #8150

Final closure evidence is local and evidence-ready. Remote issue closure, push,
and validation remain deferred under the current worker constraints.

Evidence: `tests/conformance/hard_cutover_issue_index.json`,
`tests/conformance/hard_cutover_retired_surface_absence.json`,
`docs/issues/hard_cutover_8132_8150_evidence.json`,
`docs/issues/hard_cutover_8132_8150_evidence.md`, commits `4c0285b0d`,
`16e517982`, `e5b988129`, `a87b2c92e`, `709148d75`, `016683b19`,
`645e9c25f`.

Retired-surface state: closure is blocked only on allowed validation/GitHub/push
steps, not on missing local issue evidence indexes.
