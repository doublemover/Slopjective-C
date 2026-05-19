# site index Source Ownership

This file defines source ownership for generated `site/index.md`.

## Scope

- Source directory: `site/src/`
- Generated output: `site/index.md`
- Generator action: `npm run objc3c -- build-site`

## Source Ownership Matrix

| Source file           | Responsibility                                                 | Primary owner | Backup owner     |
| --------------------- | -------------------------------------------------------------- | ------------- | ---------------- |
| `index.contract.json` | Generator contract, canonical input/output paths, front matter | compiler/docs | compiler/tooling |
| `index.body.md`       | Curated public-facing site content and stable public anchors   | compiler/docs | spec/maintainers |
| `README.md`           | Generated-only policy and contributor guidance                 | compiler/docs | compiler/release |

## Update Workflow

1. Update `site/src/index.contract.json` only when generator contract changes.
1. Update the curated site content under `site/src/index.body.md` as needed.
1. Run `npm run objc3c -- build-site` to regenerate `site/index.md`.
1. Run `npm run objc3c -- check-site` and require pass.

## Review Policy

- Direct changes to `site/index.md` must match owner-input updates and
  regeneration evidence.
- Contract or policy changes in `site/src/*` require `compiler/docs` review.
- Anchor or status-model changes in `site/src/index.body.md` require `compiler/docs` review.
- PR description must include regeneration command and drift-check result.
- Command examples must use the public npm action surface,
  `npm run objc3c -- <action>`, and must not introduce retired package-script
  names.
- Capability status changes must cite `docs/support/capability_matrix.*`,
  `docs/support/evidence_map.md`, and
  `docs/support/capability_claim_responsibility.md`.

## Generated Surface Boundary

Treat these as the live documentation-generation surfaces:

- human-facing generated site:
  - `site/src/index.body.md`
  - `site/src/index.contract.json`
  - `npm run objc3c -- build-site`
  - `site/index.md`
- human-facing generated native implementation doc:
  - `docs/objc3c-native/src/*.md`
  - `npm run objc3c -- build-native-docs`
  - `docs/objc3c-native.md`
- machine-facing generated operator appendix:
  - `package.json`
  - public npm action surface: `npm run objc3c -- <action>`
  - action catalog: `scripts/objc3c_workflow/action_catalog.py`
  - `npm run objc3c -- build-public-command-surface`
  - `docs/runbooks/objc3c_public_command_surface.md`

Generated report and artifact directories remain transient output roots, not
documentation owner inputs.
