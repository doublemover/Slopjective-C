# objc3c Maintainer Workflows

## Generated Documentation Surface

Use the generators directly for documentation surfaces that are supposed to be
checked into the repo:

- public site:
  - source: `site/src/index.body.md`
  - build/check: `python scripts/build_site_index.py` / `python scripts/build_site_index.py --check`
- native implementation doc:
  - source: `docs/objc3c-native/src/*.md`
  - build/check: `python scripts/build_objc3c_native_docs.py` / `python scripts/build_objc3c_native_docs.py --check`
- machine-facing operator appendix:
  - source: `package.json` + `scripts/objc3c_public_workflow_runner.py`
  - build/check: `python scripts/render_objc3c_public_command_surface.py` / `python scripts/render_objc3c_public_command_surface.py --check`

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
```

## Direct tools

- dependency boundaries: `python scripts/check_objc3c_dependency_boundaries.py --strict`
- task hygiene: `python scripts/ci/check_task_hygiene.py`
- docs stitch/check: `python scripts/build_objc3c_native_docs.py --check`
- parity source check: `python scripts/check_objc3c_library_cli_parity.py ...`

The live maintainer surface is intentionally small. Historical planning, contract, and milestone-specific validation material is archived under `tmp/archive/`.
