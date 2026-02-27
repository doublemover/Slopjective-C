'use strict';

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const root = process.cwd();
const fixtureDir = path.join(root, 'tests', 'tooling', 'fixtures', 'activation_triggers', 'preflight', 'zero_open');
const outputDir = path.join(root, 'reports', 'activation_preflight', 'ci_fixture_zero_open');
const snapshotDir = path.join(root, 'tmp', 'activation_preflight', 'm03_strict', 'snapshot_inputs');

const generatedAtUtc = new Date().toISOString();
const normalize = (targetPath) => path.relative(root, targetPath).replace(/\\/g, '/');

function loadRows(sourcePath) {
  const payload = JSON.parse(fs.readFileSync(sourcePath, 'utf8'));
  if (Array.isArray(payload)) {
    return payload;
  }
  if (payload && typeof payload === 'object') {
    for (const key of ['items', 'open', 'issues', 'milestones']) {
      if (Array.isArray(payload[key])) {
        return payload[key];
      }
    }
  }
  throw new Error(`unsupported fixture snapshot shape: ${normalize(sourcePath)}`);
}

fs.mkdirSync(snapshotDir, { recursive: true });

for (const name of ['issues', 'milestones']) {
  const sourcePath = path.join(fixtureDir, `${name}.json`);
  const rows = loadRows(sourcePath);
  const wrapped = {
    generated_at_utc: generatedAtUtc,
    source: `fixture:${normalize(sourcePath)}`,
    count: rows.length,
    items: rows,
  };
  const outputPath = path.join(snapshotDir, `${name}.json`);
  fs.writeFileSync(outputPath, `${JSON.stringify(wrapped, null, 2)}\n`, 'utf8');
}

const runArgs = [
  'scripts/run_activation_preflight.py',
  '--issues-json',
  normalize(path.join(snapshotDir, 'issues.json')),
  '--milestones-json',
  normalize(path.join(snapshotDir, 'milestones.json')),
  '--catalog-json',
  'tests/tooling/fixtures/activation_triggers/preflight/zero_open/catalog.json',
  '--open-blockers-json',
  'tests/tooling/fixtures/activation_triggers/preflight/zero_open/open_blockers.json',
  '--issues-max-age-seconds',
  '86400',
  '--milestones-max-age-seconds',
  '86400',
  '--output-dir',
  normalize(outputDir),
];

const completed = spawnSync('python', runArgs, { stdio: 'inherit' });
if (completed.error) {
  throw completed.error;
}
process.exit(completed.status === null ? 1 : completed.status);
