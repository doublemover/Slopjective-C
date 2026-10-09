import { readFile } from 'node:fs/promises';

const DAY = 86_400_000;

export function validateConfig(config) {
  if (config.version !== 1 || !Array.isArray(config.suites) || config.suites.length === 0) {
    throw new Error('Unsupported or empty adaptive CI configuration');
  }
  localDate(Date.now(), config.timezone);
  const ids = new Set();
  const crons = new Set();
  for (const suite of config.suites) {
    if (!/^[a-z][a-z0-9-]{0,30}$/.test(suite.id) || ids.has(suite.id)) throw new Error('Invalid or duplicate suite ID');
    if (!/^[a-zA-Z0-9_-]+\.ya?ml$/.test(suite.workflow)) throw new Error('Invalid reusable workflow path');
    if (typeof suite.cron !== 'string' || suite.cron.trim().split(/\s+/).length !== 5 || crons.has(suite.cron)) throw new Error('Invalid or duplicate evaluation slot');
    if (!Number.isInteger(suite.monthlyDay) || suite.monthlyDay < 1 || suite.monthlyDay > 28) throw new Error('Invalid monthly anchor');
    if (!Array.isArray(suite.expectedJobs) || suite.expectedJobs.length === 0) throw new Error('Expected workload jobs are required');
    if (new Set(suite.expectedJobs.map((job) => job.name)).size !== suite.expectedJobs.length) throw new Error('Duplicate expected workload job');
    ids.add(suite.id);
    crons.add(suite.cron);
  }
  return config;
}

function timestamp(value, name) {
  const result = Date.parse(value);
  if (!Number.isFinite(result)) throw new Error(`Invalid ${name}: ${value}`);
  return result;
}

export function localDate(value, timezone = 'America/Detroit') {
  const parts = new Intl.DateTimeFormat('en-CA', {
    timeZone: timezone,
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
  }).formatToParts(new Date(value));
  const field = (type) => parts.find((part) => part.type === type).value;
  return `${field('year')}-${field('month')}-${field('day')}`;
}

export function calendarDaysSince(now, before, timezone) {
  return (Date.parse(localDate(now, timezone)) - Date.parse(localDate(before, timezone))) / DAY;
}

function globExpression(pattern) {
  let result = '^';
  for (let index = 0; index < pattern.length; index += 1) {
    const character = pattern[index];
    if (character === '*' && pattern[index + 1] === '*') {
      index += 1;
      if (pattern[index + 1] === '/') {
        index += 1;
        result += '(?:.*/)?';
      } else result += '.*';
    } else if (character === '*') result += '[^/]*';
    else if (character === '?') result += '[^/]';
    else result += character.replace(/[\\^$+?.()|{}\[\]]/g, '\\$&');
  }
  return new RegExp(`${result}$`);
}

export function relevantPath(path, suite) {
  if (typeof path !== 'string' || path.startsWith('/') || path.split('/').includes('..')) {
    throw new Error('Invalid changed repository path');
  }
  if ((suite.protectedPaths ?? []).some((pattern) => globExpression(pattern).test(path))) {
    return true;
  }
  return (suite.paths ?? ['**']).some((pattern) => globExpression(pattern).test(path)) &&
    !(suite.ignorePaths ?? []).some((pattern) => globExpression(pattern).test(path));
}

