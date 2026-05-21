from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = (
    ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
)
CONTRACT_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "ownership_memory_model" / "support_claim_contract.json"
)
CLAIM_ID = "objc3c.behavior.language.ownership-memory-model"
PUBLIC_COMMAND = "npm run objc3c -- validate-conformance-corpus"


def _read_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_ownership_memory_model_support_claim_is_durable_groundwork() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)

    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)
    }
    fixture_paths = {
        str(fixture["path"]) for fixture in manifest["fixtures"] if isinstance(fixture, dict)
    }

    claim = manifest_claims[CLAIM_ID]
    row = catalog_rows[CLAIM_ID]

    assert contract["issue_ref"] == 8166
    assert contract["support_claim"] == CLAIM_ID
    assert contract["public_command"] == PUBLIC_COMMAND
    assert claim["owner_phase"] == "sema"
    assert claim["behavior_fixture"] == (
        "tests/tooling/fixtures/native/borrowed_retainable_abi_completion_positive.objc3"
    )
    assert claim["behavior_fixture"] in fixture_paths
    assert claim["executable_command"] == PUBLIC_COMMAND

    assert row["capability_id"] == contract["capability_id"]
    assert row["owner_phase"] == contract["owner_phase"]
    assert row["runnable_command"] == PUBLIC_COMMAND
    assert row["conformance_fixture"] == (
        "tests/tooling/fixtures/ownership_memory_model/support_claim_contract.json"
    )
    assert row["runtime_acceptance_case"] == "ownership-memory-model-groundwork"

    positive = set(row["positive_evidence"])
    negative = set(row["negative_evidence"])
    assert set(contract["positive_evidence"]).issubset(positive)
    assert set(contract["negative_evidence"]).issubset(negative)
    assert "tests/tooling/fixtures/ownership_memory_model/support_claim_contract.json" in positive
    assert set(contract["required_diagnostic_codes"]).issubset(
        set(row["required_diagnostic_codes"])
    )
    assert "O3S221" in row["required_diagnostic_codes"]

    for path in [
        row["conformance_fixture"],
        row["traceability_fixture"],
        *row["positive_evidence"],
        *row["negative_evidence"],
    ]:
        _assert_repo_path_exists(path)
