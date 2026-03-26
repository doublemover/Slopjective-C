# M314-B003 Expectations

Contract ID: `objc3c-cleanup-build-test-doc-runner-unification/m314-b003-v1`

## Purpose

Unify the public build, test, and docs package scripts behind one Python workflow runner so the operator surface stops exposing multiple implementation dialects.

## Required truths

- One shared Python workflow runner now fronts the compact public package subset from `M314-B002`.
- Public package scripts no longer invoke PowerShell or direct Python builders directly.
- PowerShell wrappers remain implementation details behind the runner.
- Internal compatibility aliases outside the public subset may remain unchanged for now.

## Unified public workflows

- native build variants
- docs/spec build
- compile wrapper
- spec lint
- top-level and native test flows in the public subset
- runnable toolchain packaging
- compile proof

## Evidence

- `spec/planning/compiler/m314/m314_b003_build_test_doc_workflow_runner_unification_core_feature_implementation_plan.json`
- `tmp/reports/m314/M314-B003/build_test_doc_workflow_runner_unification_summary.json`

## Next issue

- `M314-B004`
