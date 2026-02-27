'use strict';

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const mode = (process.argv[2] || '').trim();
if (mode !== 'm09' && mode !== 'm10') {
  console.error('quality-gate-baseline: FAIL (usage: node scripts/node/check_quality_gate_automation_strict.js <m09|m10>)');
  process.exit(1);
}

const root = process.cwd();
const tmpDir = path.join(root, 'tmp', 'quality_gate_automation', mode);
const tmpMd = path.join(tmpDir, 'v011_quality_gate_decision.md');
const tmpStatus = path.join(tmpDir, 'v011_quality_gate_decision.status.json');
const expectedMd = path.join(root, 'reports', 'releases', 'v011_quality_gate_decision.md');
const expectedStatus = path.join(root, 'reports', 'releases', 'v011_quality_gate_decision.status.json');

const fail = (message) => {
  console.error(`quality-gate-baseline: FAIL (${message})`);
  process.exit(1);
};

const read = (targetPath) => fs.readFileSync(targetPath, 'utf8').replace(/\r\n/g, '\n');

fs.mkdirSync(tmpDir, { recursive: true });

const run = spawnSync(
  'python',
  [
    'scripts/generate_quality_gate_decision.py',
    '--output-md',
    tmpMd,
    '--output-status',
    tmpStatus,
    '--generated-at',
    '2026-02-23T22:00:00Z',
  ],
  { stdio: 'inherit' }
);

if (run.error) {
  throw run.error;
}
if (run.status !== 0) {
  process.exit(run.status === null ? 1 : run.status);
}

const mdMatch = read(tmpMd) === read(expectedMd);
const statusMatch = read(tmpStatus) === read(expectedStatus);
const status = JSON.parse(read(tmpStatus));
const decisionMatch = status.overall_decision === 'hold' && status.qg_04_result === 'fail' && status.recommendation_signal === 'no-go';

if (mode === 'm09') {
  if (!mdMatch || !statusMatch || !decisionMatch) {
    fail(`md_match=${mdMatch}, status_match=${statusMatch}, decision_match=${decisionMatch}`);
  }
  console.log('quality-gate-baseline: OK (md_match=true, status_match=true, overall_decision=hold, qg_04_result=fail, recommendation_signal=no-go)');
  process.exit(0);
}

const contractMatch = status.contract_id === 'V013-CONF-02-QUALITY-GATE-v2';
const evidenceMap = Object.fromEntries((status.evidence_items || []).map((row) => [row.evidence_id, row.status]));
const evidenceMatch = evidenceMap['EV-06'] === 'pass' && evidenceMap['EV-07'] === 'pass' && evidenceMap['EV-08'] === 'fail' && Object.keys(evidenceMap).length === 3;
const downstreamMatch = Array.isArray(status.downstream_consumers) && status.downstream_consumers.join(',') === 'V013-CONF-03,V013-REL-01';
const blockerMatch = Array.isArray(status.unresolved_blockers) && status.unresolved_blockers.map((row) => row.blocker_id).join(',') === 'BLK-189-01,BLK-189-02,BLK-189-03';
const handoffs = status.downstream_handoffs || [];
const handoffConf = handoffs.find((row) => row.consumer_seed === 'V013-CONF-03');
const handoffRel = handoffs.find((row) => row.consumer_seed === 'V013-REL-01');
const handoffMatch = !!handoffConf && !!handoffRel && Array.isArray(handoffConf.required_inputs) && handoffConf.required_inputs.join(',') === 'EV-07,EV-08' && Array.isArray(handoffRel.required_inputs) && handoffRel.required_inputs.join(',') === 'EV-07,EV-08,BLK-189 posture';

if (!mdMatch || !statusMatch || !contractMatch || !decisionMatch || !evidenceMatch || !downstreamMatch || !blockerMatch || !handoffMatch) {
  fail(`md_match=${mdMatch}, status_match=${statusMatch}, contract_match=${contractMatch}, decision_match=${decisionMatch}, evidence_match=${evidenceMatch}, downstream_match=${downstreamMatch}, blocker_match=${blockerMatch}, handoff_match=${handoffMatch}`);
}

console.log('quality-gate-baseline: OK (md_match=true, status_match=true, contract_id=V013-CONF-02-QUALITY-GATE-v2, overall_decision=hold, qg_04_result=fail, recommendation_signal=no-go, downstream_consumers=V013-CONF-03|V013-REL-01, blocker_ids=BLK-189-01|BLK-189-02|BLK-189-03)');
