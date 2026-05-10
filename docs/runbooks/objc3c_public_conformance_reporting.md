# ObjC 3 Public Conformance Reporting

This runbook defines the live `objc3c.reporting.publicconformance.v1` boundary for public conformance
reporting, stability scoring, dashboard publication, and third-party-legible
summaries.

## Working Boundary

Use these checked-in surfaces directly:

- conformance and replay inputs:
  - `tests/conformance/`
  - `tests/tooling/fixtures/external_validation/`
  - `docs/runbooks/objc3c_conformance_corpus.md`
  - `docs/runbooks/objc3c_external_validation.md`
- live upstream reports:
  - `tmp/reports/conformance/corpus-surface-summary.json`
  - `tmp/reports/conformance/corpus-index.json`
  - `tmp/reports/external-validation/source-surface-summary.json`
  - `tmp/reports/external-validation/intake-replay-summary.json`
  - `tmp/reports/external-validation/publication-summary.json`
- checked-in schema and release-evidence anchors:
  - `schemas/objc3-conformance-dashboard-status-v1.schema.json`
  - `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
  - `npm run objc3c -- check-release-evidence`

These schema anchors are owned by `scripts/objc3c_shared/schema_registry.py`;
runbook prose and public reports cite the registry-backed files instead of
copying schema fragments.

Machine-owned public-reporting outputs must stay under:

- `tmp/reports/public-conformance/`
- `tmp/artifacts/public-conformance/`

Owner-split source modules:

- contract and path models:
  `scripts/objc3c_workflow/actions/release_governance_public_conformance_contracts.py`
- typed data models:
  `scripts/objc3c_workflow/actions/release_governance_public_conformance_models.py`
- action payload fragments:
  `scripts/objc3c_workflow/actions/release_governance_public_conformance_action_fragments.py`

## Architecture

Public reporting stays on one path:

1. Load the checked-in corpus and external-validation contracts.
2. Load machine-owned evidence that was emitted by the live corpus and replay
   workflows.
3. Derive a machine-readable credibility and stability summary from those live
   reports.
4. Publish a third-party-legible report that resolves back to the same checked
   in contracts and machine-owned evidence.

The public report is not a second truth source. It is a projection over the
same live validation and publication artifacts that already back internal
conformance and external-validation claims.

## Claim Boundary

Publishable public conformance claims must resolve back to all of the
following:

- a checked-in corpus or external-validation contract
- a machine-owned report emitted by the live workflow
- a checked-in schema or release-evidence contract
- an action-catalog-owned deterministic report builder

Public reporting must fail closed when upstream evidence is missing, stale,
quarantined, or not traceable to a checked-in validation family.

Capability language is strict:

- `claim-ready` means all required upstream evidence owners report `PASS`, no
  schema anchors are missing, and no hard blocks remain.
- `provisional` means the report is publishable with explicit caution and
  deductions.
- `blocked` means the public artifact must state the blocking evidence and
  cannot imply conformance readiness.

## Credibility And Stability Policy

Checked-in policy contract:

- `tests/tooling/fixtures/public_conformance_reporting/stability_policy.json`

Current public score semantics:

- `claim-ready`: score `95-100`; all required upstream evidence passed and no
  blocked evidence was promoted into the public claim set
- `provisional`: score `70-94`; the report remains publishable but carries
  caution because some non-blocking credibility deductions were applied
- `blocked`: score `0-69`; upstream evidence or schema anchors are missing, or
  a required credibility gate failed

The score is not a free-standing marketing metric. It is a compact projection
over the live conformance corpus, external-validation, and checked-in
schema/release anchors.

## Schema Surface

Checked-in schema contract:

- `tests/tooling/fixtures/public_conformance_reporting/schema_surface.json`

Checked-in schema anchors:

- dashboard status schema: `schemas/objc3-conformance-dashboard-status-v1.schema.json`
- public scorecard schema: `schemas/objc3c-public-conformance-scorecard-v1.schema.json`
- public summary schema: `schemas/objc3c-public-conformance-summary-v1.schema.json`
- release-evidence bundle schema: `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

The public report may widen fields later, but it must stay schema-shaped and
traceable to registry-backed checked-in contracts. This runbook must not copy
public-conformance JSON Schema fragments or treat generated public reports as
support boundaries outside the capability matrix and evidence map.

## Explicit Non-Goals

- no spreadsheet-only or prose-only claim publication path
- no second executable corpus rooted outside existing conformance and fixture
  surfaces
- no milestone-only badge or score calculation rules
- no public summary that cannot be traced back to machine-owned evidence

## Live Paths Later Issues Must Reuse

Later public-conformance reporting work must stay on these paths:

- checked-in contracts and runbooks:
  - `tests/conformance/corpus_surface.json`
  - `tests/conformance/longitudinal_suites.json`
  - `tests/tooling/fixtures/external_validation/`
  - `docs/runbooks/objc3c_conformance_corpus.md`
  - `docs/runbooks/objc3c_external_validation.md`
- existing evidence generation and validation scripts:
  - `npm run objc3c -- validate-conformance-corpus`
  - `npm run objc3c -- validate-external-validation`
  - `npm run objc3c -- test-external-validation-replay`
  - `npm run objc3c -- publish-external-repro-corpus`
  - `npm run objc3c -- validate-external-validation-integration`
  - `npm run objc3c -- check-release-evidence`
- checked-in schema surfaces:
  - `schemas/objc3-conformance-dashboard-status-v1.schema.json`
  - `schemas/objc3-conformance-evidence-bundle-v1.schema.json`
  - `scripts/objc3c_shared/schema_registry.py`

Later work may widen scoring, schema, publication, and workflow coverage, but
it must stay on this boundary.

## Current Checked-In Source Surface

- contract root: `tests/tooling/fixtures/public_conformance_reporting/`
- workflow surface:
  `tests/tooling/fixtures/public_conformance_reporting/workflow_surface.json`
- action payload fragments:
  `scripts/objc3c_workflow/actions/release_governance_public_conformance_action_fragments.py`
- source check: `npm run objc3c -- check-public-conformance-reporting-surface`
- source summary: `tmp/reports/public-conformance/source-surface-summary.json`
- schema check: `npm run objc3c -- check-public-conformance-schema-surface`
- schema summary: `tmp/reports/public-conformance/schema-surface-summary.json`
- scorecard builder: `npm run objc3c -- build-public-conformance-scorecard`
- scorecard summary: `tmp/reports/public-conformance/scorecard-summary.json`
- publication builder: `npm run objc3c -- publish-public-conformance-report`
- public summary: `tmp/reports/public-conformance/public-summary.json`
- published scorecard artifact: `tmp/artifacts/public-conformance/scorecard/public-conformance-scorecard.json`
- published badge artifact: `tmp/artifacts/public-conformance/badge/public-conformance-badge.json`
- published Markdown report: `tmp/artifacts/public-conformance/report/public-conformance-report.md`

Helper implementations are owned by the release-governance public-conformance
action modules and exposed through the action catalog facade. They are not a
separate public reporting command surface.
