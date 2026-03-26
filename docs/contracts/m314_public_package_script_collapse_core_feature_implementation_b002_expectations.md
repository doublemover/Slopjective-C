# M314-B002 Expectations

Contract ID: `objc3c-cleanup-public-package-script-collapse/m314-b002-v1`

## Purpose

Implement a compact public package-script surface and remove the broader compatibility alias mass from `package.json`.

## Required truths

- `package.json` publishes one explicit machine-readable public command subset for operators.
- That public subset contains at most `25` commands.
- Public documentation prefers the explicit public subset over direct Python or PowerShell commands where a public wrapper already exists.
- Legacy aliases are removed from `package.json` and are not part of the public command budget.

## Implemented public subset

- build
- native build variants
- spec build
- compile
- spec lint
- top-level and native test entrypoints needed for ordinary operator workflows
- packaging and proof entrypoints

## Non-goals

- Do not broaden the public command set beyond the frozen subset.
- Do not reintroduce a compatibility alias window.
- Do not retire the prototype compiler path here.

## Evidence

- `spec/planning/compiler/m314/m314_b002_public_package_script_collapse_core_feature_implementation_surface.json`
- `tmp/reports/m314/M314-B002/public_package_script_collapse_summary.json`

## Next issue

- `M314-B003`
