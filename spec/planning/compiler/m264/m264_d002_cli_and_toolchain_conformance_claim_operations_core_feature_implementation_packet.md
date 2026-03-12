# M264-D002 Packet

Issue: `#7241`
Milestone: `M264`
Lane: `D`
Wave: `W57`

## Objective

Implement the real operator surface for the truthful conformance artifacts added
in `M264-C001`, `M264-C002`, and `M264-D001`.

## Scope

- add native CLI flag parity for:
  - `--emit-objc3-conformance`
  - `--emit-objc3-conformance-format <json|yaml>`
  - `--validate-objc3-conformance <report.json>`
- keep JSON as the only runnable format
- fail closed for YAML
- consume emitted report/publication sidecars in validation-only mode
- publish one machine-readable validation summary sidecar

## Code anchors

- `native/objc3c/src/driver/objc3_cli_options.h`
- `native/objc3c/src/driver/objc3_cli_options.cpp`
- `native/objc3c/src/driver/objc3_compilation_driver.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/io/objc3_manifest_artifacts.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`

## Spec anchors

- `docs/objc3c-native.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `spec/DECISIONS_LOG.md`

## Acceptance

- compile path accepts the emit/format flags and remains truthful
- validation-only path succeeds on a real emitted report/publication pair
- validation-only path writes `module.objc3-conformance-validation.json`
- validation summary exposes the current live profile/compatibility truth surface
- YAML requests fail closed with deterministic diagnostics
- malformed report input fails closed with deterministic diagnostics
- issue-local checker, pytest, and lane-D readiness all pass
