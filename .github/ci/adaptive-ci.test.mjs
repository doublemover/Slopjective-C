import assert from 'node:assert/strict';
import test from 'node:test';
import { resolveSource, calendarDaysSince, collectSnapshot, decide, localDate, relevantPath, summarizeAttempt, validateConfig } from './adaptive-ci.mjs';

const suite = {
  id: 'full', monthlyDay: 3, paths: ['**'],
  ignorePaths: ['README.md', 'docs/**/*.md'],
  protectedPaths: ['.github/**', 'docs/contracts/**'],
  expectedJobs: [{ name: 'test', requiredSteps: ['^Run tests$'] }],
};
const attempt = (at, extra = {}) => ({ at, started: true, complete: true, outcome: 'success', ...extra });
const change = (at, paths = ['src/main.ts']) => ({ at, paths });
const evaluate = (now, history = [], changes = [], override = {}) => decide({ suite: { ...suite, ...override }, now, history, changes });

test('configuration rejects duplicate suites and workflow path traversal', () => {
  const valid = { ...suite, workflow: 'test.yml', cron: '17 7 * * 2' };
  assert.equal(validateConfig({ version: 1, timezone: 'America/Detroit', suites: [valid] }).version, 1);
  assert.throws(() => validateConfig({ version: 1, suites: [valid, valid] }));
  assert.throws(() => validateConfig({ version: 1, suites: [{ ...valid, workflow: '../outside.yml' }] }));
});

