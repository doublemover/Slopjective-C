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


def test_runtime_backed_collection_claims_are_dedicated_module_contracts() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    collections_module = _read_json(COLLECTIONS_MODULE_PATH)
    semantic_policy = _read_json(SEMANTIC_POLICY_PATH)

    assert 8161 in catalog["issue_refs"]

    fixture_paths = {
        str(fixture["path"]) for fixture in manifest["fixtures"] if isinstance(fixture, dict)
    }
    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)
    }
    contract_claims = {
        str(claim["support_claim"]): claim
        for claim in contract["claims"]
        if isinstance(claim, dict)
    }
    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}

    for claim_id, expected in contract_claims.items():
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]
        matrix_row = matrix_rows[expected["capability_id"]]

        assert matrix_row["state"] == "implemented"
        assert matrix_row["support_claims"] == [claim_id]
        assert claim["owner_phase"] == "runtime"
        assert row["owner_phase"] == "runtime"
        assert row["capability_id"] == expected["capability_id"]
        assert row["conformance_fixture"] == (
            "tests/tooling/fixtures/stdlib_collections/"
            "runtime_backed_collection_claims_contract.json"
        )
        assert row["traceability_fixture"] == "stdlib/modules/objc3.collections/module.json"
        assert row["runnable_command"] == claim["executable_command"]
        assert row["runnable_command"].startswith(PUBLIC_COMMAND_PREFIX)
        assert row["runtime_acceptance_case"] == "stdlib-foundation-next-runtime-probe"

        assert claim["behavior_fixture"] in fixture_paths
        assert claim["behavior_fixture"] == COLLECTIONS_POSITIVE_FIXTURE
        assert claim["behavior_fixture"] in row["positive_evidence"]
        assert FOUNDATION_NEXT_PROBE in row["positive_evidence"]
        assert "stdlib/modules/objc3.collections/module.objc3" in row["positive_evidence"]
        assert (
            "native/objc3c/src/runtime/stdlib/collections_runtime_contract.h"
            in row["positive_evidence"]
        )
        assert COLLECTIONS_NEGATIVE_FIXTURE in row["negative_evidence"]
        assert {"O3S206"}.issubset(set(row["required_diagnostic_codes"]))

        assert (
            expected["module"]
            == collections_module["canonical_module"]
            == "objc3.collections"
        )
        assert expected["api_family"] in collections_module["api_families"]
        assert set(expected["exports"]).issubset(set(collections_module["exports"]))
        assert set(expected["runtime_abi"]).issubset(
            set(collections_module["runtime_abi"])
        )
        assert set(expected["semantic_policy_keys"]).issubset(
            set(semantic_policy["collection_semantics"])
        )

        requirement_text = " ".join(row["source_truth_requirements"]).lower()
        for reserved in expected["reserved_until_runtime_storage"]:
            assert reserved in requirement_text

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)

        executable_evidence = {
            (item["path"], item.get("command"))
            for item in matrix_row["evidence"]
            if item["kind"] == "test"
        }
        assert (claim["behavior_fixture"], claim["executable_command"]) in executable_evidence
        assert (
            FOUNDATION_NEXT_PROBE,
            "npm run objc3c -- test-runtime-acceptance-fast",
        ) in executable_evidence


def test_collection_groundwork_does_not_publish_reserved_collection_claims() -> None:
    catalog = _read_json(CATALOG_PATH)
    rows = [row for row in catalog["rows"] if isinstance(row, dict)]
    collection_rows = [
        row for row in rows if ".stdlib.collections." in str(row.get("support_claim", ""))
    ]

    assert {row["support_claim"] for row in collection_rows} == {
        "objc3c.behavior.stdlib.collections.array-slice-runtime-shape",
        "objc3c.behavior.stdlib.collections.map-entry-runtime-shape",
        "objc3c.behavior.stdlib.collections.set-iteration-runtime-shape",
    }

    forbidden_fragments = {
        "literal",
        "generic",
        "iteration",
        "foundation",
    }
    reserved_rows = [
        row
        for row in collection_rows
        if row["support_claim"]
        != "objc3c.behavior.stdlib.collections.set-iteration-runtime-shape"
    ]
    for row in reserved_rows:
        claim_text = row["support_claim"].lower()
        capability_text = row["capability_id"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_fragments)
        assert not any(fragment in capability_text for fragment in forbidden_fragments)
