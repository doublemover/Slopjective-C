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
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
ADVANCED_RUNTIME_FEATURE_FAMILIES = {
    "arc",
    "blocks",
    "concurrency",
    "errors",
    "interop",
    "metaprogramming",
    "property",
}
ADVANCED_RUNTIME_PUBLIC_STATUSES = {"implemented", "reserved", "rejected", "internal"}


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


def _manifest_claims() -> dict[str, dict[str, Any]]:
    manifest = _read_json(MANIFEST_PATH)
    return {str(row["claim_id"]): row for row in manifest["support_claims"]}


def _assert_split_contract_matches_source_truth(contract: dict[str, Any]) -> None:
    rows = _matrix_rows()
    evidence_rows = _evidence_rows()
    umbrella = rows[contract["reserved_umbrella"]]

    if umbrella["state"] == "reserved":
        assert "support_claims" not in umbrella
        assert {
            (contract["reserved_umbrella"], None, contract["source"]),
            (
                contract["reserved_umbrella"],
                None,
                "tests/tooling/test_runtime_capability_public_split.py",
            ),
        } <= evidence_rows
    else:
        assert umbrella["state"] == "implemented"
        expected_umbrella_claims = {
            "runtime.object-model.full-realization": [
                "objc3c.behavior.runtime.object-model.full-realization"
            ],
            "language.advanced-runtime-closure": [
                "objc3c.behavior.language.advanced-runtime-closure"
            ],
        }
        assert umbrella["support_claims"] == expected_umbrella_claims[
            contract["reserved_umbrella"]
        ]

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
        assert owner_row["state"] in {"reserved", "implemented"}
        assert "support_claim" not in boundary


def _assert_object_model_full_realization_readiness_is_promoted_evidence(
    contract: dict[str, Any],
) -> None:
    readiness_evidence = contract.get("full_realization_readiness_evidence")

    assert isinstance(readiness_evidence, list)
    assert len(readiness_evidence) == 1

    row = readiness_evidence[0]
    assert row["issue"] == 8198
    assert row["capability_id"] == contract["reserved_umbrella"]
    assert row["public_status"] == "implemented"
    assert row["support_claim_published"] is True
    assert row["remaining_blockers"] == ()

    assert row["covered_axes"] == (
        "class",
        "metaclass",
        "category",
        "protocol",
        "property",
        "ivar",
        "selector",
        "public-reflection",
        "registration-replay",
    )
    for path in (
        row["contract_path"],
        row["combined_positive_fixture"],
        row["public_reflection_probe"],
    ):
        assert not str(path).startswith("tmp/")
        assert (ROOT / path).exists(), path

    for command in row["public_commands"]:
        assert str(command).startswith("npm run objc3c -- ")


def test_object_model_public_capability_split_matches_capability_matrix() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_split_contract_matches_source_truth(contract)


def test_object_model_reserved_boundaries_stay_non_claiming() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_reserved_boundaries_do_not_publish_claims(contract)


def test_object_model_full_realization_readiness_is_promoted_evidence() -> None:
    contract = build_object_model_capability_split_contract()

    assert contract["issue"] == 8154
    _assert_object_model_full_realization_readiness_is_promoted_evidence(contract)


def test_advanced_runtime_public_capability_split_matches_capability_matrix() -> None:
    contract = build_advanced_runtime_capability_split_contract()

    assert contract["issue"] == 8155
    _assert_split_contract_matches_source_truth(contract)


def test_advanced_runtime_reserved_boundaries_stay_non_claiming() -> None:
    contract = build_advanced_runtime_capability_split_contract()

    assert contract["issue"] == 8155
    _assert_reserved_boundaries_do_not_publish_claims(contract)
