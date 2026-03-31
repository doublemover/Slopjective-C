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
