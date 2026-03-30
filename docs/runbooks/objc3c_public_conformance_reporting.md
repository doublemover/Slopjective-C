# ObjC 3 Public Conformance Reporting

This runbook defines the live `M304` boundary for public conformance
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
  - `scripts/check_release_evidence.py`

Machine-owned public-reporting outputs must stay under:

- `tmp/reports/public-conformance/`
- `tmp/artifacts/public-conformance/`

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
- a deterministic reporting script in `scripts/`

Public reporting must fail closed when upstream evidence is missing, stale,
quarantined, or not traceable to a checked-in validation family.

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

The public report may widen fields later, but it must stay schema-shaped and
traceable to checked-in contracts.

## Explicit Non-Goals

- no spreadsheet-only or prose-only claim publication path
- no second executable corpus rooted outside existing conformance and fixture
  surfaces
- no milestone-only badge or score calculation rules
- no public summary that cannot be traced back to machine-owned evidence

## Live Paths Later Issues Must Reuse

Later `M304` issues must stay on these paths:

- checked-in contracts and runbooks:
  - `tests/conformance/corpus_surface.json`
  - `tests/conformance/longitudinal_suites.json`
  - `tests/tooling/fixtures/external_validation/`
  - `docs/runbooks/objc3c_conformance_corpus.md`
  - `docs/runbooks/objc3c_external_validation.md`
- existing evidence generation and validation scripts:
  - `scripts/generate_conformance_corpus_index.py`
  - `scripts/check_objc3c_conformance_corpus_integration.py`
  - `scripts/run_objc3c_external_validation_replay.py`
  - `scripts/publish_objc3c_external_repro_corpus.py`
  - `scripts/check_objc3c_external_validation_integration.py`
  - `scripts/check_release_evidence.py`
- checked-in schema surfaces:
  - `schemas/objc3-conformance-dashboard-status-v1.schema.json`
  - `schemas/objc3-conformance-evidence-bundle-v1.schema.json`

Later work may widen scoring, schema, publication, and workflow coverage, but
it must stay on this boundary.

## Current Checked-In Source Surface

- contract root: `tests/tooling/fixtures/public_conformance_reporting/`
- source check: `python scripts/check_public_conformance_reporting_source_surface.py`
- source summary: `tmp/reports/public-conformance/source-surface-summary.json`
- schema check: `python scripts/check_public_conformance_schema_surface.py`
- schema summary: `tmp/reports/public-conformance/schema-surface-summary.json`
- scorecard builder: `python scripts/build_objc3c_public_conformance_scorecard.py`
- scorecard summary: `tmp/reports/public-conformance/scorecard-summary.json`
- publication builder: `python scripts/publish_objc3c_public_conformance_report.py`
- public summary: `tmp/reports/public-conformance/public-summary.json`
- published scorecard artifact: `tmp/artifacts/public-conformance/scorecard/public-conformance-scorecard.json`
- published badge artifact: `tmp/artifacts/public-conformance/badge/public-conformance-badge.json`
- published Markdown report: `tmp/artifacts/public-conformance/report/public-conformance-report.md`
