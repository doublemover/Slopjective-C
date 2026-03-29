# objc3c Maintainer Workflows

## Generated Documentation Surface

Use the generators directly for documentation surfaces that are supposed to be
checked into the repo:

- public site:
  - source: `site/src/index.body.md`
  - build/check: `npm run build:spec` / `npm run check:spec:generated`
- native implementation doc:
  - source: `docs/objc3c-native/src/*.md`
  - build/check: `npm run build:docs:native` / `npm run check:docs:native`
- machine-facing operator appendix:
  - source: `package.json` + `scripts/objc3c_public_workflow_runner.py`
  - build/check: `npm run build:docs:commands` / `npm run check:docs:commands`

Do not hand-edit generated outputs. Do not treat `tmp/reports/` or
`tmp/artifacts/` as canonical documentation.

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
- docs stitch/check: `npm run check:docs:native`
- parity source check: `python scripts/check_objc3c_library_cli_parity.py ...`

The live maintainer surface is intentionally small. Historical planning, contract, and milestone-specific validation material is archived under `tmp/archive/`.
