# objc3c Maintainer Workflows

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
