# objc3c-native Source Fragment Contract

## Ownership Policy

These fragment files are the canonical source for `docs/objc3c-native.md`.
They describe the live native frontend, not historical milestone closeouts.

## Canonical Fragment Taxonomy

- CLI and driver behavior
- Grammar and parser surface
- Semantic and lowering surface
- Runtime execution architecture
- Diagnostics
- Artifacts and build outputs
- Live validation surface
- Library embedding API

## Audience Boundary

- `README.md`
  - onboarding, setup, build/test entrypoints, repository map
- `site/index.md`
  - public project explanation and implementation status
- `docs/objc3c-native.md`
  - implementation-facing narrative over the live native frontend/runtime surface
- `docs/reference/legacy_spec_anchor_index.md`
  - compatibility redirects only, not primary onboarding

Do not move machine-owned packet inventories, `tmp/` proof paths, or archived
milestone closeout material into these fragments.

## Live Code Paths For Documentation Work

Later documentation issues should use these live paths directly instead of
adding sidecar scaffolding:

- source fragments:
  - `docs/objc3c-native/src/*.md`
- stitched implementation doc:
  - `docs/objc3c-native.md`
- public onboarding paths:
  - `README.md`
  - `site/index.md`
- doc build/check path:
  - `scripts/build_objc3c_native_docs.py`
- site index build path:
  - `scripts/build_site_index.py`
- public command surface generation:
  - `scripts/render_objc3c_public_command_surface.py`

Explicit non-goals for this fragment tree:

- historical roadmap narration,
- per-milestone closeout storytelling,
- raw emitted report dumps,
- machine-only appendix material that belongs in generated artifacts instead of
  reader-facing docs.

## Deterministic Stitch Order

1. `10-cli.md`
2. `20-grammar.md`
3. `30-semantics.md`
4. `35-runtime-architecture.md`
5. `40-diagnostics.md`
6. `50-artifacts.md`
7. `60-tests.md`
8. `library-api.md`

## Include Rules

- Keep these fragments focused on the current live surface.
- Put historical planning and closeout material under `tmp/archive/`.
- Avoid milestone-coded sections and issue-era command chains here.

## Contract Validation

- Rebuild: `python scripts/build_objc3c_native_docs.py`
- Drift check: `python scripts/build_objc3c_native_docs.py --check`
