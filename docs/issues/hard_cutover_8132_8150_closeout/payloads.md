# Hard-Cutover Tracker Closeout Payloads

These are local, tracker-ready notes for `#8132`-`#8150`. They do not assert
remote closure. Validation, `gh`, push, and issue edits were intentionally not
run while preparing them.

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
