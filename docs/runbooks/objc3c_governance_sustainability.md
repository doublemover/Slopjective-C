# objc3c Governance And Sustainability

This runbook freezes the governance baseline for `M318`.

Canonical checked-in governance inventory contract:

- `tests/tooling/fixtures/governance_sustainability/budget_inventory.json`

Replayable inventory generator:

- `python scripts/build_governance_budget_inventory_summary.py`

Governance focus:

- measure the live repo shape before setting or tightening budgets
- keep budget and anti-noise claims tied to existing enforcement anchors
- treat exceptions and drift as explicit recorded objects instead of tribal knowledge

Sustainable progress policy:

- every governed surface must map to one canonical checked-in owner and one replayable measurement path
- budget increases are allowed only with a measured delta, a recorded reason, and a follow-on ratchet path
- new automation should extend existing runner, hygiene, and source-surface checks before adding new wrappers or command aliases
- governance claims stay narrower than the evidence; unresolved drifts must appear in reports, waivers, or release-blocking status instead of being silently tolerated

Exception model:

- exceptions are checked-in waiver objects, not ad hoc comments
- each waiver must name:
  - governed surface
  - owner
  - reason
  - evidence paths
  - opened timestamp
  - expiry timestamp
  - current status
- expired waivers are release-blocking until removed or superseded
- budget increases without a corresponding checked-in policy update and measured evidence are invalid

Canonical checked-in policy contracts:

- `tests/tooling/fixtures/governance_sustainability/sustainable_progress_policy.json`
- `tests/tooling/fixtures/governance_sustainability/waiver_registry.json`

Replayable policy summary:

- `python scripts/build_governance_policy_summary.py`

Machine-owned governance schema surface:

- `tests/tooling/fixtures/governance_sustainability/schema_surface.json`
- `schemas/objc3c-governance-budget-summary-v1.schema.json`
- `schemas/objc3c-governance-anti-regression-summary-v1.schema.json`
- `python scripts/check_governance_sustainability_schema_surface.py`

Replayable governance enforcement:

- `python scripts/check_governance_sustainability_budget_enforcement.py`
- `python scripts/ci/run_task_hygiene_gate.py`
- canonical enforcement summary: `tmp/reports/m318/M318-C002/governance_budget_enforcement_summary.json`

Long-horizon anti-regression reporting:

- `python scripts/build_governance_anti_regression_summary.py`
- canonical history artifact: `tmp/artifacts/governance-sustainability/anti-regression-history.json`
- canonical anti-regression summary: `tmp/reports/m318/M318-D002/governance_anti_regression_summary.json`

Current governance entry surfaces:

- `scripts/ci/check_task_hygiene.py`
- `scripts/ci/run_task_hygiene_gate.py`
- `scripts/check_repo_superclean_surface.py`
- `scripts/check_documentation_surface.py`
- `scripts/check_objc3c_dependency_boundaries.py`
- `docs/runbooks/objc3c_maintainer_workflows.md`
- `docs/runbooks/objc3c_public_command_surface.md`
- `scripts/objc3c_public_workflow_runner.py`

Current budget surfaces measured by `M318-A001`:

- package script count and category mix
- public workflow action count
- checked-in runbook count
- checked-in schema count
- live `check_*.py` script count
- must-remain-absent repo roots and workflow patterns
- known live governance drifts that later issues must ratchet:
  - milestone-scoped checkers
  - live Python bytecode
  - live `__pycache__` directories

Explicit non-goals:

- setting final waiver policy in `A001`
- defining review or expiry mechanics in `A001`
- claiming long-horizon regression reporting in `A001`
- widening public workflow surface before the policy and schema issues land

Generated evidence:

- `tmp/reports/m318/M318-A001/governance_budget_inventory_summary.json`
- `tmp/reports/m318/M318-B001/governance_policy_summary.json`