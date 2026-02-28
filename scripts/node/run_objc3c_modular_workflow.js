'use strict';

const { spawnSync } = require('child_process');

const mode = (process.argv[2] || '').trim().toLowerCase();

const workflows = {
  lex: ['check:objc3c:boundaries', 'test:objc3c:parser-replay-proof'],
  parse: ['check:objc3c:boundaries', 'test:objc3c:parser-replay-proof'],
  sema: ['check:objc3c:boundaries', 'test:objc3c:diagnostics-replay-proof'],
  lower: ['check:objc3c:boundaries', 'test:objc3c:lowering-regression', 'test:objc3c:lowering-replay-proof'],
  ir: ['check:objc3c:boundaries', 'test:objc3c:typed-abi-replay-proof'],
};

if (!Object.prototype.hasOwnProperty.call(workflows, mode)) {
  console.error('objc3c-modular-workflow: FAIL (usage: node scripts/node/run_objc3c_modular_workflow.js <lex|parse|sema|lower|ir>)');
  process.exit(1);
}

const runNpmScript = (scriptName) => {
  console.log(`[objc3c-modular:${mode}] npm run ${scriptName}`);
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
};

for (const scriptName of workflows[mode]) {
  runNpmScript(scriptName);
}

console.log(`[objc3c-modular:${mode}] sequence complete (${workflows[mode].length} script(s))`);
