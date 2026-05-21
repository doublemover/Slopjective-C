from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from scripts.objc3c_runtime_acceptance.domains.advanced_runtime_capability_split import (
    build_advanced_runtime_capability_split_contract,
)
from scripts.objc3c_runtime_acceptance.domains.object_model_capability_split import (
    build_object_model_capability_split_contract,
)

ROOT = Path(__file__).resolve().parents[2]
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs" / "support" / "evidence_map.json"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _matrix_rows() -> dict[str, dict[str, Any]]:
    matrix = _read_json(MATRIX_PATH)
    return {str(row["id"]): row for row in matrix["capabilities"]}


def _evidence_rows() -> set[tuple[str, str | None, str]]:
    evidence_map = _read_json(EVIDENCE_MAP_PATH)
    return {
        (
            str(row["capability_id"]),
            row.get("support_claim"),
            str(row["path"]),
        )
        for row in evidence_map["rows"]
    }


def _assert_split_contract_matches_source_truth(contract: dict[str, Any]) -> None:
    rows = _matrix_rows()
    evidence_rows = _evidence_rows()
    umbrella = rows[contract["reserved_umbrella"]]

    assert umbrella["state"] == "reserved"
    assert "support_claims" not in umbrella
    assert {
        (contract["reserved_umbrella"], None, contract["source"]),
        (
            contract["reserved_umbrella"],
            None,
            "tests/tooling/test_runtime_capability_public_split.py",
        ),
    } <= evidence_rows

    implemented_rows = contract["implemented_rows"]
    assert implemented_rows
    for expected in implemented_rows:
        row = rows[expected["capability_id"]]

        assert row["state"] == "implemented"
        assert row["support_claims"] == [expected["support_claim"]]
        assert expected["behavior_fixture"] in {
            evidence["path"] for evidence in row["evidence"]
        }
        assert (
            expected["capability_id"],
            expected["support_claim"],
            expected["behavior_fixture"],
        ) in evidence_rows


def _assert_reserved_boundaries_do_not_publish_claims(contract: dict[str, Any]) -> None:
    rows = _matrix_rows()
    reserved_boundaries = contract.get("reserved_boundaries")

    assert isinstance(reserved_boundaries, list)
    assert reserved_boundaries
    for boundary in reserved_boundaries:
        assert boundary["public_status"] == "reserved"
        assert boundary["matrix_owner"] == contract["reserved_umbrella"]
        assert "reason" in boundary

        owner_row = rows[boundary["matrix_owner"]]
        assert owner_row["state"] == "reserved"
        assert "support_claims" not in owner_row


def _assert_object_model_support_contracts_are_source_derived(
    contract: dict[str, Any],
) -> None:
    implemented_rows = {
        row["capability_id"]: row for row in contract["implemented_rows"]
    }
    support_contracts = contract.get("implemented_support_contracts")

    assert isinstance(support_contracts, list)
    assert len(support_contracts) == 4

    covered_scopes = set()
    for support_contract in support_contracts:
        capability_id = support_contract["capability_id"]
        support_claim = support_contract["support_claim"]
        implemented_row = implemented_rows[capability_id]

        assert implemented_row["support_claim"] == support_claim
        assert support_contract["contract_id"].startswith(
            "objc3c.object-model.public-support."
        )
        assert support_contract["public_command"].startswith("npm run objc3c -- ")
        assert "full-realization" not in support_claim

        source_truth = support_contract["source_truth"]
        positive_evidence = support_contract["positive_evidence"]
        negative_evidence = support_contract["negative_evidence"]
        assert len(source_truth) >= 3
        assert len(positive_evidence) >= 3
        assert len(negative_evidence) >= 2

        for path in (*source_truth, *positive_evidence, *negative_evidence):
            assert not str(path).startswith("tmp/")
            assert (ROOT / path).exists(), path

        covered_scopes.add(support_contract["contract_scope"])

    assert any("class and metaclass identity" in scope for scope in covered_scopes)
    assert any("category attachment" in scope for scope in covered_scopes)
    assert any("property and ivar layout" in scope for scope in covered_scopes)
    assert any("public capability truth" in scope for scope in covered_scopes)


def test_object_model_public_capability_split_matches_capability_matrix() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_split_contract_matches_source_truth(contract)


def test_object_model_public_support_contracts_are_source_derived() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_object_model_support_contracts_are_source_derived(contract)


def test_object_model_reserved_boundaries_stay_non_claiming() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_reserved_boundaries_do_not_publish_claims(contract)


def test_advanced_runtime_public_capability_split_matches_capability_matrix() -> None:
    contract = build_advanced_runtime_capability_split_contract()

    assert contract["issue"] == 8155
    _assert_split_contract_matches_source_truth(contract)


def test_advanced_runtime_reserved_boundaries_stay_non_claiming() -> None:
    contract = build_advanced_runtime_capability_split_contract()

    assert contract["issue"] == 8155
    _assert_reserved_boundaries_do_not_publish_claims(contract)
