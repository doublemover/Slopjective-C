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
FORMAL_CONTRACT_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "ownership_memory_model" / "formal_model_contract.json"
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


def _formal_contract_failures(contract: dict[str, Any]) -> list[str]:
    failures: list[str] = []

    diagnostic_codes = {
        str(entry["code"])
        for entry in contract.get("diagnostic_contracts", [])
        if isinstance(entry, dict)
    }
    for flow_name, flow in contract.get("flow_contracts", {}).items():
        if not isinstance(flow, dict):
            failures.append(f"{flow_name}: flow contract must be an object")
            continue
        for code in flow.get("diagnostic_codes", []):
            if str(code) not in diagnostic_codes:
                failures.append(f"{flow_name}: diagnostic {code} is not declared")
        non_goal = str(flow.get("non_goal", ""))
        if flow_name == "arc_boundary_lowering" and "public ARC runtime ABI" not in non_goal:
            failures.append("arc_boundary_lowering: public ARC ABI non-goal drifted")

    for record in contract.get("source_records", []):
        if not isinstance(record, dict):
            failures.append("source record must be an object")
            continue
        path = str(record.get("path", ""))
        if path.startswith(("tmp/", "tmp\\")) or "generated" in path:
            failures.append(f"{path}: source record is not durable source truth")
            continue
        source_path = ROOT / path
        if not source_path.is_file():
            failures.append(f"{path}: source record missing")
            continue
        text = source_path.read_text(encoding="utf-8")
        for token in record.get("required_tokens", []):
            if str(token) not in text:
                failures.append(f"{path}: required token missing: {token}")

    return failures


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


def test_ownership_memory_model_formal_slice_is_source_derived() -> None:
    support_contract = _read_json(CONTRACT_PATH)
    contract = _read_json(FORMAL_CONTRACT_PATH)

    assert contract["contract_id"] == "objc3c.ownership.memory-model.formal-slice.v1"
    assert contract["issue_ref"] == 8166
    assert contract["support_claim"] == CLAIM_ID
    assert contract["support_claim"] == support_contract["support_claim"]
    assert set(contract["ownership_kinds"]) == {
        "strong_owned",
        "weak",
        "unowned",
        "borrowed_reference",
        "consumed_value",
        "autoreleased_result",
    }

    flows = contract["flow_contracts"]
    assert flows["consumed_values"]["positive_fixture"] in support_contract["positive_evidence"]
    assert flows["consumed_values"]["negative_fixture"] in support_contract["negative_evidence"]
    assert flows["borrowed_references"]["positive_fixture"] in support_contract["positive_evidence"]
    assert "summary-only" not in flows["arc_boundary_lowering"]["rule"]
    assert "public ARC runtime ABI" in flows["arc_boundary_lowering"]["non_goal"]

    assert _formal_contract_failures(contract) == []


def test_ownership_memory_model_formal_slice_fails_closed_on_source_drift() -> None:
    contract = _read_json(FORMAL_CONTRACT_PATH)
    drifted = json.loads(json.dumps(contract))
    drifted["source_records"][0]["required_tokens"].append(
        "missing-objc3-ownership-memory-model-token"
    )

    failures = _formal_contract_failures(drifted)

    assert failures == [
        "native/objc3c/src/sema/objc3_sema_contract_ownership_memory_model.h: "
        "required token missing: missing-objc3-ownership-memory-model-token"
    ]
