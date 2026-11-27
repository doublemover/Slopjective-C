from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = (
    ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
)
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "stdlib_collections"
    / "runtime_backed_collection_claims_contract.json"
)
COLLECTIONS_MODULE_PATH = (
    ROOT / "stdlib" / "modules" / "objc3.collections" / "module.json"
)
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "
COLLECTIONS_POSITIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/"
    "stdlib_foundation_next_collections_helpers.objc3"
)
COLLECTIONS_NEGATIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/negative/"
    "stdlib_foundation_next_collections_helper_signature_conflict.objc3"
)
FOUNDATION_NEXT_PROBE = "tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp"


def _read_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def _expected_claim_ids(contract: dict[str, Any]) -> set[str]:
    return {str(claim["support_claim"]) for claim in contract["claims"]}


def test_collection_groundwork_does_not_publish_reserved_collection_claims() -> None:
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    rows = [row for row in catalog["rows"] if isinstance(row, dict)]
    collection_rows = [
        row
        for row in rows
        if str(row.get("support_claim", "")) in _expected_claim_ids(contract)
    ]

    assert {row["support_claim"] for row in collection_rows} == _expected_claim_ids(
        contract
    )

    forbidden_fragments = {
        "foundation",
        "nsarray",
        "nsdictionary",
        "hashing",
        "interpolation",
    }
    for row in collection_rows:
        claim_text = row["support_claim"].lower()
        capability_text = row["capability_id"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_fragments)
        assert not any(fragment in capability_text for fragment in forbidden_fragments)
