# objc3c Maintainer Workflows

## Generated Documentation Surface

Use the generators directly for documentation surfaces that are supposed to be
checked into the repo:

- public site:
  - source: `site/src/index.body.md`
  - build/check: `npm run build:site` / `npm run check:site`
- native implementation doc:
  - source: `docs/objc3c-native/src/*.md`
  - build/check: `npm run build:docs:native` / `npm run check:docs:native`
- machine-facing operator appendix:
  - source: `package.json` + `scripts/objc3c_public_workflow_runner.py`
  - build/check: `npm run build:docs:commands` / `npm run check:docs:commands`

Do not hand-edit generated outputs. Do not treat `tmp/reports/` or
`tmp/artifacts/` as canonical documentation.

## Superclean Working Boundary

Use these roots directly when cleaning or renaming repo surfaces:

- implementation roots:
  - `native/objc3c/`
  - `scripts/`
  - `tests/`
- checked-in doc sources:
  - `README.md`
  - `CONTRIBUTING.md`
  - `site/src/`
  - `docs/objc3c-native/src/`
  - `package.json`
- generated checked-in outputs:
  - `site/index.md`
  - `docs/objc3c-native.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
- machine-owned outputs only:
  - `tmp/`
  - `artifacts/`
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json` is the build-emitted source-of-truth artifact for the repo superclean surface

Do not add milestone-specific wrappers, sidecar compatibility files, or
parallel source-of-truth copies when changing these surfaces.

Contributor-facing entrypoint:

- `CONTRIBUTING.md` is the contributor instruction surface for normal repo
  changes
- `README.md` stays focused on onboarding, setup, and repo navigation
- this runbook is maintainer-only and should not accumulate contributor
  guidance that belongs in `CONTRIBUTING.md`

## Build

```powershell
npm run build:objc3c-native
npm run build:objc3c-native:contracts
```

## Test

```powershell
npm run test:smoke
npm run test:ci
npm run test:docs
npm run test:repo
```

`npm run test:ci` now includes the compact documentation integration surface:

- generated site drift,
- generated native-doc drift,
- generated public-command-surface drift,
- and reader-facing documentation/readability boundary checks.

`npm run test:docs` runs the full documentation build/check pass:

- rebuild the published site output,
- rebuild the generated native implementation docs,
- rebuild the generated public command appendix,
- and then re-check all three outputs plus the reader-facing documentation surface.

## Direct tools

- dependency boundaries: `python scripts/check_objc3c_dependency_boundaries.py --strict`
- task hygiene: `python scripts/ci/check_task_hygiene.py`
- repo superclean surface: `npm run check:repo:surface`
- repo superclean integration: `npm run test:repo`
- docs stitch/check: `npm run check:docs:native`
- parity source check: `python scripts/check_objc3c_library_cli_parity.py ...`

The live maintainer surface is intentionally small. Historical planning, contract, and milestone-specific validation material is archived under `tmp/archive/`.
