# objc3c Full-Envelope Claimability

## Scope

This runbook defines the checked-in support matrix, claim taxonomy, and demotion
model for narrow production-strength claims over the intended objc3c
language/runtime envelope.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/full_envelope_claimability/support_matrix_claim_taxonomy.json`
- `tests/tooling/fixtures/full_envelope_claimability/production_strength_claim_policy.json`
- `tests/tooling/fixtures/full_envelope_claimability/release_blocker_rollout_policy.json`
- `tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json`

Replayable public workflow actions:

- `npm run objc3c -- validate-release-candidate-conformance`
- `npm run objc3c -- validate-runnable-release-candidate`
- `npm run objc3c -- validate-public-conformance-reporting`
- `npm run objc3c -- validate-performance-governance`
- `npm run objc3c -- validate-release-foundation`
- `npm run objc3c -- validate-release-operations`
- `npm run objc3c -- validate-distribution-credibility`

Helper implementations are action-catalog anchors for
evidence generation and claim classification. They are not separate public
commands.

## Claim Taxonomy

The canonical support classes for full-envelope claims are:

- `supported`: the surface is backed by the current checked-in contracts plus
  passing integrated evidence on the live workflow path
- `experimental`: the surface is visible and intentionally documented, but one
  or more evidence families or support-window guarantees remain narrower than
  normal production claims
- `unsupported`: the surface is explicitly outside the current supported
  boundary and must stay fail-closed, diagnostic-backed, or comparison-only
- `release-blocking`: the surface cannot be claimed for release while required
  evidence, packaging, or trust signals are missing or failing

These classes apply to public and internal claim surfaces equally. No release
note, dashboard, tutorial, showcase, or README statement may imply a wider
class than the checked-in matrix.

Support classification and public-claim drift are action-catalog-owned evidence
steps inside the full-envelope workflow. They validate the checked-in taxonomy,
reject unsupported claim widening, and emit machine-owned summaries under
`tmp/`/`reports/` without becoming public commands themselves.

## Production-Strength Claim And Support-Window Policy

Production-strength claims are allowed only when:

- the surface is currently classified as `supported`
- required integrated evidence families are passing
- the release channel and support window are consistent with
  `docs/runbooks/objc3c_release_operations.md`
- the public claim surface stays narrower than the current runtime-owned helper,
  package, trust, and fail-closed boundaries

Current support-window rules for production-strength claims:

- `stable`: allowed to carry production-strength claims for supported surfaces
- `candidate`: allowed to carry pre-release production-strength claims only when
  the same evidence stack is passing and the claim remains explicitly
  candidate-scoped
- `preview`: never carries production-strength claims; at most it may publish
  experimental or unsupported guidance

No production-strength claim may widen support beyond the checked-in release
channel, support-window, or runtime-boundary contracts.

## Release-Blocker And Rollout Criteria

The full-envelope release gate is explicit and machine-resolved:

- `stable` rollout requires passing required evidence families, a
  production-strength claim class, non-blocked performance governance,
  publishable release-foundation and release-operations artifacts, and a `ready`
  distribution-trust state
- `candidate` rollout may proceed only for candidate-scoped claims and only when
  the same evidence stack is coherent enough to avoid `release-blocking`
- `preview` rollout may publish exploratory or experimental guidance, but it is
  never treated as production-strength

The canonical release blockers for this milestone are:

- a required integration report is missing or not `PASS`
- public conformance reporting is `blocked`
- performance governance is `blocked` or not claim-ready
- release-foundation publication artifacts are missing
- release-operations support-window or update artifacts are missing
- distribution credibility is not `ready`
- a surface is marked `unsupported` but is being promoted as `supported`
- the full-envelope dashboard projection would publish `preview-only` or
  `candidate-scoped` instead of `production-strength`

This milestone may conclude that the current envelope remains release-blocked.
The policy surface states that through checked-in contract fields rather than
treating passing integration scripts as sufficient release evidence.

The release-blocker summary owns the dashboard release-blocker projection. The
dashboard consumes that projection and must not invent a separate claim class,
blocker list, or production-strength decision. Any dashboard state that is not
`production-strength` is a stable-release blocker until the upstream evidence
and release-blocker summary both converge.

## Stability Regression And Rollout Implementation

The canonical rollout-readiness summary for this milestone must be derived from:

- the full-envelope support matrix summary
- the production-strength claim policy summary
- the release-blocker and rollout summary
- the live public-conformance, performance-governance, release-foundation,
  release-operations, and distribution-credibility integration reports

The derived rollout summary must make these decisions explicit:

- current rollout class: `stable`, `candidate`, or `preview`
- whether the envelope is currently production-strength claimable
- which active regressions or blocker states forced demotion
- which release and trust artifacts remain publishable even when the envelope is
  not stable-rollout ready

## Dashboard And Claim Publication Surface

The machine-owned envelope claim outputs for this milestone must stay under:

- `tmp/reports/full-envelope-claimability/`
- `tmp/artifacts/full-envelope-claimability/`

The checked-in contract and schema surface for those outputs is:

- `tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json`
- `schemas/objc3c-full-envelope-dashboard-summary-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

