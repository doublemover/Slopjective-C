# Validation Harness Catalog

- issue: `M313-B002`
- policy_id: `validation-consolidation-policy-v1`
- harness_path: `scripts/shared_compiler_runtime_acceptance_harness.py`
- harness_suite_count: `3`
- workflow_action_count: `135`
- workflow_script_count: `135`

## Shared acceptance harness suites
- `runtime-acceptance`
- `public-test-fast`
- `public-test-full`

## Public workflow validation families
- `aggregate-validation`: `3` actions, `3` package scripts, tiers=`acceptance`
- `bonus-experiences`: `2` actions, `2` package scripts, tiers=`acceptance, runnable`
- `compiler-throughput`: `3` actions, `3` package scripts, tiers=`acceptance, runnable`
- `conformance-corpus`: `2` actions, `2` package scripts, tiers=`acceptance, runnable`
- `distribution-credibility`: `6` actions, `6` package scripts, tiers=`acceptance, runnable, static-guard`
- `docs`: `6` actions, `6` package scripts, tiers=`acceptance, static-guard`
- `external-validation`: `4` actions, `4` package scripts, tiers=`acceptance, integration, static-guard`
- `misc`: `36` actions, `36` package scripts, tiers=`acceptance, integration`
- `onboarding`: `1` actions, `1` package scripts, tiers=`acceptance`
- `packaging-channels`: `4` actions, `4` package scripts, tiers=`acceptance, runnable, static-guard`
- `performance`: `8` actions, `8` package scripts, tiers=`acceptance, runnable`
- `performance-governance`: `5` actions, `5` package scripts, tiers=`acceptance, integration, runnable, static-guard`
- `public-conformance`: `7` actions, `7` package scripts, tiers=`acceptance, integration, runnable, static-guard`
- `release-foundation`: `3` actions, `3` package scripts, tiers=`acceptance, static-guard`
- `release-operations`: `5` actions, `5` package scripts, tiers=`acceptance, runnable, static-guard`
- `repo-shape`: `2` actions, `2` package scripts, tiers=`acceptance, static-guard`
- `runtime-architecture`: `1` actions, `1` package scripts, tiers=`acceptance`
- `runtime-closure`: `17` actions, `17` package scripts, tiers=`acceptance, runnable`
- `showcase`: `3` actions, `3` package scripts, tiers=`acceptance, runnable, static-guard`
- `static-guard-surface`: `1` actions, `1` package scripts, tiers=`static-guard`
- `stdlib`: `8` actions, `8` package scripts, tiers=`acceptance, runnable, static-guard`
- `stress`: `8` actions, `8` package scripts, tiers=`acceptance, integration, runnable, static-guard`

## Direct non-runner validation scripts
- `check:objc3c:llvm-capabilities` -> `python scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/llvm_capabilities/summary.json`
- `check:release-evidence` -> `python scripts/check_release_evidence.py`
- `check:task-hygiene` -> `python scripts/ci/run_task_hygiene_gate.py`
- `check:objc3c:boundaries` -> `python scripts/check_objc3c_dependency_boundaries.py --strict`
- `check:md` -> `prettier --check "**/*.md"`
- `check:md:all` -> `npm run build:site && npm run check:md`

## Migration targets
- primary shared harness: `scripts/shared_compiler_runtime_acceptance_harness.py`
- primary public runner: `scripts/objc3c_public_workflow_runner.py`
- legacy namespace work: `M313-B003`
- artifact contract work: `M313-C001`
