from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "ownership_concurrency"
    / "async_arc_executor_boundary_contract.json"
)
OWNERSHIP_SUPPORT_PATH = (
    ROOT / "tests" / "tooling" / "fixtures" / "ownership_memory_model" / "support_claim_contract.json"
)
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
CONCURRENCY_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "stdlib_concurrency"
    / "public_concurrency_usability_claims_contract.json"
)
CONCURRENCY_MODULE_PATH = ROOT / "stdlib" / "modules" / "objc3.concurrency" / "module.json"
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_file(path: str) -> Path:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert "generated" not in path.lower()
    source_path = ROOT / path
    assert source_path.is_file(), path
    return source_path


def _catalog_rows() -> dict[str, dict[str, Any]]:
    catalog = _read_json(CATALOG_PATH)
    return {str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)}


def _matrix_rows() -> dict[str, dict[str, Any]]:
    matrix = _read_json(MATRIX_PATH)
    return {str(row["id"]): row for row in matrix["capabilities"] if isinstance(row, dict)}


def _source_record_failures(contract: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    for record in contract.get("source_records", []):
        if not isinstance(record, dict):
            failures.append("source record must be an object")
            continue
        path = str(record.get("path", ""))
        source_path = _assert_repo_file(path)
        text = source_path.read_text(encoding="utf-8")
        for token in record.get("required_tokens", []):
            token_text = str(token)
            if token_text not in text:
                failures.append(f"{path}: required token missing: {token_text}")
    return failures


def test_async_arc_executor_boundary_contract_is_source_derived() -> None:
    contract = _read_json(CONTRACT_PATH)
    ownership_support = _read_json(OWNERSHIP_SUPPORT_PATH)
    catalog_rows = _catalog_rows()
    matrix_rows = _matrix_rows()
    concurrency_contract = _read_json(CONCURRENCY_CONTRACT_PATH)
    concurrency_module = _read_json(CONCURRENCY_MODULE_PATH)
    semantic_policy = _read_json(SEMANTIC_POLICY_PATH)

    assert contract["contract_id"] == "objc3c.ownership.concurrency-boundary.v1"
    assert set(contract["issue_refs"]) == {8166, 8167}
    assert contract["public_command_evidence"] == {
        "ownership": "npm run objc3c -- validate-conformance-corpus",
        "concurrency": "npm run objc3c -- test-runtime-acceptance-concurrency",
    }
    assert set(contract["support_claims"]) == {
        "objc3c.behavior.language.ownership-memory-model",
        "objc3c.behavior.stdlib.concurrency.public-executor-hop-api",
        "objc3c.behavior.stdlib.concurrency.public-actor-mailbox-api",
    }

    ownership_row = catalog_rows["objc3c.behavior.language.ownership-memory-model"]
    ownership_matrix_paths = {
        item["path"]
        for item in matrix_rows["language.ownership.memory-model"]["evidence"]
        if isinstance(item, dict)
    }
    for path in contract["ownership_positive_evidence"]:
        _assert_repo_file(path)
        assert path in ownership_support["positive_evidence"]
        assert path in ownership_row["positive_evidence"]
        assert path in ownership_matrix_paths

    assert set(["O3S221", "O3S308"]).issubset(ownership_row["required_diagnostic_codes"])

    module_exports = set(concurrency_module["exports"])
    module_runtime_abi = set(concurrency_module["runtime_abi"])
    concurrency_claims = {
        claim["support_claim"]: claim
        for claim in concurrency_contract["claims"]
        if isinstance(claim, dict)
    }
    semantic_keys = set(semantic_policy["concurrency_semantics"])

    for expected in contract["concurrency_claims"]:
        claim_id = expected["support_claim"]
        catalog_row = catalog_rows[claim_id]
        contract_claim = concurrency_claims[claim_id]
        matrix_row = matrix_rows[expected["capability_id"]]

        assert catalog_row["capability_id"] == expected["capability_id"]
        assert catalog_row["runtime_acceptance_case"] == "stdlib-concurrency-runtime-probe"
        assert matrix_row["support_claims"] == [claim_id]
        assert set(expected["required_exports"]).issubset(module_exports)
        assert set(expected["required_runtime_abi"]).issubset(module_runtime_abi)
        assert set(expected["required_probe_fields"]).issubset(
            set(contract_claim["runtime_probe_payload_fields"])
        )
        assert set(contract_claim["semantic_policy_keys"]).issubset(semantic_keys)
        for fragment in expected["reserved_fragments"]:
            assert fragment in catalog_row["source_truth_requirements"]

    assert _source_record_failures(contract) == []


def test_async_arc_executor_boundary_rejects_broad_claim_drift() -> None:
    contract = _read_json(CONTRACT_PATH)
    claim_text = json.dumps(
        [
            {
                "support_claim": claim["support_claim"],
                "capability_id": claim["capability_id"],
                "required_exports": claim["required_exports"],
                "required_runtime_abi": claim["required_runtime_abi"],
            }
            for claim in contract["concurrency_claims"]
        ],
        sort_keys=True,
    )

    for fragment in contract["forbidden_public_claim_fragments"]:
        assert fragment not in claim_text

    drifted = json.loads(json.dumps(contract))
    drifted["source_records"][0]["required_tokens"].append(
        "missing-async-arc-executor-boundary-token"
    )

    assert _source_record_failures(drifted) == [
        "docs/spec/ownership.md: required token missing: "
        "missing-async-arc-executor-boundary-token"
    ]
