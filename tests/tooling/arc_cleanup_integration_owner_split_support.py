from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES

ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "arc_cleanup_integration" / "owner_contract.json"
)
CAPABILITY_MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs" / "support" / "evidence_map.json"


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def load_contract() -> dict[str, Any]:
    return load_json(CONTRACT_PATH)


def iter_surface_fixture_paths(contract: dict[str, Any]) -> set[str]:
    paths: set[str] = set()
    for surface in contract["integration_surfaces"].values():
        paths.update(surface["fixtures"])
    return paths


def iter_surface_case_ids(contract: dict[str, Any]) -> set[str]:
    case_ids: set[str] = set()
    for surface in contract["integration_surfaces"].values():
        case_ids.update(surface["case_ids"])
    return case_ids


def capability_by_id(capability_id: str) -> dict[str, Any]:
    matrix = load_json(CAPABILITY_MATRIX_PATH)
    for capability in matrix["capabilities"]:
        if capability["id"] == capability_id:
            return capability
    raise AssertionError(f"missing capability row: {capability_id}")


def evidence_rows_for(capability_id: str) -> list[dict[str, Any]]:
    evidence_map = load_json(EVIDENCE_MAP_PATH)
    return [
        row
        for row in evidence_map["rows"]
        if row["capability_id"] == capability_id
    ]


def assert_issue_8036_contract_integrity() -> None:
    contract = load_contract()
    assert contract["issue"] == 8036
    assert contract["draft_id"] == "OC3-T10-003"
    assert contract["support_claim"] == "objc3c.behavior.arc-cleanup.integration"
    assert (
        contract["public_command"]
        == "npm run objc3c -- test-runtime-acceptance-arc-cleanup-integration"
    )
    assert set(contract["integration_surfaces"]) == {
        "blocks",
        "properties",
        "errors",
        "async",
        "interop",
    }


def assert_issue_8036_fixtures_cover_required_source_shapes() -> None:
    contract = load_contract()
    for relative_path in iter_surface_fixture_paths(contract):
        path = ROOT / relative_path
        assert path.is_file(), relative_path

    for relative_path, required_tokens in contract["required_source_tokens"].items():
        source = (ROOT / relative_path).read_text(encoding="utf-8")
        for token in required_tokens:
            assert token in source, f"{relative_path} missing {token!r}"


def assert_arc_cleanup_runtime_suite_maps_every_surface() -> None:
    contract = load_contract()
    suite_id = contract["runtime_acceptance_suite"]
    suite_cases = set(RUNTIME_ACCEPTANCE_SUITE_CASES[suite_id])

    assert suite_cases == iter_surface_case_ids(contract)
    assert "block-storage-arc-automation-semantics" in suite_cases
    assert "arc-property-helper" in suite_cases
    assert "error-runtime-abi-cleanup" in suite_cases
    assert "unified-concurrency-runtime-abi" in suite_cases
    assert "runtime-package-loader-bridge-abi" in suite_cases


def assert_arc_cleanup_support_claim_is_durable() -> None:
    contract = load_contract()
    capability = capability_by_id("language.arc-cleanup.integration")
    evidence_rows = evidence_rows_for("language.arc-cleanup.integration")
    evidence_paths = {row["path"] for row in evidence_rows}

    assert capability["state"] == "implemented"
    assert capability["support_claims"] == [contract["support_claim"]]
    assert any(
        row.get("command") == contract["public_command"]
        for row in evidence_rows
    )
    assert CONTRACT_PATH.relative_to(ROOT).as_posix() in evidence_paths
    assert iter_surface_fixture_paths(contract).issubset(evidence_paths)
