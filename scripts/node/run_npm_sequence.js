'use strict';

const { spawnSync } = require('child_process');

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
        })
      : spawnSync('npm', ['run', scriptName], { stdio: 'inherit' });
  if (completed.error) {
    throw completed.error;
  }
  if (completed.status !== 0) {
    process.exit(completed.status === null ? 1 : completed.status);
  }
}

console.log(`[${label}] sequence complete (${args.length} script(s))`);
