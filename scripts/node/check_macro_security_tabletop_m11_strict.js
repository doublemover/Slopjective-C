'use strict';

const fs = require('fs');
const path = require('path');

const root = process.cwd();

const required = [
  'spec/planning/v013_macro_security_tabletop_package.md',
  'spec/governance/macro_security_incident_playbook_v1.md',
  'reports/security/v013_macro_tabletop.md',
];
const scenarioIds = ['SMT-V013-01', 'SMT-V013-02', 'SMT-V013-03', 'SMT-V013-04', 'SMT-V013-05'];
const tierIds = ['RT-V013-T1', 'RT-V013-T2', 'RT-V013-T3', 'RT-V013-T4'];
const remediationIds = ['FRL-V013-01', 'FRL-V013-02', 'FRL-V013-03', 'FRL-V013-04', 'FRL-V013-05'];
const tupleTokens = ['#790', 'W1', 'BATCH-20260223-11S', 'AC-V013-GOV-04'];

function fail(message) {
  console.error(`macro-security-tabletop-contract: FAIL (${message})`);
  process.exit(1);
}

const read = (relativePath) => fs.readFileSync(path.join(root, relativePath), 'utf8').replace(/\r\n/g, '\n');

for (const relativePath of required) {
  const absolutePath = path.join(root, relativePath);
  if (!fs.existsSync(absolutePath)) {
    fail(`missing_file=${relativePath}`);
  }
}

const docs = Object.fromEntries(required.map((relativePath) => [relativePath, read(relativePath)]));

for (const relativePath of required) {
  for (const token of tupleTokens) {
    if (!docs[relativePath].includes(token)) {
      fail(`missing_tuple_token=${token} file=${relativePath}`);
    }
  }
}

for (const id of scenarioIds) {
  for (const relativePath of required) {
    if (!docs[relativePath].includes(id)) {
      fail(`missing_scenario_id=${id} file=${relativePath}`);
    }
  }
}

for (const id of tierIds) {
  for (const relativePath of required) {
    if (!docs[relativePath].includes(id)) {
      fail(`missing_response_tier_id=${id} file=${relativePath}`);
    }
  }
}

for (const id of remediationIds) {
  for (const relativePath of required) {
    if (!docs[relativePath].includes(id)) {
      fail(`missing_remediation_id=${id} file=${relativePath}`);
    }
  }
}

if (!docs['spec/planning/v013_macro_security_tabletop_package.md'].includes('## 7. Acceptance Checklist Mapping (`AC-V013-GOV-04`)')) {
  fail('package_acceptance_mapping_missing');
}
if (!docs['spec/governance/macro_security_incident_playbook_v1.md'].includes('### 12.4 Reseed metadata binding (`#790`, `BATCH-20260223-11S`)')) {
  fail('playbook_reseed_binding_missing');
}
if (!docs['reports/security/v013_macro_tabletop.md'].includes('Scenario matrix summary: `5/5` scenarios passed severity/tier determinism checks.')) {
  fail('report_scenario_summary_missing_or_drifted');
}

console.log('macro-security-tabletop-contract: OK (files=3, tuple_tokens=4, scenarios=5, tiers=4, remediations=5, deterministic_summary=5/5)');