This runbook cites the registry-backed schema file as the dashboard shape owner.
It does not define a local dashboard schema fragment, and generated dashboard
summaries remain projections over canonical support/evidence inputs rather than
standalone support claims.

The envelope dashboard is a projection over the support matrix, claim policy,
release-blocker summary, rollout-readiness summary, and the live conformance,
performance, release, and trust integration reports. Its claim class, blocker
state, acceptance matrix, and release artifact fields are owned by checked-in
contracts.

The dashboard must carry the `dashboard_release_blocker_projection` emitted by
the full-envelope release-blocker summary step in the workflow. The
projection names the dashboard/public-summary output paths, required dashboard
fields, current rollout class, public claim class, and whether the dashboard
blocks production-strength release claims. The dashboard builder fails if its
derived public claim class drifts from this release-blocker projection.

## Soak, Stress, And External Validation Integration

Full-envelope claimability must keep the long-running validation surfaces live:

- conformance corpus integration
- stress integration
- external validation integration
- public-conformance publication that projects over those validation families

Later evidence packaging and closeout gates must consume the shared full-envelope
dashboard and the live validation integrations instead of publishing a second
soak or external-validation truth surface.

## Release-Candidate Evidence Packaging

The full-envelope evidence package for this milestone must terminate in the live:

- release-foundation manifest, SBOM, and attestation
- release-operations update manifest, support-window report, and channel catalog
- distribution-credibility trust report
- full-envelope dashboard and public summary

This packaging surface is allowed to prove that release artifacts are coherent
even when the full envelope is not yet stable-rollout ready.

## Evidence Families

Full-envelope claimability is derived from these integrated evidence families:

- conformance corpus
- stress integration
- external validation
- public conformance reporting
- performance governance
- release foundation
- release operations
- distribution credibility

The support matrix projects over those live evidence families plus the checked-in
runtime-closure contracts from:

- `docs/runbooks/objc3c_object_model_closure.md`
- `docs/runbooks/objc3c_block_arc_closure.md`
- `docs/runbooks/objc3c_error_runtime_closure.md`
- `docs/runbooks/objc3c_concurrency_runtime_closure.md`
- `docs/runbooks/objc3c_metaprogramming_interop_closure.md`

The full-envelope matrix is not a second truth source. It is an envelope-level
classification over the same live runtime, conformance, performance, release,
and trust surfaces that already exist.

## Current Supported Envelope

The current supported full-envelope claim is intentionally narrow:

- object-model, property/ivar/reflection, block/ARC, error, concurrency, and
  metaprogramming/interop closures are supportable only on their currently
  documented runtime-owned helper, packaged-toolchain, and fail-closed
  boundaries
- release, update, package, and trust claims are supportable only on the
  existing machine-owned publication path
- public claims must resolve back to passing integrated evidence families; a
  closed milestone alone is not sufficient

## Demotion Model

Claims must be demoted when any of the following conditions hold:

- required integrated evidence is missing or not passing
- required packaged release or update evidence is missing
- required trust, publication, or support-window evidence is missing
- a surface is still intentionally fail-closed or explicitly unsupported in its
  checked-in runtime closure runbook

Demotion is explicit:

- supported -> experimental when the surface remains runnable and documented,
  but support-window or evidence breadth is intentionally narrow
- supported or experimental -> unsupported when the checked-in contracts or
  runtime closure runbooks still mark the surface as fail-closed or non-goal
- any class -> release-blocking when a required evidence family for release,
  publication, packaging, updates, or trust is missing or failing

## Public Claim Surfaces

Later issues must keep these public claim surfaces aligned with the support
matrix:

- `README.md`
- `docs/objc3c-native.md`
- `docs/runbooks/objc3c_public_conformance_reporting.md`
- `docs/runbooks/objc3c_performance_governance.md`
- `docs/runbooks/objc3c_release_foundation.md`
- `docs/runbooks/objc3c_release_operations.md`
- `docs/runbooks/objc3c_distribution_credibility.md`
- package bridge: `npm run objc3c -- <action>`

## Non-Goals

- no blanket claim that every intended future topology is already supported
- no widening of public runtime ABI beyond the checked-in runtime closure
  boundaries
- no hand-maintained support spreadsheet or marketing-only badge surface
- no release claim that bypasses the integrated conformance, performance,
  packaging, update, or trust reports

## Successor Tracks

Follow-on work for stronger claims or wider support belongs in:

- developer tooling, LSP, formatting, and debugger integration
- cross-platform, toolchain-matrix, and platform-support hardening
- security hardening, macro trust, and supply-chain resilience
- support-window maintenance, canonical conversion replay, soak, and long-horizon operations
- package manager, registry, and dependency workflow ecosystem
- testing framework, templates, and canonical application architecture surfaces
- adoption program, conversion playbooks, and ecosystem legibility
- governance, extension lifecycle, and ecosystem sustainability
