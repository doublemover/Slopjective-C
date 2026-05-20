# Capability Schema Examples

These examples show the minimum shapes expected by the canonical capability
schemas. They are illustrative; the checked-in owner data remains
`docs/support/capability_matrix.json` and `docs/support/evidence_map.json`.
The schema IDs are `objc3c-capability-matrix-v1` and
`objc3c-capability-evidence-map-v1`, owned by
`scripts/objc3c_shared/schema_registry.py` and published to the native artifact
schema registry by
`native/objc3c/src/artifacts/json/capability_support_schema_records.cpp`;
`docs/support/capability_matrix.schema.json` is a ref-only support-directory
entrypoint for the canonical matrix schema. Examples and support-directory files
must not duplicate schema fragments.

## Capability Matrix Entry

```json
{
  "id": "compiler.parser.core-declarations",
  "title": "Canonical parser syntax",
  "state": "implemented",
  "summary": "The native parser accepts canonical modules, bindings, functions, control flow, and expression forms used by the runnable subset.",
  "support_claims": ["objc3c.behavior.parser.canonical-syntax"],
  "owner_modules": [
    "native/objc3c/src/parse/objc3_parser_core.cpp",
    "native/objc3c/src/parse/objc3_parser_declaration_surface.cpp"
  ],
  "evidence": [
    {
      "kind": "test",
      "path": "tests/native/parser/positive/canonical_module_main.objc3",
      "command": "npm run objc3c -- test-behavior-matrix"
    },
    {
      "kind": "source",
      "path": "native/objc3c/src/parse/objc3_parser.cpp"
    }
  ]
}
```

Rules shown by this entry:

- `implemented` behavior needs evidence.
- `implemented` behavior is the only state that may carry a public
  `objc3c.behavior.*` support claim.
- Public replay commands use the npm bridge only.
- Source files and `owner_modules` may bound the implementation without
  becoming public command surface.

## Implemented Runtime Capability Entry

```json
{
  "id": "runtime.concurrency.async-actors",
  "title": "Async and actor runtime closure",
  "state": "implemented",
  "summary": "The private runtime path has canonical actor executor behavior and runtime-acceptance evidence for async continuation, task scheduler, and actor mailbox helpers.",
  "support_claims": ["objc3c.behavior.runtime.concurrency-async-actors"],
  "evidence": [
    {
      "kind": "test",
      "path": "tests/native/runtime/concurrency/actor_executor_contract.objc3",
      "command": "npm run objc3c -- test-behavior-matrix"
    },
    {
      "kind": "test",
      "path": "scripts/objc3c_runtime_acceptance/domains/concurrency_live_runtime_cases.py",
      "command": "npm run objc3c -- test-runtime-acceptance-fast"
    }
  ]
}
```

Rules shown by this entry:

- Implemented runtime rows must carry a canonical behavior fixture and an npm
  bridge command.
- Runtime probe evidence may narrow the support claim without widening public
  ABI support.
- Reserved, rejected, and internal rows still do not carry public behavior
  support claims.

## Retired Surface Term

```json
{
  "term": "retired adapter",
  "canonical_handling": "Reject as a support claim; replace with an implemented capability row or a diagnostic rejection row.",
  "allowed_context": "Negative examples only."
}
```

Rules shown by this entry:

- Retired wording is data for rejection and hygiene, not a support state.
- Active capability rows use current feature names and evidence.

## Evidence Map Row

```json
{
  "capability_id": "compiler.e2e.runnable-smoke",
  "support_claim": "objc3c.behavior.e2e.runnable-smoke",
  "evidence_kind": "test",
  "path": "tests/native/e2e/smoke/basic_i32_return_main.objc3",
  "command": "npm run objc3c -- test-behavior-matrix"
}
```

Rules shown by this row:

- `capability_id` must match a matrix entry.
- `support_claim` uses the `objc3c.behavior.*` namespace.
- `command` is present only for public npm-bridge replay commands.

## Evidence Policy

Projection contract:

```json
{
  "source": "docs/support/capability_matrix.json#/capabilities/*/evidence",
  "owner": "scripts/capability_docs_validator/evidence_map.py",
  "row_key": ["capability_id", "support_claim", "evidence_kind", "path", "command"],
  "drift_rule": "The evidence map is a flattened projection of capability matrix evidence rows. Validators fail on duplicate, missing, or extra row keys."
}
```

Rules shown by this contract:

- Evidence-map rows are keyed projections of matrix evidence, not independent
  support claims.
- `support_claim` and `command` are key fields when present; blank cells remain
  part of the ownership-boundary key.
- A row can be added or removed only by keeping the matrix and evidence map in
  exact key agreement.

```json
{
  "public_command_surface": "npm run objc3c -- <action>",
  "command_required_for": ["replayable implemented behavior evidence"],
  "command_forbidden_for": [
    "source ownership rows",
    "schema ownership rows",
    "doc boundary rows",
    "diagnostic inventory rows that are not public replay commands"
  ],
  "row_role_rule": "Rows without command are ownership or boundary evidence; they do not define public workflow surface or broaden capability state."
}
```

Rules shown by this policy:

- A source, schema, doc, or diagnostic row can support a boundary without
  becoming a public action.
- Replayable behavior evidence uses the npm bridge and keeps the command on the
  exact row it replays.
- A blank command cell is intentional; consumers must not infer hidden helper
  commands from it.

## Support Claim Contract

```json
{
  "support_claim_required_for_states": ["implemented"],
  "support_claim_forbidden_for_states": ["rejected", "reserved", "internal"],
  "public_behavior_claim_namespace": "objc3c.behavior.*",
  "no_alias_or_retired_route_claim_sources": [
    "retired surface terms",
    "retired mode labels",
    "workflow registry facades",
    "direct helper commands",
    "generated-output rows without matching implemented rows",
    "compatibility or retired route wording"
  ],
  "owner_only_states": ["rejected", "reserved", "internal"],
  "consumer_rule": "Only implemented rows with support_claims may produce public Objective-C 3.0 behavior claims. Rejected, reserved, and internal rows are negative, reserved, schema, workflow, owner-boundary, or evidence-boundary rows only."
}
```

Rules shown by this contract:

- Support claims are state-gated before docs or release evidence can quote
  them.
- The matrix schema enforces the state gate: implemented rows require
  `support_claims`, while rejected, reserved, and internal rows cannot carry
  them.
- Alias, compatibility, retired route, registry-facade, helper-command, and
  evidence-log wording cannot become public behavior support.
- Owner-only rows can identify claim responsibility without widening the
  supported language surface.

## Anti-Examples

These are not valid support claims:

- "supported through a retired adapter"
- "accepted by an alternate parser path"
- "available through a retired mode label"
- "retired-source lane accepts old syntax"
- "run a direct helper script as the public command"
- "implemented because a roadmap says it is planned"
- "complete because a generated-output row says so without a matching implemented row"