export function decide({ suite, now, history = [], changes = [], timezone = 'America/Detroit' }) {
  const nowMs = timestamp(now, 'current time');
  const today = localDate(nowMs, timezone);
  const month = today.slice(0, 7);
  if (!Number.isInteger(suite.monthlyDay) || suite.monthlyDay < 1 || suite.monthlyDay > 28) {
    throw new Error('monthlyDay must be an integer from 1 through 28');
  }
  if (suite.notBefore && today < suite.notBefore) {
    return { due: false, reason: `Not enabled before ${suite.notBefore}`, cadence: 'paused' };
  }
  const attempts = history.filter((run) => run.started || run.inProgress);
  for (const run of attempts) {
    if (timestamp(run.at, 'attempt time') > nowMs) throw new Error('Future attempt in history');
  }
  if (attempts.some((run) => run.inProgress)) {
    return { due: false, reason: 'This suite already has an in-progress attempt', cadence: 'busy' };
  }
  const ordered = attempts.toSorted((left, right) => timestamp(right.at) - timestamp(left.at));
  const lastAttempt = ordered[0];
  const lastSuccess = ordered.find((run) => run.complete && run.outcome === 'success');
  const lastSuccessMs = lastSuccess ? timestamp(lastSuccess.at) : -Infinity;
  const relevant = changes.filter((change) => {
    const at = timestamp(change.at, 'change time');
    if (at > nowMs) throw new Error('Future activity in snapshot');
    return (at > lastSuccessMs || change.source?.startsWith('pull:')) && calendarDaysSince(nowMs, at, timezone) <= 28 &&
      (change.unknownPaths === true || change.paths.some((path) => relevantPath(path, suite)));
  });
  const active = relevant.length > 0;
  const age = lastAttempt ? calendarDaysSince(nowMs, timestamp(lastAttempt.at), timezone) : null;
  const attemptedThisMonth = attempts.some((run) => localDate(timestamp(run.at), timezone).startsWith(month));
  const monthlyDue = !attemptedThisMonth && Number(today.slice(8)) >= suite.monthlyDay;
  const fortnightlyDue = active && lastAttempt && age >= 14;
  if (lastAttempt && age < 14) {
    return { due: false, reason: `Last attempt was ${age} local calendar days ago`, cadence: active ? 'fortnightly' : 'monthly' };
  }
  return {
    due: Boolean(monthlyDue || fortnightlyDue),
    cadence: active && lastAttempt ? 'fortnightly' : 'monthly',
    reason: monthlyDue ? 'Monthly pass is due' : fortnightlyDue ? 'Relevant uncovered activity and 14 days elapsed' : 'No eligible monthly or activity-based pass',
    relevantChanges: relevant.length,
    lastAttempt: lastAttempt?.at ?? null,
    lastCompleteSuccess: lastSuccess?.at ?? null,
  };
}

export function summarizeAttempt(run, jobs, suite) {
  if (!Array.isArray(suite.expectedJobs) || suite.expectedJobs.length === 0) {
    throw new Error('At least one expected workload job is required');
  }
  const prefix = `run-${suite.id} / `;
  const children = jobs.filter((job) => job.name.startsWith(prefix));
  const started = children.some((job) => job.runner_id > 0 && job.started_at);
  const inProgress = ['queued', 'in_progress', 'waiting', 'pending'].includes(run.status) ||
    children.some((job) => ['queued', 'in_progress', 'waiting', 'pending'].includes(job.status));
  const complete = suite.expectedJobs.every((expected) => {
    const matches = children.filter((job) => job.name.slice(prefix.length) === expected.name);
    return matches.length === 1 && matches[0].conclusion === 'success' && matches[0].runner_id > 0 &&
      (matches[0].steps ?? []).length > 0 &&
      (expected.requiredSteps ?? []).every((pattern) => matches[0].steps.some((step) =>
        new RegExp(pattern).test(step.name) && step.conclusion === 'success'));
  });
  return { at: run.created_at, sha: run.head_sha, started, inProgress, complete, outcome: complete ? 'success' : run.conclusion };
}

async function listBounded(call, params, key, maximumPages = 5) {
  const result = [];
  for (let page = 1; page <= maximumPages; page += 1) {
    const response = await call({ ...params, per_page: 100, page });
    const values = key ? response.data[key] : response.data;
    if (!Array.isArray(values)) throw new Error('Unexpected GitHub list response');
    result.push(...values);
    if (values.length < 100) return result;
  }
  throw new Error('GitHub history exceeds the bounded collector; refusing an incomplete cadence decision');
}

