'use strict';

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const root = process.cwd();
const outputDir = path.join(root, 'reports', 'open_blocker_audit', 'ci_repo_root');
const stdoutPath = path.join(outputDir, 'open_blocker_audit_contract_check.stdout.json');
const stderrPath = path.join(outputDir, 'open_blocker_audit_contract_check.stderr.txt');

fs.mkdirSync(outputDir, { recursive: true });

const args = [
  'scripts/check_open_blocker_audit_contract.py',
  '--summary',
  'reports/open_blocker_audit/ci_repo_root/open_blocker_audit_summary.json',
  '--snapshot',
  'reports/open_blocker_audit/ci_repo_root/inputs/open_blockers.snapshot.json',
  '--extract-log',
  'reports/open_blocker_audit/ci_repo_root/extract_open_blockers.log',
  '--contract-id',
  'open-blocker-audit-runner',
  '--contract-version',
  'v0.1',
];

const completed = spawnSync('python', args, { encoding: 'utf8' });
if (completed.error) {
  throw completed.error;
}

fs.writeFileSync(stdoutPath, completed.stdout || '', 'utf8');
fs.writeFileSync(stderrPath, completed.stderr || '', 'utf8');

if (completed.status !== 0) {
  process.stderr.write(completed.stderr || '');
  process.exit(completed.status === null ? 1 : completed.status);
}

console.log('open-blocker-audit-contract: OK (fixture contract run captured to reports/open_blocker_audit/ci_repo_root)');
