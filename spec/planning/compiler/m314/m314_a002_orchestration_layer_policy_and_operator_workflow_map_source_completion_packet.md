# M314-A002 Planning Packet

## Summary

Freeze the operator workflow map for `M314` so downstream command-surface collapse work can remove ambiguity instead of inventing yet more entrypoint layers.

## Frozen orchestration policy

- Public operator layer: `package.json` scripts.
- Internal parameterized layer: Python runners under `scripts/`.
- Platform-specific implementation layer: PowerShell wrappers and helper scripts.
- CI-only layer: `.github/workflows/*.yml`.

## Current workflow map

- Native build
  - public: `build`, `build:objc3c-native`, `build:objc3c-native:contracts`, `build:objc3c-native:full`, `build:objc3c-native:reconfigure`
  - implementation: `scripts/build_objc3c_native.ps1`
- Native compile
  - public: `compile:objc3c`
  - implementation: `scripts/objc3c_native_compile.ps1`
- Spec build and lint
  - public: `build:spec`, `lint:spec`
  - internal: `python scripts/build_site_index.py`, `python scripts/spec_lint.py`
- Execution smoke
  - public: `test:objc3c:execution-smoke`
  - implementation: `scripts/check_objc3c_native_execution_smoke.ps1`
  - README currently leaks the PowerShell implementation directly and must stop doing that later in `M314`.

## Migration notes

- Direct Python and PowerShell commands may remain documented temporarily only where no compact public package entrypoint has been migrated yet.
- `M314-B003`, `M314-C002`, and `M314-D001` own the public-facing documentation cleanup and wrapper unification.
- `M314-B005` owns the prototype compiler retirement path and must not reintroduce a second public compiler interface.

## Evidence

- `spec/planning/compiler/m314/m314_a002_orchestration_layer_policy_and_operator_workflow_map_source_completion_map.json`

Next issue: `M314-B001`.
