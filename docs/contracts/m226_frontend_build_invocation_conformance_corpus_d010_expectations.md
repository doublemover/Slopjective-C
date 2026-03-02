# M226 Frontend Build and Invocation Conformance Corpus Expectations (D010)

Contract ID: `objc3c-frontend-build-invocation-conformance-corpus/m226-d010-v1`
Status: Accepted
Scope: Concrete conformance corpus expansion for frontend wrapper invocation profiles.

## Objective

Freeze a deterministic conformance corpus that expands the D009 matrix into
executable acceptance/rejection case rows and fail-closed wrapper coverage.

## Required Invariants

1. Build script emits a D010 conformance corpus artifact:
   - `tmp/artifacts/objc3c-native/frontend_conformance_corpus.json`
2. D010 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-conformance-corpus/m226-d010-v1`
3. D010 artifact depends on:
   - `objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1`
   - `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
4. D010 corpus includes:
   - acceptance corpus rows keyed by D009 profile key
   - rejection corpus rows pinning deterministic diagnostics and exit code `2`
5. Wrapper validates D010 in both no-cache and cache-aware execution paths.
6. Wrapper fail-closes unless at least one acceptance corpus case exists for the
   computed D009 invocation profile key.

## Validation

- `python scripts/check_m226_d010_frontend_build_invocation_conformance_corpus_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d010_frontend_build_invocation_conformance_corpus_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-D010/frontend_build_invocation_conformance_corpus_summary.json`
