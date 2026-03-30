# objc3c Performance Governance Boundary

## Working Boundary

This runbook defines the live `M296` boundary for performance budgets,
regression diagnosis, report publication, and release-ready evidence.

Use these checked-in surfaces directly:

- upstream live performance reports:
  - `tmp/reports/performance/benchmark-summary.json`
  - `tmp/reports/performance/comparative-baselines-summary.json`
  - `tmp/reports/compiler-throughput/benchmark-summary.json`
  - `tmp/reports/runtime-performance/benchmark-summary.json`
  - `tmp/reports/runtime-performance/integration-summary.json`
- checked-in benchmark and workload contracts:
  - `docs/runbooks/objc3c_performance.md`
  - `docs/runbooks/objc3c_compiler_throughput.md`
  - `docs/runbooks/objc3c_runtime_performance.md`
  - `tests/tooling/fixtures/performance/`
  - `tests/tooling/fixtures/compiler_throughput/`
  - `tests/tooling/fixtures/runtime_performance/`
- checked-in governance contracts:
  - `tests/tooling/fixtures/performance_governance/budget_model.json`
  - `tests/tooling/fixtures/performance_governance/source_surface.json`
  - `tests/tooling/fixtures/performance_governance/claim_policy.json`
  - `tests/tooling/fixtures/performance_governance/breach_triage_policy.json`
  - `tests/tooling/fixtures/performance_governance/lab_policy.json`

Machine-owned governance outputs must stay under:

- `tmp/reports/performance-governance/`
- `tmp/artifacts/performance-governance/`

## Budget Model And Breach Taxonomy

The live budget model is checked in at
`tests/tooling/fixtures/performance_governance/budget_model.json`.

Current budget families:

- `compile-throughput`
  - objective: bound cold compile, cache-hit proof, invalidation, and
    docs-generation regression against the checked-in native compiler path
- `runtime-hot-path`
  - objective: bound startup, dispatch, reflection, and ownership-helper timing
    against the checked-in runtime probe path
- `comparative-baseline`
  - objective: keep comparative-baseline publication honest and fail closed when
    reference languages are unavailable or materially noisier than the checked
    in machine profile allows
- `publication-freshness`
  - objective: prevent stale benchmark evidence from being promoted into release
    or dashboard summaries

Current breach taxonomy:

- `hard-regression`
  - budget exceeded beyond the release-blocking threshold
- `soft-regression`
  - budget exceeded beyond the warning threshold but below release-blocking
- `coverage-gap`
  - required upstream benchmark evidence is missing, unavailable, or stale
- `environment-drift`
  - machine profile or noise envelope no longer matches the checked-in
    performance lab contract
- `waived-regression`
  - a temporary checked-in waiver explains the exception and expiration

## Publishable Source Surface

The authoritative publishable source surface is checked in at
`tests/tooling/fixtures/performance_governance/source_surface.json`.

Only these checked-in sources may feed the public performance report:

- live benchmark and integration summaries emitted by the existing performance,
  compiler-throughput, and runtime-performance workflows
- checked-in budget, claim, breach-triage, and lab-policy contracts
- checked-in public command and maintainer workflow surfaces
- deterministic report-build scripts in `scripts/`

No spreadsheet-only, screenshot-only, or hand-edited sidecar performance report
is allowed.

## Public Claim And Waiver Policy

The authoritative policy is checked in at
`tests/tooling/fixtures/performance_governance/claim_policy.json`.

Current publishable claim classes:

- `release-ready`
  - all required benchmark families passed within budget and no expired waiver
    is active
- `caution`
  - publishable with explicit regression or environment caveats
- `blocked`
  - missing evidence, hard regression, environment drift, or expired waiver
    prevents publication

Waivers are only valid when they are:

- checked in
- attached to a named budget family and breach class
- time-bounded
- linked to the command/report paths that prove the remaining evidence state

## Breach Diagnosis And Triage Policy

The authoritative triage semantics are checked in at
`tests/tooling/fixtures/performance_governance/breach_triage_policy.json`.

Every breach diagnosis must resolve back to:

- the budget family and metric id that failed
- the child summary path that established the failure
- the severity band and operator action
- whether the failure is attributable to code regression, source coverage gap,
  or environment drift

The report is not allowed to emit an unclassified regression.

## Stable Lab And Noise-Control Policy

The authoritative lab policy is checked in at
`tests/tooling/fixtures/performance_governance/lab_policy.json`.

The performance program assumes one checked-in lab contract:

- benchmark publication is local-machine and non-portable unless the machine
  profile, sample counts, and noise envelope match the checked-in policy
- environment drift must fail closed when CPU, OS, executable provenance, or
  sample-variance bounds leave the approved range
- comparative baselines may be marked unavailable, but not silently omitted

## Explicit Non-Goals

- no sidecar benchmark app or spreadsheet-only dashboard path
- no budget claim without a checked-in policy contract and machine-owned child
  report
- no waiver stored only in issue text or release notes
- no public report that bypasses the existing performance benchmark surfaces

## Live Paths Later Issues Must Reuse

- benchmark and comparison roots:
  - `scripts/benchmark_objc3c_performance.py`
  - `scripts/run_objc3c_comparative_baselines.py`
  - `scripts/check_objc3c_compiler_throughput_integration.py`
  - `scripts/check_objc3c_runtime_performance_integration.py`
- package and workflow surfaces:
  - `scripts/objc3c_public_workflow_runner.py`
  - `package.json`
  - `scripts/build_objc3c_native.ps1`
  - `scripts/package_objc3c_runnable_toolchain.ps1`
- public workflow documentation:
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `docs/runbooks/objc3c_maintainer_workflows.md`
