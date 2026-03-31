# Validation Acceptance Suite Matrix

- issue: `M313-C002`
- policy_id: `validation-consolidation-policy-v1`
- suite_family_count: `20`

## Shared acceptance harness suites
- `runtime-acceptance`
- `public-test-fast`
- `public-test-full`

## Canonical suite families
- `aggregate-validation`
  - tiers: `acceptance`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:fast`, `test:objc3c:full`, `test:objc3c:nightly`
- `bonus-experiences`
  - tiers: `acceptance, runnable`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:bonus-experiences`, `test:bonus-experiences:e2e`
- `compiler-throughput`
  - tiers: `acceptance, runnable`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `inspect:objc3c:compiler-throughput`, `test:objc3c:compiler-throughput`, `test:objc3c:runnable-compiler-throughput`
- `conformance-corpus`
  - tiers: `acceptance, runnable`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:objc3c:conformance-corpus`, `test:objc3c:runnable-conformance-corpus`
- `distribution-credibility`
  - tiers: `acceptance, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:distribution-credibility:schemas`, `check:objc3c:distribution-credibility:surface`, `inspect:objc3c:distribution-credibility`, `publish:objc3c:distribution-credibility`, `test:objc3c:distribution-credibility`, `test:objc3c:distribution-credibility:e2e`
- `docs`
  - tiers: `acceptance, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `build:docs:native`, `build:site`, `check:docs:native`, `check:docs:surface`, `check:site`, `test:docs`
- `external-validation`
  - tiers: `acceptance, integration, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:external-validation:surface`, `test:objc3c:external-validation`, `test:objc3c:external-validation:integration`, `test:objc3c:external-validation:replay`
- `onboarding`
  - tiers: `acceptance`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:getting-started`
- `packaging-channels`
  - tiers: `acceptance, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:packaging-channels:schemas`, `check:objc3c:packaging-channels:surface`, `test:objc3c:packaging-channels`, `test:objc3c:packaging-channels:e2e`
- `performance`
  - tiers: `acceptance, runnable`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `inspect:objc3c:performance`, `inspect:objc3c:performance-dashboard`, `inspect:objc3c:runtime-performance`, `publish:objc3c:performance-report`, `test:objc3c:performance`, `test:objc3c:runnable-performance`, `test:objc3c:runnable-runtime-performance`, `test:objc3c:runtime-performance`
- `performance-governance`
  - tiers: `acceptance, integration, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:performance-governance:schemas`, `check:objc3c:performance-governance:surface`, `test:objc3c:performance-governance`, `test:objc3c:performance-governance:e2e`, `test:objc3c:performance-governance:integration`
- `public-conformance`
  - tiers: `acceptance, integration, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:public-conformance:schemas`, `check:objc3c:public-conformance:surface`, `inspect:objc3c:public-conformance:scorecard`, `publish:objc3c:public-conformance`, `test:objc3c:public-conformance`, `test:objc3c:public-conformance:e2e`, `test:objc3c:public-conformance:integration`
- `release-foundation`
  - tiers: `acceptance, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:release-foundation:schemas`, `check:objc3c:release-foundation:surface`, `test:objc3c:release-foundation`
- `release-operations`
  - tiers: `acceptance, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:objc3c:release-operations:schemas`, `check:objc3c:release-operations:surface`, `publish:objc3c:release-operations`, `test:objc3c:release-operations`, `test:objc3c:release-operations:e2e`
- `repo-shape`
  - tiers: `acceptance, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:repo:surface`, `test:repo`
- `runtime-architecture`
  - tiers: `acceptance`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:objc3c:runtime-architecture`
- `runtime-closure`
  - tiers: `acceptance, runnable`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `test:objc3c:block-arc-conformance`, `test:objc3c:concurrency-conformance`, `test:objc3c:error-conformance`, `test:objc3c:interop-conformance`, `test:objc3c:metaprogramming-conformance`, `test:objc3c:object-model-conformance`, `test:objc3c:release-candidate-conformance`, `test:objc3c:runnable-block-arc`, `test:objc3c:runnable-bootstrap`, `test:objc3c:runnable-concurrency`, `test:objc3c:runnable-error`, `test:objc3c:runnable-interop`, `test:objc3c:runnable-metaprogramming`, `test:objc3c:runnable-object-model`, `test:objc3c:runnable-release-candidate`, `test:objc3c:runnable-storage-reflection`, `test:objc3c:storage-reflection-conformance`
- `showcase`
  - tiers: `acceptance, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:showcase:surface`, `test:showcase`, `test:showcase:e2e`
- `stdlib`
  - tiers: `acceptance, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `build:objc3c:stdlib`, `check:stdlib:surface`, `test:stdlib`, `test:stdlib:advanced`, `test:stdlib:advanced:e2e`, `test:stdlib:e2e`, `test:stdlib:program`, `test:stdlib:program:e2e`
- `stress`
  - tiers: `acceptance, integration, runnable, static-guard`
  - owner: `scripts/objc3c_public_workflow_runner.py`
  - package_scripts: `check:stress:surface`, `test:objc3c:fuzz-safety`, `test:objc3c:lowering-runtime-stress`, `test:objc3c:stress`, `test:objc3c:stress-crash-triage`, `test:objc3c:stress-minimization`, `test:objc3c:stress:e2e`, `test:objc3c:stress:integration`

## Aggregate entrypoints
- `test:fast` -> `test-fast` (developer-fast-aggregate)
- `test:objc3c:full` -> `test-full` (developer-full-aggregate)
- `test:objc3c:nightly` -> `test-nightly` (nightly-aggregate)

Next issues: `M313-D001`, `M313-D002`
