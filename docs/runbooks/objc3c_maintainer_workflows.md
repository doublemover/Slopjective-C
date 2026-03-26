# Objective-C 3 Maintainer Workflows

This runbook is the canonical maintainer/operator workflow guide for the cleaned
`M314` command surface.

## Canonical Docs

- README quickstart: [README.md](../../README.md)
- Public command surface: [objc3c_public_command_surface.md](objc3c_public_command_surface.md)
- Incremental native build workflow: [m276_incremental_native_build_operator_runbook.md](m276_incremental_native_build_operator_runbook.md)

## Public-First Rule

- Prefer the public `package.json` command surface for normal operator work.
- Those commands delegate through `scripts/objc3c_public_workflow_runner.py`.
- No legacy compatibility or milestone-local package-script aliases remain supported.
- `native/objc3c/` is the only supported compiler implementation root.
- The retired prototype Python compiler surface is archival only and must not be
  treated as an executable workflow path.

## Common Maintainer Flows

### Build the native toolchain

```powershell
npm run build:objc3c-native
```

### Rebuild packets and contracts

```powershell
npm run build:objc3c-native:contracts
```

### Rebuild the public site overview

```powershell
npm run build:spec
```

### Compile one fixture through the native compiler

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/readme-hello --emit-prefix module
```

### Run the public native validation surfaces

```powershell
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:full
```

### Run the compile proof workflow

```powershell
npm run proof:objc3c
```

## When Direct Scripts Are Acceptable

- Use direct Python or PowerShell scripts only when working on internal tooling
  or when no public wrapper exists yet.
- When a public wrapper exists, do not document the direct script as the primary
  operator path.

## Direct Maintainer Checks

These are maintainer-only and intentionally not part of the public package surface:

- `python scripts/ci/run_task_hygiene_gate.py`
- `python scripts/check_objc3c_dependency_boundaries.py --strict`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lexer_extraction_token_contract.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1`
