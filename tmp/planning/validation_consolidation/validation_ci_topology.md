# Validation CI Topology

- issue: `M313-D001`

## Aggregate schedules
- `test:fast` -> `5` families
  - families: `aggregate-validation`, `docs`, `repo-shape`, `showcase`, `onboarding`
- `test:objc3c:full` -> `12` families
  - families: `aggregate-validation`, `docs`, `repo-shape`, `showcase`, `onboarding`, `stdlib`, `performance`, `compiler-throughput`, `runtime-architecture`, `release-foundation`, `packaging-channels`, `release-operations`
- `test:objc3c:nightly` -> `20` families
  - families: `aggregate-validation`, `docs`, `repo-shape`, `showcase`, `onboarding`, `stdlib`, `performance`, `compiler-throughput`, `runtime-architecture`, `release-foundation`, `packaging-channels`, `release-operations`, `bonus-experiences`, `conformance-corpus`, `stress`, `external-validation`, `public-conformance`, `performance-governance`, `distribution-credibility`, `runtime-closure`

Next issue: `M313-D002`
