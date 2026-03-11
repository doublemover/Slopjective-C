# M276 Incremental Native Build Operator Runbook

## Build tree and published artifacts

- Persistent build tree:
  - `tmp/build-objc3c-native`
- Compile database:
  - `tmp/build-objc3c-native/compile_commands.json`
- Build fingerprint:
  - `tmp/build-objc3c-native/native_build_backend_fingerprint.json`
- Published native outputs:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`

## Fingerprint inputs

The wrapper reconfigures when the stored fingerprint no longer matches the current operator environment. The fingerprint includes:

- `generator`
- `cmake`
- `ninja`
- `clangxx`
- `llvm_root`
- `llvm_include_dir`
- `libclang`
- `build_dir`
- `source_dir`
- `runtime_output_dir`
- `library_output_dir`
- `direct_object_emission`
- `warning_parity`

## Operator workflow

| Command | Use when | What it does |
| --- | --- | --- |
| `npm run build:objc3c-native` | normal local binary work | refreshes native binaries through the persistent CMake/Ninja tree |
| `npm run build:objc3c-native:contracts` | packet-only/checker work | regenerates the public contracts packet surface without forcing a full build |
| `npm run build:objc3c-native:full` | closeout or deliberately broad validation | refreshes binaries and the full packet family |
| `npm run build:objc3c-native:reconfigure` | toolchain or fingerprint drift | forces a fresh configure against the existing persistent build tree, then rebuilds binaries |

## Diagnosing stale or mismatched build trees

- If the wrapper reports `cmake_configure=refresh-fingerprint`, the stored fingerprint no longer matches the current toolchain or path surface.
- If the wrapper reports `cmake_configure=force-reconfigure`, the operator explicitly requested a fresh configure.
- If `compile_commands.json` is missing after configure, the configure step is not healthy and the wrapper should fail closed.
- The supported recovery path is reconfigure, not deletion:
  - `npm run build:objc3c-native:reconfigure`

## Validation evidence

- Helper-driven issue work writes summaries under `tmp/reports/<milestone>/<issue>/`.
- Build-surface modernization closeout evidence for this tranche lives under:
  - `tmp/reports/m276/`
