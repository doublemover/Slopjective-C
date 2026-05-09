from __future__ import annotations

from pathlib import Path

from scripts.runtime_closure_owner_contracts import (
    REQUIRED_OWNER_POLICY,
    REQUIRED_OWNER_ROLES,
    load_runtime_closure_owner_contract,
    read_json_object,
    runtime_closure_owner_checks,
    runtime_closure_owner_summary,
)

ROOT = Path(__file__).resolve().parents[2]
BOUNDARY_INVENTORIES = (
    ROOT / "tests/tooling/fixtures/block_arc_closure/boundary_inventory.json",
    ROOT / "tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json",
    ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
)


def test_runtime_closure_owner_contracts_are_source_owned_and_fail_closed() -> None:
    for boundary_path in BOUNDARY_INVENTORIES:
        boundary = read_json_object(boundary_path)
        owner_contract = load_runtime_closure_owner_contract(ROOT, boundary)
        checks = runtime_closure_owner_checks(ROOT, boundary, owner_contract)

        assert all(checks.values()), f"{boundary_path}: {checks}"
        assert owner_contract["owner_policy"] == REQUIRED_OWNER_POLICY
        assert set(owner_contract["role_contracts"]) == set(REQUIRED_OWNER_ROLES)


def test_runtime_closure_owner_summary_keeps_publication_policy_visible() -> None:
    for boundary_path in BOUNDARY_INVENTORIES:
        owner_contract = load_runtime_closure_owner_contract(
            ROOT, read_json_object(boundary_path)
        )
        summary = runtime_closure_owner_summary(owner_contract)

        assert summary["owner_role_count"] == len(REQUIRED_OWNER_ROLES)
        assert summary["report_only_allowed"] is False
        assert summary["fallback_allowed"] is False
        assert summary["missing_artifact_behavior"] == "fail-closed"
