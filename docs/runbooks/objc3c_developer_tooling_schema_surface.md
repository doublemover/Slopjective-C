# objc3c Developer Tooling Schema Surface

## Purpose

This runbook defines the checked schema gate for the Objective-C 3 developer
tooling editor surface. It is a validation and contract layer over the existing
`inspect-editor-tooling` workflow; it is not a new editor, language server
daemon, debugger, or runtime behavior path.

## Source Of Truth

Checked-in contract and schemas:

- `tests/tooling/fixtures/developer_tooling/tooling_schema_surface_contract.json`
- `schemas/objc3c-developer-tooling-editor-surface-v1.schema.json`
- `schemas/objc3c-developer-tooling-schema-surface-summary-v1.schema.json`

Checker and focused tests:

- `scripts/check_objc3c_developer_tooling_schema_surface.py`
- `tests/tooling/test_developer_tooling_schema_surface.py`

Generated validation output stays under `tmp/`:

- `tmp/reports/developer-tooling/schema-surface/tooling_schema_surface_summary.json`

## Replay Command

Run the focused schema-surface check from the repo root:

```powershell
npm run objc3c -- validate-developer-tooling
```

The checker runs the existing workflow action through:

```powershell
npm run objc3c -- inspect-editor-tooling tests/tooling/fixtures/native/hello.objc3
```

It validates the generated combined editor surface and verifies that these
standalone payloads match the embedded payloads exactly:

- language-server capability surface
- navigation index
- workspace semantic index
- artifact inspector
- formatter surface
- debug map

## Truth Boundaries

The schema gate proves only the generated JSON contract and the fail-closed
publication boundaries for the existing tooling payloads.

It does not claim:

- a live LSP server process
- references, rename, semantic tokens, or general code actions
- statement-level debugger stepping
- full source-map publication
- editor-extension packaging

Those surfaces must stay unpublished or reserved until the canonical compiler
and runtime artifacts prove them directly.
