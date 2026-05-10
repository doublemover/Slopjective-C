"""Issue-template payload construction for seed batches."""

from __future__ import annotations

from .config import DEFAULT_UNASSIGNED_OWNER, FAMILY_LABEL, WORKLANE_LABEL
from .models import BatchRow, ParseError, PriorityRow, SeedOwnerAssignment, SeedRow
from .planning_topology import split_seed_title_action


def build_issue_template(
    *,
    batch: BatchRow,
    seed: SeedRow,
    priority: PriorityRow,
    wave_id: str,
    owner_assignments: dict[str, SeedOwnerAssignment] | None,
) -> dict[str, object]:
    family_label = FAMILY_LABEL.get(seed.family, "unknown")
    worklane_label = WORKLANE_LABEL.get(seed.worklane, "unknown")
    owner_primary = DEFAULT_UNASSIGNED_OWNER
    owner_backup = DEFAULT_UNASSIGNED_OWNER
    if owner_assignments is not None:
        assignment = owner_assignments.get(seed.seed_id)
        if assignment is None:
            raise ParseError(
                f"owner map missing assignment for seed id: {seed.seed_id}"
            )
        owner_primary = assignment.owner_primary
        owner_backup = assignment.owner_backup

    return {
        "seed_id": seed.seed_id,
        "title": seed.proposed_issue_title,
        "body_fields": {
            "seed_id": seed.seed_id,
            "family_tag": seed.family,
            "worklane": seed.worklane,
            "source_refs": [
                "SRC-V013-12 tmp/reports/v013_future_work_seed_matrix.md"
            ],
            "depends_on": list(seed.depends_on),
            "shard_class": seed.shard_class,
            "objective": [split_seed_title_action(seed.proposed_issue_title)],
            "artifact_targets": list(seed.artifact_targets),
            "acceptance_gate_id": seed.acceptance_gate_id,
            "validation_commands": [
                "python scripts/spec_lint.py",
                "python scripts/check_issue_checkbox_drift.py",
            ],
            "batch_id": batch.batch_id,
            "owner_primary": owner_primary,
            "owner_backup": owner_backup,
        },
        "labels": [
            "phase:v013",
            f"family:{family_label}",
            f"lane:{worklane_label}",
            f"seed:{seed.seed_id}",
            f"shard:{seed.shard_class}",
            f"priority:{priority.tier.lower()}",
        ],
        "priority": {
            "priority_score": priority.priority_score,
            "duv": priority.duv,
            "dc": priority.dc,
            "tier": priority.tier,
        },
        "wave_id": wave_id,
    }


__all__ = ["build_issue_template"]
