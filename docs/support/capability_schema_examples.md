# Capability Schema Examples

These examples show the minimum shapes expected by the canonical capability
truth schemas. They are illustrative; the checked-in truth remains
`docs/support/capability_matrix.json` and `docs/support/evidence_map.json`.

## Capability Matrix Entry

```json
{
  "id": "compiler.parser.core-declarations",
  "title": "Canonical parser syntax",
  "state": "implemented",
  "summary": "The native parser accepts canonical modules, bindings, functions, control flow, and expression forms used by the runnable subset.",
  "support_claims": [
    "objc3c.behavior.parser.canonical-syntax"
  ],
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
- Public replay commands use the npm bridge only.
- Source files and `owner_modules` may bound the implementation without
  becoming public command surface.

## Reserved Capability Entry

```json
{
  "id": "runtime.concurrency.async-actors",
  "title": "Async and actor runtime closure",
  "state": "reserved",
  "summary": "Async, task, and actor syntax and runtime concepts remain unavailable unless an entry is separately marked implemented here.",
  "evidence": [
    {
      "kind": "doc",
      "path": "docs/spec/concurrency_reserved.md"
    },
    {
      "kind": "diagnostic",
      "path": "tests/conformance/diagnostics/manifest.json"
    }
  ]
}
```

Rules shown by this entry:

- Reserved is an unavailable state, not a fallback mode.
- A reserved row can point to docs or diagnostics instead of runnable tests.
- A row that moves this capability out of `reserved` must carry evidence before
  docs can claim public support.

## Retired Surface Term

```json
{
  "term": "shim",
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

```json
{
  "public_command_surface": "npm run objc3c -- <action>",
  "command_required_for": [
    "replayable implemented behavior evidence"
  ],
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

## Anti-Examples

These are not valid support claims:

- "supported through a shim"
- "accepted by parser fallback"
- "available in compatibility mode"
- "migration lane accepts old syntax"
- "run a direct helper script as the public command"
- "implemented because a roadmap says it is planned"
- "complete because a generated report says so without a matching implemented row"
