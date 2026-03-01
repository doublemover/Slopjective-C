'use strict';

const { spawnSync } = require('child_process');

const DEFAULT_TIMEOUT_MS = Number.parseInt(process.env.RUN_NPM_SEQUENCE_TIMEOUT_MS || '1800000', 10);
const timeoutMs = Number.isFinite(DEFAULT_TIMEOUT_MS) && DEFAULT_TIMEOUT_MS > 0 ? DEFAULT_TIMEOUT_MS : 1800000;

const args = process.argv.slice(2);
const label = args.shift();

if (!label || args.length === 0) {
  console.error('run-npm-sequence: FAIL (usage: node scripts/node/run_npm_sequence.js <label> <script> [<script>...])');
  process.exit(1);
}

for (const scriptName of args) {
  console.log(`[${label}] npm run ${scriptName}`);
  const completed =
    process.platform === 'win32'
      ? spawnSync(process.env.ComSpec || 'cmd.exe', ['/d', '/s', '/c', `npm run ${scriptName}`], {
          stdio: 'inherit',
          timeout: timeoutMs,
        })
      : spawnSync('npm', ['run', scriptName], { stdio: 'inherit', timeout: timeoutMs });
  if (completed.error) {
    if (completed.error.code === 'ETIMEDOUT') {
      console.error(
        `[${label}] FAIL npm run ${scriptName} timed out after ${timeoutMs}ms; ` +
          'override via RUN_NPM_SEQUENCE_TIMEOUT_MS.'
      );
      process.exit(124);
    }
    throw completed.error;
  }
  if (completed.status !== 0) {
    process.exit(completed.status === null ? 1 : completed.status);
  }
}

console.log(`[${label}] sequence complete (${args.length} script(s))`);
