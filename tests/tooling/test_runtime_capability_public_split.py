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


def test_object_model_public_capability_split_matches_capability_matrix() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_split_contract_matches_source_truth(contract)


def test_advanced_runtime_public_capability_split_matches_capability_matrix() -> None:
    contract = build_advanced_runtime_capability_split_contract()

    assert contract["issue"] == 8155
    _assert_split_contract_matches_source_truth(contract)