test('initial monthly pass waits for the declared anchor', () => {
  assert.equal(evaluate('2026-11-02T12:00:00Z').due, false);
  assert.equal(evaluate('2026-11-03T12:00:00Z').due, true);
});
test('one inactive monthly attempt, including a failed attempt', () => {
  for (const outcome of ['success', 'failure', 'cancelled']) {
    const result = evaluate('2026-11-24T12:00:00Z', [attempt('2026-11-03T12:00:00Z', { outcome, complete: outcome === 'success' })]);
    assert.equal(result.due, false);
  }
});
test('relevant uncovered activity promotes after fourteen local days', () => {
  const history = [attempt('2026-11-03T12:00:00Z')];
  const changes = [change('2026-11-04T12:00:00Z')];
  assert.equal(evaluate('2026-11-16T12:00:00Z', history, changes).due, false);
  assert.equal(evaluate('2026-11-17T12:00:00Z', history, changes).due, true);
});
test('unrelated documentation alone does not promote', () => {
  assert.equal(evaluate('2026-11-17T12:00:00Z', [attempt('2026-11-03T12:00:00Z')], [change('2026-11-04T12:00:00Z', ['README.md', 'docs/guide.md', 'docs/guides/topic.md'])]).due, false);
});
test('machine-consumed contracts override prose exclusions', () => {
  assert.equal(relevantPath('docs/contracts/schema.md', suite), true);
  assert.equal(relevantPath('docs/guide.md', suite), false);
  assert.equal(relevantPath('.github/workflows/test.yml', suite), true);
});
test('activity ages out after twenty-eight local days', () => {
  const history = [attempt('2026-11-03T12:00:00Z', { complete: false, outcome: 'failure' })];
  assert.equal(evaluate('2026-11-30T12:00:00Z', history, [change('2026-11-02T12:00:00Z')]).due, true);
  assert.equal(evaluate('2026-11-30T12:00:00Z', history, [change('2026-11-01T12:00:00Z')]).due, false);
});
test('a complete pass covers earlier activity', () => {
  assert.equal(evaluate('2026-11-30T12:00:00Z', [attempt('2026-11-15T12:00:00Z')], [change('2026-11-14T12:00:00Z')]).due, false);
});
test('failed and incomplete passes never become successful coverage', () => {
  const history = [attempt('2026-11-03T12:00:00Z', { complete: false })];
  assert.equal(evaluate('2026-11-17T12:00:00Z', history, [change('2026-11-02T12:00:00Z')]).due, true);
});
test('month boundaries do not duplicate a recent active pass', () => {
  const history = [attempt('2026-11-28T12:00:00Z')];
  assert.equal(evaluate('2026-12-03T12:00:00Z', history, [change('2026-11-29T12:00:00Z')]).due, false);
  assert.equal(evaluate('2026-12-12T12:00:00Z', history, [change('2026-11-29T12:00:00Z')]).due, true);
});
test('a third fortnight in one month is a real occurrence, not silently dropped', () => {
  const history = [attempt('2026-12-01T12:00:00Z'), attempt('2026-12-15T12:00:00Z')];
  assert.equal(evaluate('2026-12-29T12:00:00Z', history, [change('2026-12-16T12:00:00Z')]).due, true);
});
test('DST fall transition uses calendar days, not 336 elapsed hours', () => {
  assert.equal(calendarDaysSince('2026-11-08T15:00:00Z', '2026-10-25T14:00:00Z', 'America/Detroit'), 14);
});
test('DST spring transition uses calendar days', () => {
  assert.equal(calendarDaysSince('2026-03-15T14:00:00Z', '2026-03-01T15:00:00Z', 'America/Detroit'), 14);
});
test('local date handles UTC midnight across a month boundary', () => {
  assert.equal(localDate('2026-11-01T02:00:00Z'), '2026-10-31');
});
test('private suite is paused through October', () => {
  assert.equal(evaluate('2026-10-20T12:00:00Z', [], [], { notBefore: '2026-11-01' }).cadence, 'paused');
});
test('inflight attempts prevent duplicate execution', () => {
  assert.equal(evaluate('2026-11-03T12:00:00Z', [attempt('2026-11-03T11:00:00Z', { inProgress: true })]).due, false);
});
test('truncated changed paths promote the safe full scope', () => {
  assert.equal(evaluate('2026-11-17T12:00:00Z', [attempt('2026-11-03T12:00:00Z')], [{ at: '2026-11-04T12:00:00Z', paths: [], unknownPaths: true }]).due, true);
});
test('future or malformed history/activity fails closed', () => {
  assert.throws(() => evaluate('2026-11-03T12:00:00Z', [attempt('2026-11-04T12:00:00Z')]));
  assert.throws(() => evaluate('2026-11-03T12:00:00Z', [], [change('not-a-date')]));
  assert.throws(() => relevantPath('../secret', suite));
  assert.throws(() => evaluate('2026-11-03T12:00:00Z', [], [], { monthlyDay: 31 }));
});
const job = (extra = {}) => ({ name: 'run-full / test', runner_id: 1, started_at: '2026-11-03T12:01:00Z', status: 'completed', conclusion: 'success', steps: [{ name: 'Run tests', conclusion: 'success' }], ...extra });
const run = { created_at: '2026-11-03T12:00:00Z', head_sha: 'abc', status: 'completed', conclusion: 'success' };
test('completed workload and required step establish full coverage', () => {
  assert.equal(summarizeAttempt(run, [job()], suite).complete, true);
});
test('green no-op, skipped work and absent step records cannot establish coverage', () => {
  for (const item of [job({ steps: [] }), job({ runner_id: 0 }), job({ conclusion: 'skipped' }), job({ steps: [{ name: 'Run tests', conclusion: 'skipped' }] })]) {
    assert.equal(summarizeAttempt(run, [item], suite).complete, false);
  }
});
test('missing matrix variant prevents full coverage', () => {
  assert.equal(summarizeAttempt(run, [job()], { ...suite, expectedJobs: [{ name: 'test' }, { name: 'windows' }] }).complete, false);
});
test('no-op gate alone does not count as a suite attempt', () => {
  assert.equal(summarizeAttempt(run, [job({ name: 'decide' })], suite).started, false);
});
test('queued caller counts as inflight even before reusable jobs appear', () => {
  assert.equal(summarizeAttempt({ ...run, status: 'queued' }, [], suite).inProgress, true);
});
test('collector avoids all activity API work when a monthly pass is already due', async () => {
  const github = { rest: { actions: { listWorkflowRuns: async () => ({ data: { workflow_runs: [] } }) } } };
  const result = await collectSnapshot({ github, context: { repo: { owner: 'owner', repo: 'repo' }, runId: 1 }, suite, now: '2026-11-03T12:00:00Z', defaultBranch: 'trunk' });
  assert.equal(result.changes.length, 0);
});
test('collector failure is surfaced, not changed into a successful skip', async () => {
  const github = { rest: { actions: { listWorkflowRuns: async () => { throw new Error('denied'); } } } };
  await assert.rejects(collectSnapshot({ github, context: { repo: { owner: 'owner', repo: 'repo' }, runId: 1 }, suite, now: '2026-11-03T12:00:00Z', defaultBranch: 'trunk' }), /denied/);
});

test('source resolution uses trusted config and returns one immutable commit', async () => {
  const sourceSha = 'a'.repeat(40);
  let called;
  const github = { rest: { repos: { getCommit: async (args) => { called = args; return { data: { sha: sourceSha } }; } } } };
  const result = await resolveSource({ github, context: { repo: { owner: 'o', repo: 'r' } }, sourceRef: 'neon-vector-horizon' });
  assert.deepEqual(result, { sourceRef: 'neon-vector-horizon', sourceSha });
  assert.equal(called.ref, 'neon-vector-horizon');
});
test('source resolution rejects invalid refs and nonimmutable API results', async () => {
  await assert.rejects(resolveSource({ github: {}, context: {}, sourceRef: '../untrusted' }), /trusted/);
  const github = { rest: { repos: { getCommit: async () => ({ data: { sha: 'main' } }) } } };
  await assert.rejects(resolveSource({ github, context: { repo: {} }, sourceRef: 'main' }), /immutable/);
});

test('default-branch success does not claim an open PR head was tested', () => {
  const result = evaluate('2026-11-29T12:00:00Z', [attempt('2026-11-15T12:00:00Z')], [{ ...change('2026-11-14T12:00:00Z'), source: 'pull:17' }]);
  assert.equal(result.due, true);
});