export async function collectSnapshot({ github, context, suite, now, defaultBranch, timezone = 'America/Detroit' }) {
  const common = { owner: context.repo.owner, repo: context.repo.repo };
  const runs = await listBounded(github.rest.actions.listWorkflowRuns, {
    ...common, workflow_id: 'adaptive-maintenance.yml',
  }, 'workflow_runs');
  const relevantRuns = runs.filter((run) => run.id !== context.runId &&
    run.display_title === `Adaptive maintenance / ${suite.id}` &&
    timestamp(run.created_at) >= timestamp(now) - 370 * DAY);
  const history = [];
  for (const run of relevantRuns) {
    const jobs = await listBounded(github.rest.actions.listJobsForWorkflowRun, {
      ...common, run_id: run.id, filter: 'latest',
    }, 'jobs');
    history.push(summarizeAttempt(run, jobs, suite));
  }
  const preliminary = decide({ suite, now, history, changes: [], timezone });
  if (preliminary.due || preliminary.cadence === 'busy' || preliminary.cadence === 'paused' ||
      preliminary.reason.startsWith('Last attempt') || history.every((attempt) => !attempt.started)) {
    return { suite, now, history, changes: [] };
  }
  const lastSuccess = history.filter((attempt) => attempt.complete && attempt.outcome === 'success')
    .toSorted((left, right) => timestamp(right.at) - timestamp(left.at))[0];
  const since = new Date(Math.max(timestamp(now) - 29 * DAY, lastSuccess ? timestamp(lastSuccess.at) : 0)).toISOString();
  const commitResponse = await github.rest.repos.listCommits({
    ...common, sha: defaultBranch, since, per_page: 100,
  });
  const commits = commitResponse.data;
  const changes = [];
  for (const commit of commits) {
    const detail = await github.rest.repos.getCommit({ ...common, ref: commit.sha, per_page: 100 });
    const files = detail.data.files ?? [];
    changes.push({
      at: detail.data.commit.committer?.date ?? detail.data.commit.author?.date,
      paths: files.flatMap((file) => [file.filename, file.previous_filename].filter(Boolean)),
      unknownPaths: files.length >= 100,
      source: `commit:${commit.sha}`,
    });
    if (decide({ suite, now, history, changes, timezone }).due) return { suite, now, history, changes };
  }
  if (commits.length >= 100) {
    changes.push({ at: now, paths: [], unknownPaths: true, source: 'commit-list-truncated' });
    return { suite, now, history, changes };
  }
  const pulls = await listBounded(github.rest.pulls.list, { ...common, state: 'open', base: defaultBranch });
  for (const pull of pulls.filter((item) => !item.draft)) {
    const head = await github.rest.repos.getCommit({ ...common, ref: pull.head.sha, per_page: 1 });
    const at = head.data.commit.committer?.date ?? head.data.commit.author?.date;
    if (timestamp(at) < timestamp(now) - 29 * DAY) continue;
    const files = await listBounded(github.rest.pulls.listFiles, { ...common, pull_number: pull.number });
    changes.push({ at, paths: files.flatMap((file) => [file.filename, file.previous_filename].filter(Boolean)), source: `pull:${pull.number}` });
    if (decide({ suite, now, history, changes, timezone }).due) return { suite, now, history, changes };
  }
  return { suite, now, history, changes };
}

export async function resolveSource({ github, context, sourceRef }) {
  if (typeof sourceRef !== 'string' || !/^[A-Za-z0-9][A-Za-z0-9._/-]*$/.test(sourceRef) || sourceRef.includes('..')) {
    throw new Error('A trusted source branch is required');
  }
  const { data } = await github.rest.repos.getCommit({ ...context.repo, ref: sourceRef });
  if (!/^[a-f0-9]{40}$/.test(data.sha)) throw new Error('Source did not resolve to an immutable commit');
  return { sourceRef, sourceSha: data.sha };
}

export async function run({ github, context, core, now = new Date().toISOString() }) {
  const config = validateConfig(JSON.parse(await readFile(`${process.env.GITHUB_WORKSPACE}/.github/ci/adaptive-ci.json`, 'utf8')));
  const selected = context.eventName === 'schedule'
    ? config.suites.filter((suite) => suite.cron === context.payload.schedule)
    : config.suites.filter((suite) => suite.id === context.payload.inputs?.suite);
  for (const suite of config.suites) core.setOutput(`run_${suite.id.replaceAll('-', '_')}`, 'false');
  if (selected.length !== 1) throw new Error('A maintenance invocation must select exactly one configured suite');
  const { data: repository } = await github.rest.repos.get(context.repo);
  if (context.ref !== `refs/heads/${repository.default_branch}`) {
    throw new Error('Adaptive maintenance must run from the actual default branch');
  }
  const source = await resolveSource({ github, context, sourceRef: config.sourceRef });
  core.setOutput('source_sha', source.sourceSha);
  core.setOutput('source_ref', source.sourceRef);
  core.info(`Source checkout: ${source.sourceRef} at ${source.sourceSha}; dispatcher: ${context.sha}`);
  const suite = selected[0];
  const snapshot = await collectSnapshot({ github, context, suite, now, defaultBranch: repository.default_branch });
  const decision = decide({ ...snapshot, timezone: config.timezone });
  core.setOutput(`run_${suite.id.replaceAll('-', '_')}`, String(decision.due));
  await core.summary.addHeading('Adaptive maintenance decision').addCodeBlock(JSON.stringify({ suite: suite.id, dispatcherSha: context.sha, ...source, ...decision }, null, 2), 'json').write();
  core.info(`${suite.id}: ${decision.reason}`);
}
