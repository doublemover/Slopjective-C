# M314-A002 Expectations

Contract ID: `objc3c-cleanup-orchestration-layer-workflow-map/m314-a002-v1`

## Purpose

Freeze one truthful orchestration-layer policy and one operator workflow map on top of the measured command-surface inventory from `M314-A001`.

## Required truths

- `package.json` scripts are the primary public operator-facing entrypoint layer.
- Python runners under `scripts/` are the internal parameterized orchestration layer.
- PowerShell scripts are implementation details or narrow platform-specific helpers, not a competing public operator namespace.
- GitHub workflow files are CI entrypoints only.
- The README currently leaks two direct non-public workflow commands that must be migrated later:
  - `python scripts/build_site_index.py`
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1`
- Current public operator workflows are already anchored around package scripts for native build, native compile, spec build, and spec lint.

## Frozen workflow map

- Build workflow: public package entrypoints backed by PowerShell implementation wrappers.
- Documentation workflow: public package entrypoints with internal Python builders.
- Execution-smoke workflow: public package entrypoint retained, README direct PowerShell usage marked transitional.
- CI workflow entrypoints remain non-public and are not part of the operator command budget.

## Evidence

- `spec/planning/compiler/m314/m314_a002_orchestration_layer_policy_and_operator_workflow_map_source_completion_map.json`
- `tmp/reports/m314/M314-A002/orchestration_layer_workflow_map_summary.json`

## Next issue

- `M314-B001`
