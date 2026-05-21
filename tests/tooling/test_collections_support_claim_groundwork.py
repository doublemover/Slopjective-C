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
    "collection literals",
    "generic element/key/value typing",
    "owned arbitrary-length array storage",
    "array mutation",
    "syntax-level for-in integration",
    "map iteration protocol",
    "non-i32 hashing",
    "set deletion",
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


def test_runtime_backed_collection_claims_are_dedicated_module_contracts() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    collections_module = _read_json(COLLECTIONS_MODULE_PATH)
    semantic_policy = _read_json(SEMANTIC_POLICY_PATH)

    assert 8161 in catalog["issue_refs"]
    assert set(contract["reserved_public_surfaces"]) == EXPECTED_RESERVED_PUBLIC_SURFACES

    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}
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

    assert contract["source_backed_contract"]["module_manifest"] == (
        "stdlib/modules/objc3.collections/module.json"
    )
    assert collections_module["canonical_module"] == "objc3.collections"
    assert set(collections_module["runtime_abi"]) == _expected_runtime_abi(contract)

    for expected in contract["claims"]:
        claim_id = expected["support_claim"]
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]
        matrix_row = matrix_rows[expected["capability_id"]]

        assert matrix_row["state"] == "implemented"
        assert matrix_row["support_claims"] == [claim_id]
        assert claim["owner_phase"] == "runtime"
        assert row["owner_phase"] == "runtime"
        assert row["capability_id"] == expected["capability_id"]
        assert row["conformance_fixture"] == COLLECTIONS_CONFORMANCE_FIXTURE
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
        assert row["negative_evidence"] == [COLLECTIONS_NEGATIVE_FIXTURE]
        assert {"O3S206"}.issubset(set(row["required_diagnostic_codes"]))

        assert expected["module"] == "objc3.collections"
        assert expected["api_family"] in collections_module["api_families"]
        assert set(expected["exports"]).issubset(set(collections_module["exports"]))
        assert set(expected["runtime_abi"]).issubset(set(collections_module["runtime_abi"]))
        assert set(expected["semantic_policy_keys"]).issubset(
            set(semantic_policy["collection_semantics"])
        )

        requirement_text = " ".join(row["source_truth_requirements"]).lower()
        for reserved in expected["reserved_until_runtime_storage"]:
            assert reserved.lower() in requirement_text

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

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


def test_collection_runtime_abi_is_source_backed_by_live_exports() -> None:
    contract = _read_json(CONTRACT_PATH)
    collections_module = _read_json(COLLECTIONS_MODULE_PATH)
    source_backing = contract["source_backed_contract"]

    module_source = _read_repo_text(source_backing["module_source"])
    positive_fixture = _read_repo_text(source_backing["positive_fixture"])
    negative_fixture = _read_repo_text(source_backing["negative_fixture"])
    runtime_header = _read_repo_text(source_backing["runtime_contract_header"])
    runtime_implementation = _read_repo_text(source_backing["runtime_implementation"])
    runtime_probe = _read_repo_text(source_backing["runtime_probe"])
    runtime_acceptance = _read_repo_text(source_backing["runtime_acceptance_domain"])

    for path in contract["source_truth"]:
        _assert_repo_path_exists(path)
    for key, path in source_backing.items():
        if key.endswith(("_fixture", "_source", "_header", "_implementation", "_probe")):
            _assert_repo_path_exists(path)
    _assert_repo_path_exists(source_backing["runtime_acceptance_domain"])

    for symbol in sorted(_expected_runtime_abi(contract)):
        signature = collections_module["runtime_abi_signatures"][symbol]
        assert signature in module_source
        assert signature in positive_fixture
        assert symbol in runtime_header
        assert f'extern "C" int {symbol}' in runtime_implementation
        assert symbol in runtime_probe

    for claim in contract["claims"]:
        for export in claim["exports"]:
            signature = collections_module["abi_signatures"][export]
            assert signature in module_source

    for public_export, value in source_backing["public_status_exports"].items():
        manifest_signature = collections_module["abi_signatures"][public_export]
        assert manifest_signature == f"let {public_export} = {value}"
        assert manifest_signature in module_source

    for status in source_backing["fail_closed_statuses"]:
        constant = status["constant"]
        value = status["value"]
        boundary = status["boundary"]
        assert f"{constant} = {value}" in runtime_header
        assert constant in runtime_implementation
        assert constant in runtime_probe
        assert boundary in " ".join(
            requirement
            for claim in contract["claims"]
            for requirement in claim["supported_now"] + claim["reserved_until_runtime_storage"]
        )

    for field, expected_value in source_backing["runtime_probe_summary_fields"].items():
        assert f'\\"{field}\\":' in runtime_probe
        assert f'payload.get("{field}")' in runtime_acceptance
        assert str(expected_value) in runtime_acceptance

    assert "objc3_runtime_stdlib_collections_array_count_i32(handle: bool)" in (
        negative_fixture
    )


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
        "literal",
        "generic",
        "foundation",
        "nsarray",
        "nsdictionary",
        "arbitrary-length",
        "for-in",
        "delete",
        "hashing",
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
        if reserved_text == "map iteration protocol":
            reserved_text = "map iteration"
        assert reserved_text in boundary
