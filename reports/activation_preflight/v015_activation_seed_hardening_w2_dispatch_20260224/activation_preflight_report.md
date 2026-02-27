# Activation Preflight Orchestration

## Inputs

- Issues snapshot: `tmp/v015_m22_open_issues_snapshot.json`
- Milestones snapshot: `tmp/v015_m22_open_milestones_snapshot.json`
- Catalog JSON: `spec/planning/remaining_task_review_catalog.json`
- Open blockers JSON: _none_
- Spec lint globs: _default spec_lint globs_

## Snapshot Refresh + Freshness

- Snapshot refresh requested: `true`
- Snapshot refresh attempted: `true`
- Snapshot refresh exit code: `0`
- Issues snapshot path: `tmp/v015_m22_open_issues_snapshot.json`
- Milestones snapshot path: `tmp/v015_m22_open_milestones_snapshot.json`
- Open blockers snapshot path: _none_
- Issues max age seconds: _none_
- Milestones max age seconds: _none_
- Snapshot generated_at_utc override: _none_

## Activation State

- Gate open: `true`
- Activation required: `True`
- Queue state: `dispatch-open`
- Active trigger IDs: `T1-ISSUES`, `T2-MILESTONES`
- Open blocker count: `0`
- Open blockers trigger fired: `false`
- Activation checker exit code: `1`

## Spec Lint

- spec_lint exit code: `0`
- spec_lint ok: `true`

## Final Outcome

- Final status: `activation-open`
- Final exit code: `1`

## Artifacts

- Output directory: `reports/activation_preflight/v015_activation_seed_hardening_w2_dispatch_20260224`
- Activation JSON: `check_activation_triggers.json`
- Activation markdown: `check_activation_triggers.md`
- spec_lint log: `spec_lint.log`
- Snapshot capture log: `capture_activation_snapshots.log`
- Summary JSON: `activation_preflight_summary.json`
- Report markdown: `activation_preflight_report.md`

## Command Exit Codes

| Command | Exit Code |
| --- | --- |
| `check_activation_triggers_json` | `1` |
| `check_activation_triggers_markdown` | `1` |
| `spec_lint` | `0` |
| `capture_activation_snapshots` | `0` |

## Errors

- _none_
