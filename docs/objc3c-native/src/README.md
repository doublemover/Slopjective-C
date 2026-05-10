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
- `docs/support/capability_matrix.md`
  - current support boundary with executable evidence links

Do not move machine-owned packet inventories, transient output paths, or archived
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
  - `npm run objc3c -- build-native-docs`
  - `npm run objc3c -- check-native-docs`
- site index build path:
  - `npm run objc3c -- build-site`
  - `npm run objc3c -- check-site`
- public command surface build/check path:
  - `npm run objc3c -- build-public-command-surface`
  - `npm run objc3c -- check-public-command-surface`
- public command budget path:
  - `npm run objc3c -- check-public-command-budget`
- reader-facing documentation surface validation:
  - `npm run objc3c -- check-documentation-surface`
  - `npm run objc3c -- validate-documentation-surface`

## Canonical Naming And Path Rules

Use these naming rules when downstream cleanup work renames or consolidates
repo surfaces:

- user-facing package entrypoints come from the `package.json` `objc3c`
  script:
  `npm run objc3c -- <action>`
- checked-in generated docs keep one source root each:
  - `site/index.md` <= `site/src/`
  - `docs/objc3c-native.md` <= `docs/objc3c-native/src/`
  - `docs/runbooks/objc3c_public_command_surface.md` <= `package.json`
    public command surface: `npm run objc3c -- <action>`
- implementation paths stay under `native/objc3c/`, `scripts/`, and `tests/`
- transient outputs stay under `tmp/`
- published binaries and libraries stay under `artifacts/`

Explicit non-goals for naming cleanup:

- inventing duplicate owner directories,
- promoting `tmp/` or `artifacts/` paths into canonical doc inputs,
- reintroducing milestone-coded, stage-coded, or retired alias names as
  first-class command surfaces.

## Generated Doc And Machine-Appendix Surface

These surfaces are generated and must stay tied to their canonical inputs:

- human-facing generated implementation doc:
  - output: `docs/objc3c-native.md`
  - sources: `docs/objc3c-native/src/*.md`
  - generator action: `npm run objc3c -- build-native-docs`
- human-facing generated public site:
  - output: `site/index.md`
  - sources: `site/src/index.body.md`, `site/src/index.contract.json`
  - generator action: `npm run objc3c -- build-site`
- machine-facing generated operator appendix:
  - output: `docs/runbooks/objc3c_public_command_surface.md`
  - sources: `package.json`, `scripts/objc3c_workflow/action_catalog.py`,
    `scripts/build_objc3c_public_command_contract.py`
  - build/check: `npm run objc3c -- build-public-command-surface` / `npm run objc3c -- check-public-command-surface`
  - command-budget check: `npm run objc3c -- check-public-command-budget`

Generated proof and report outputs are transient artifacts, not canonical
documentation sources. Capability claims stay owned by the capability matrix and
evidence map.

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
- Keep large `35-runtime-architecture.md` additions under its section-owner
  groups instead of adding unknown fragment files.
- Put historical planning and closeout material under `tmp/archive/`.
- Avoid milestone-coded sections and issue-era command chains here.
- Do not reintroduce workflow registry facades, adapter layers, old source
  modes, or direct helper commands as supported public paths.

## Contract Validation

- Rebuild: `npm run objc3c -- build-native-docs`
- Drift check: `npm run objc3c -- check-native-docs`
- Reader surface check: `npm run objc3c -- check-documentation-surface`
- Full docs workflow validation: `npm run objc3c -- validate-documentation-surface`
