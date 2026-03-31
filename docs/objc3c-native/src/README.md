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
- `CONTRIBUTING.md`
  - contributor instructions for normal repo changes inside the live boundary
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
  - `CONTRIBUTING.md`
  - `site/index.md`
- doc build/check path:
  - `npm run build:docs:native`
  - `npm run check:docs:native`
- site index build path:
  - `npm run build:site`
  - `npm run check:site`
- public command surface build/check path:
  - `npm run build:docs:commands`
  - `npm run check:docs:commands`

## Canonical Naming And Path Rules

Use these naming rules when downstream cleanup work renames or consolidates
repo surfaces:

- user-facing package entrypoints come from `package.json` and map directly to
  `scripts/objc3c_public_workflow_runner.py`
- checked-in generated docs keep one source root each:
  - `site/index.md` <= `site/src/`
  - `docs/objc3c-native.md` <= `docs/objc3c-native/src/`
  - `docs/runbooks/objc3c_public_command_surface.md` <= `package.json` +
    `scripts/objc3c_public_workflow_runner.py`
- implementation paths stay under `native/objc3c/`, `scripts/`, and `tests/`
- transient outputs stay under `tmp/`
- published binaries and libraries stay under `artifacts/`

Explicit non-goals for naming cleanup:

- inventing second source-of-truth directories,
- promoting `tmp/` or `artifacts/` paths into canonical doc inputs,
- reintroducing milestone-coded, stage-coded, or compatibility-alias names as
  first-class command surfaces.

## Generated Doc And Machine-Appendix Surface

These surfaces are generated and must stay tied to their canonical inputs:

- human-facing generated implementation doc:
  - output: `docs/objc3c-native.md`
  - sources: `docs/objc3c-native/src/*.md`
  - generator: `python scripts/build_objc3c_native_docs.py`
- human-facing generated public site:
  - output: `site/index.md`
  - sources: `site/src/index.body.md`, `site/src/index.contract.json`
  - generator: `python scripts/build_site_index.py`
- machine-facing generated operator appendix:
  - output: `docs/runbooks/objc3c_public_command_surface.md`
  - sources: `package.json`, `scripts/objc3c_public_workflow_runner.py`, `scripts/build_objc3c_public_command_contract.py`
  - build/check: `npm run build:docs:commands` / `npm run check:docs:commands`

Generated proof and report outputs under `tmp/` are evidence, not canonical
documentation sources.

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
