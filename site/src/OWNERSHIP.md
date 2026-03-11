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
