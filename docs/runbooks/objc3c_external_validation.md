# ObjC 3 External Validation

This runbook defines the live `M303` boundary for external fixture intake,
independent replay, and ecosystem-facing credibility work.

## Working Boundary

Use these checked-in roots directly:

- intake and replay contract roots:
  - `tests/tooling/fixtures/external_validation/`
  - `tests/tooling/fixtures/objc3c/`
  - `tests/conformance/`
- live replay and validation scripts:
  - `scripts/check_objc3c_parser_replay_proof.ps1`
  - `scripts/check_objc3c_diagnostics_replay_proof.ps1`
  - `scripts/check_objc3c_lowering_replay_proof.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/check_objc3c_conformance_corpus_integration.py`
  - `scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py`
- machine-owned outputs only:
  - `tmp/reports/external-validation/`
  - `tmp/artifacts/external-validation/`

The external-validation surface is layered on the existing checked-in corpus,
replay proofs, and runnable package contracts. External evidence must terminate
in the same live compile, replay, and executable validation paths that already
back internal conformance claims.

## Architecture

External fixture intake stays on one path:

1. Normalize incoming ecosystem evidence into checked-in fixture contracts.
2. Map each accepted fixture to an existing conformance or replay family.
3. Replay it through the live compiler/runtime path instead of a sidecar tool.
4. Publish machine-owned replay and credibility summaries under `tmp/`.

Independent replay means:

- the accepted fixture can be rebuilt and rerun from checked-in scripts
- replay uses the live compiler/runtime path
- the emitted summary can be traced back to a checked-in fixture contract

## Trust And Triage Policy

Checked-in policy contract:

- `tests/tooling/fixtures/external_validation/trust_policy.json`

Allowed trust states:

- `candidate`
- `accepted`
- `quarantined`
- `rejected`

Fail-closed rule:

- external evidence is publishable only when it reaches `accepted`
- missing provenance, replay nondeterminism, or unresolved disclosure risk must
  hold the fixture in `quarantined` or `rejected`

## Normalization Semantics

Checked-in normalization manifest:

- `tests/tooling/fixtures/external_validation/intake_manifest.json`

Normalization rules:

- every accepted external fixture must map onto one checked-in conformance case
  or one checked-in replay contract
- every normalized entry must name the live replay script that proves the claim
- provenance stays attached to the normalized entry instead of living in ad hoc
  notes or issue comments
- raw ecosystem inputs do not become a second executable corpus root

## Quarantine And Disclosure Diagnostics

Checked-in quarantine manifest:

- `tests/tooling/fixtures/external_validation/quarantine_manifest.json`

Compatibility rules:

- quarantined and rejected fixtures must carry an explicit
  `OBJC3-EXTERNAL-EVIDENCE-*` diagnostic id
- quarantined fixtures may remain `internal-only` or `redacted-summary`
- rejected fixtures stay `blocked`
- escalation targets must stay explicit so later tooling can route unresolved
  evidence without inventing a sidecar review workflow

## Artifact Surface

Checked-in artifact contract:

- `tests/tooling/fixtures/external_validation/artifact_surface.json`

Stable machine-owned roots:

- `tmp/artifacts/external-validation/`
- `tmp/reports/external-validation/`

Stable machine-owned summaries:

- `tmp/reports/external-validation/source-surface-summary.json`
- `tmp/reports/external-validation/intake-replay-summary.json`
- `tmp/reports/external-validation/publication-summary.json`
- `tmp/reports/external-validation/drill-summary.json`
- `tmp/reports/external-validation/integration-summary.json`
- `tmp/reports/external-validation/end-to-end-summary.json`

## Explicit Non-Goals

- no separate external-fixture compiler harness
- no second corpus root outside `tests/conformance/` and
  `tests/tooling/fixtures/objc3c/`
- no dashboard-only or prose-only ecosystem claims
- no manual-only replay flow that bypasses the checked-in scripts

## Live Paths Later Issues Must Reuse

Checked-in corpus and replay surfaces:

- `tests/conformance/corpus_surface.json`
- `tests/conformance/longitudinal_suites.json`
- `tests/tooling/fixtures/objc3c/`
- `docs/runbooks/objc3c_conformance_corpus.md`
- `docs/runbooks/objc3c_stress_validation.md`

Live replay and evidence surfaces:

- `scripts/check_objc3c_parser_replay_proof.ps1`
- `scripts/check_objc3c_diagnostics_replay_proof.ps1`
- `scripts/check_objc3c_lowering_replay_proof.ps1`
- `scripts/check_objc3c_execution_replay_proof.ps1`
- `scripts/check_release_evidence.py`

Packaged reproducibility surfaces:

- `scripts/package_objc3c_runnable_toolchain.ps1`
- `scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py`

Current checked-in source-surface check:

- `python scripts/check_external_validation_source_surface.py`
- `tests/tooling/fixtures/external_validation/source_surface.json`

Current intake and replay tooling:

- `python scripts/run_objc3c_external_validation_replay.py`
- `python scripts/publish_objc3c_external_repro_corpus.py`

## Claim Boundary

External credibility claims are only valid when they resolve back to:

- a checked-in normalized fixture contract
- a checked-in replay or conformance family
- machine-owned replay output under `tmp/`
- the live compile/runtime path

Later milestone issues may widen normalization, quarantine, publication, and
workflow coverage, but they must stay on this boundary.
