from __future__ import annotations

from pathlib import Path


def assert_runtime_closure_forbidden_claims_fail_closed(
    boundary_path: Path,
    boundary: dict,
    checks: dict[str, bool],
) -> None:
    assert all(checks.values()), f"{boundary_path}: {checks}"
    assert set(boundary["public_workflow_actions"])
    assert set(boundary["fail_closed_boundary_inventory"].values()) == {
        "fail-closed"
    }
