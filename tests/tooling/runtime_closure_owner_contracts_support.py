from __future__ import annotations

from pathlib import Path
from typing import Any

from scripts.runtime_closure_owner_contracts import (
    load_runtime_closure_owner_contract,
    read_json_object,
    runtime_closure_owner_checks,
)

ROOT = Path(__file__).resolve().parents[2]
BOUNDARY_INVENTORIES = (
    ROOT / "tests/tooling/fixtures/block_arc_closure/boundary_inventory.json",
    ROOT / "tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json",
    ROOT / "tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json",
)

RuntimeClosureOwnerFixture = tuple[Path, dict[str, Any], dict[str, Any], dict[str, bool]]


def runtime_closure_owner_fixtures() -> list[RuntimeClosureOwnerFixture]:
    fixtures: list[RuntimeClosureOwnerFixture] = []
    for boundary_path in BOUNDARY_INVENTORIES:
        boundary = read_json_object(boundary_path)
        owner_contract = load_runtime_closure_owner_contract(ROOT, boundary)
        checks = runtime_closure_owner_checks(ROOT, boundary, owner_contract)
        fixtures.append((boundary_path, boundary, owner_contract, checks))
    return fixtures
