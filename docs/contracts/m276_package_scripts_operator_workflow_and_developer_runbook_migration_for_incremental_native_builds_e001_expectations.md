# M276 Package Scripts, Operator Workflow, And Developer Runbook Migration For Incremental Native Builds Expectations (E001)

Contract ID: `objc3c-incremental-native-build-operator-surface/m276-e001-v1`

## Required outcomes

- `package.json` must expose:
  - `build:objc3c-native`
  - `build:objc3c-native:contracts`
  - `build:objc3c-native:full`
  - `build:objc3c-native:reconfigure`
- README guidance, native docs, runtime-tooling docs, and one dedicated runbook must describe:
  - the persistent build tree at `tmp/build-objc3c-native`
  - the compile database at `tmp/build-objc3c-native/compile_commands.json`
  - the fingerprint file at `tmp/build-objc3c-native/native_build_backend_fingerprint.json`
  - the fingerprint inputs:
    - generator
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
    - direct-object-emission and warning-parity flags
  - the operator decision boundary for `fast`, `contracts-only`, `full`, and `reconfigure`
  - stale-tree diagnosis without any deletion workflow
- Outdated wording that still says the command split is unfinished or implies full rebuilds for ordinary issue work must be removed.

## Required implementation surface

- `package.json`
- `README.md`
- `docs/objc3c-native/src/50-artifacts.md`
- `docs/runbooks/m276_incremental_native_build_operator_runbook.md`
- `tests/tooling/runtime/README.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Proof obligations

- `npm run build:objc3c-native:reconfigure` must succeed.
- The reconfigure path must leave:
  - `tmp/build-objc3c-native/compile_commands.json`
  - `tmp/build-objc3c-native/native_build_backend_fingerprint.json`
- The issue-local evidence must live under `tmp/reports/m276/M276-E001/`.
