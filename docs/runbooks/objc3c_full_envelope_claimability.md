# objc3c Full-Envelope Claimability

## Scope

This runbook defines the checked-in support matrix, claim taxonomy, and demotion
model for narrow production-strength claims over the intended objc3c
language/runtime envelope.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/full_envelope_claimability/support_matrix_claim_taxonomy.json`
- `tests/tooling/fixtures/full_envelope_claimability/production_strength_claim_policy.json`

Replayable generators and validators:

- `python scripts/build_full_envelope_claimability_support_matrix_summary.py`
- `python scripts/build_full_envelope_claimability_claim_policy_summary.py`

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
- required trust, publication, or compatibility evidence is missing
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
- `scripts/objc3c_public_workflow_runner.py`

## Non-Goals

- no blanket claim that every intended future topology is already supported
- no widening of public runtime ABI beyond the checked-in runtime closure
  boundaries
- no hand-maintained support spreadsheet or marketing-only badge surface
- no release claim that bypasses the integrated conformance, performance,
  packaging, update, or trust reports

## Successor Tracks

Follow-on work for stronger claims or wider support belongs in:

- `M325` developer tooling, LSP, formatting, and debugger integration
- `M326` cross-platform, toolchain-matrix, and compatibility hardening
- `M327` security hardening, macro trust, and supply-chain resilience
- `M328` compatibility maintenance, migrations, soak, and long-horizon operations
- `M329` package manager, registry, and dependency workflow ecosystem
- `M330` testing framework, templates, and canonical application architecture surfaces
- `M331` adoption program, migration playbooks, and ecosystem legibility
- `M332` governance, extension lifecycle, and ecosystem sustainability
