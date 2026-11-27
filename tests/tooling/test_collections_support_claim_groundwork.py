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
ISSUE_EVIDENCE_PATH = ROOT / "docs" / "issues" / "objc3_next_8153_8179_evidence.md"
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "stdlib_collections"
    / "runtime_backed_collection_claims_contract.json"
)
COLLECTIONS_MODULE_PATH = ROOT / "stdlib" / "modules" / "objc3.collections" / "module.json"
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "
COLLECTIONS_CONFORMANCE_FIXTURE = (
    "tests/tooling/fixtures/stdlib_collections/"
    "runtime_backed_collection_claims_contract.json"
)
COLLECTIONS_POSITIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/"
    "stdlib_foundation_next_collections_helpers.objc3"
)
COLLECTIONS_NEGATIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/negative/"
    "stdlib_foundation_next_collections_helper_signature_conflict.objc3"
)
FOUNDATION_NEXT_PROBE = "tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp"
EXPECTED_RESERVED_PUBLIC_SURFACES = {
    "non-i32 collection storage",
    "non-i32 hashing",
    "Foundation collection bridging",
    "collection display or string interpolation",
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _read_repo_text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def _issue_boundary(issue_number: int) -> str:
    prefix = f"| #{issue_number} |"
    for line in ISSUE_EVIDENCE_PATH.read_text(encoding="utf-8").splitlines():
        if line.startswith(prefix):
            cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
            assert len(cells) == 4
            return cells[3]
    raise AssertionError(f"missing issue evidence row #{issue_number}")


def _expected_claim_ids(contract: dict[str, Any]) -> set[str]:
    return {str(claim["support_claim"]) for claim in contract["claims"]}


def _expected_runtime_abi(contract: dict[str, Any]) -> set[str]:
    return {
        str(symbol)
        for claim in contract["claims"]
        for symbol in claim["runtime_abi"]
    }


def test_collection_groundwork_does_not_publish_reserved_collection_claims() -> None:
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    runtime_collection_rows = [
        row
        for row in catalog["rows"]
        if str(row.get("support_claim", "")) in _expected_claim_ids(contract)
    ]

    assert {row["support_claim"] for row in runtime_collection_rows} == _expected_claim_ids(
        contract
    )

    forbidden_fragments = {
        "foundation",
        "nsarray",
        "nsdictionary",
        "hashing",
        "interpolation",
    }
    for row in runtime_collection_rows:
        claim_text = row["support_claim"].lower()
        capability_text = row["capability_id"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_fragments)
        assert not any(fragment in capability_text for fragment in forbidden_fragments)


def test_issue_8161_boundary_names_reserved_collection_surfaces() -> None:
    contract = _read_json(CONTRACT_PATH)
    boundary = _issue_boundary(8161).lower()

    for reserved_surface in contract["reserved_public_surfaces"]:
        reserved_text = str(reserved_surface).lower()
        assert reserved_text in boundary
