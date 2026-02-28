# site index Source Ownership

This file defines source ownership for generated `site/index.md`.

## Scope

- Source directory: `site/src/`
- Generated output: `site/index.md`
- Generator: `python scripts/build_site_index.py`

## Source Ownership Matrix

| Source file | Responsibility | Primary owner | Backup owner |
| --- | --- | --- | --- |
| `index.contract.json` | Generator contract, canonical input/output paths, front matter | compiler/docs | compiler/tooling |
| `README.md` | Generated-only policy and contributor guidance | compiler/docs | compiler/release |
| `spec/TABLE_OF_CONTENTS.md` | Deterministic include ordering consumed by generator | spec/maintainers | compiler/docs |
| `spec/*.md` listed in TOC | Stitched normative content for site output | spec/maintainers | compiler/docs |

## Update Workflow

1. Update `site/src/index.contract.json` only when generator contract changes.
1. Update spec content under `spec/` as needed.
1. Run `python scripts/build_site_index.py` to regenerate `site/index.md`.
1. Run `python scripts/build_site_index.py --check` and require pass.

## Review Policy

- Manual edits to `site/index.md` are unsupported.
- Contract or policy changes in `site/src/*` require `compiler/docs` review.
- TOC ordering changes require `spec/maintainers` review.
- PR description must include regeneration command and drift-check result.
