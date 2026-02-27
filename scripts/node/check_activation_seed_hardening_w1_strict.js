'use strict';

const { spawnSync } = require('child_process');

const baseArgs = [
  'scripts/check_activation_triggers.py',
  '--issues-json',
  'tests/tooling/fixtures/activation_triggers/zero_open/issues.json',
  '--milestones-json',
  'tests/tooling/fixtures/activation_triggers/zero_open/milestones.json',
  '--catalog-json',
  'tests/tooling/fixtures/activation_triggers/zero_open/catalog.json',
  '--format',
  'json',
];

function fail(message) {
  console.error(`activation-seed-hardening: FAIL (${message})`);
  process.exit(1);
}

function run(openBlockersPath, expectedExit, expectedActiveIds, expectedOpenBlockerCount, expectedQueueState, label) {
  const completed = spawnSync('python', [...baseArgs, '--open-blockers-json', openBlockersPath], { encoding: 'utf8' });
  if (completed.error) {
    throw completed.error;
  }
  if (completed.status !== expectedExit) {
    fail(`${label}_exit=${completed.status} expected=${expectedExit}`);
  }
  if (completed.stderr && completed.stderr.trim()) {
    fail(`${label}_stderr=${JSON.stringify(completed.stderr.trim())}`);
  }

  let payload;
  try {
    payload = JSON.parse(completed.stdout);
  } catch (error) {
    fail(`${label}_invalid_json=${error.message}`);
  }

  const activeIds = payload.active_trigger_ids;
  if (!Array.isArray(activeIds) || activeIds.length !== expectedActiveIds.length || activeIds.some((value, index) => value !== expectedActiveIds[index])) {
    fail(`${label}_active_trigger_ids=${JSON.stringify(activeIds)} expected=${JSON.stringify(expectedActiveIds)}`);
  }

  const openBlockers = payload.open_blockers;
  if (!openBlockers || openBlockers.count !== expectedOpenBlockerCount) {
    fail(`${label}_open_blocker_count=${openBlockers && openBlockers.count} expected=${expectedOpenBlockerCount}`);
  }

  if (payload.queue_state !== expectedQueueState) {
    fail(`${label}_queue_state=${payload.queue_state} expected=${expectedQueueState}`);
  }
}

run('tests/tooling/fixtures/activation_triggers/zero_open/open_blockers.json', 0, [], 0, 'idle', 'zero');
run('tests/tooling/fixtures/activation_triggers/zero_open/open_blockers_nonzero.json', 1, ['T5-OPEN-BLOCKERS'], 2, 'dispatch-open', 'nonzero');
console.log('activation-seed-hardening: OK (zero_exit=0, nonzero_exit=1, trigger=T5-OPEN-BLOCKERS, zero_queue_state=idle, nonzero_queue_state=dispatch-open)');
