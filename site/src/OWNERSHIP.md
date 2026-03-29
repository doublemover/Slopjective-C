# site index Source Ownership

This file defines source ownership for generated `site/index.md`.

## Scope

- Source directory: `site/src/`
- Generated output: `site/index.md`
- Generator: `python scripts/build_site_index.py`

## Source Ownership Matrix

| Source file                 | Responsibility                                                 | Primary owner    | Backup owner     |
| --------------------------- | -------------------------------------------------------------- | ---------------- | ---------------- |
| `index.contract.json`       | Generator contract, canonical input/output paths, front matter | compiler/docs    | compiler/tooling |
| `index.body.md`             | Curated public-facing site content and stable public anchors   | compiler/docs    | spec/maintainers |
| `README.md`                 | Generated-only policy and contributor guidance                 | compiler/docs    | compiler/release |

## Update Workflow

1. Update `site/src/index.contract.json` only when generator contract changes.
1. Update the curated site content under `site/src/index.body.md` as needed.
1. Run `python scripts/build_site_index.py` to regenerate `site/index.md`.
1. Run `python scripts/build_site_index.py --check` and require pass.

## Review Policy

- Manual edits to `site/index.md` are unsupported.
- Contract or policy changes in `site/src/*` require `compiler/docs` review.
- Anchor or status-model changes in `site/src/index.body.md` require `compiler/docs` review.
- PR description must include regeneration command and drift-check result.

## Generated Surface Boundary

Treat these as the live documentation-generation surfaces:

- human-facing generated site:
  - `site/src/index.body.md`
  - `site/src/index.contract.json`
  - `python scripts/build_site_index.py`
  - `site/index.md`
- human-facing generated native implementation doc:
  - `docs/objc3c-native/src/*.md`
  - `python scripts/build_objc3c_native_docs.py`
  - `docs/objc3c-native.md`
- machine-facing generated operator appendix:
  - `package.json`
  - `scripts/objc3c_public_workflow_runner.py`
  - `python scripts/render_objc3c_public_command_surface.py`
  - `docs/runbooks/objc3c_public_command_surface.md`

`tmp/reports/` and `tmp/artifacts/` remain generated proof/evidence outputs, not
canonical documentation sources.
