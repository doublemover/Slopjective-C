# Backlog Publication Support Surfaces

- canonical policy: `tmp/planning/backlog_publication/backlog_publication_policy.json`
- human policy summary: `tmp/planning/backlog_publication/backlog_publication_policy.md`
- replayable overlap inventory generator: `tmp/planning/backlog_publication/build_backlog_overlap_supersession_inventory.py`

This directory is intentionally outside the generated per-program planning packets so regeneration does not delete the canonical M317 backlog-publication support surfaces.

Governance preflight for new work and backlog publication now runs through the live M318 surfaces before any publish step:

- `python scripts/build_governance_budget_inventory_summary.py`
- `python scripts/build_governance_policy_summary.py`
- `python scripts/build_governance_maintainer_review_summary.py`
- `python scripts/check_governance_sustainability_budget_enforcement.py`
