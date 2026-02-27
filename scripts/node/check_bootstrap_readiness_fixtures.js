'use strict';

const { spawnSync } = require('child_process');

const args = [
  'scripts/run_bootstrap_readiness.py',
  '--issues-json',
  'tests/tooling/fixtures/bootstrap_readiness/all_zero/issues.json',
  '--milestones-json',
  'tests/tooling/fixtures/bootstrap_readiness/all_zero/milestones.json',
  '--catalog-json',
  'tests/tooling/fixtures/bootstrap_readiness/all_zero/catalog.json',
  '--refresh-open-blockers',
  '--open-blockers-root',
  'spec/planning',
  '--open-blockers-generated-at-utc',
  '2026-02-25T08:00:00Z',
  '--open-blockers-source',
  'fixture:bootstrap-readiness-ci',
  '--output-dir',
  'reports/bootstrap_readiness/ci_fixture_zero_open',
  '--run-spec-lint',
];

const completed = spawnSync('python', args, { stdio: 'inherit' });
if (completed.error) {
  throw completed.error;
}
process.exit(completed.status === null ? 1 : completed.status);
