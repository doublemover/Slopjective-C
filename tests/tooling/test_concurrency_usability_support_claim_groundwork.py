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
    / "stdlib_concurrency"
    / "public_concurrency_usability_claims_contract.json"
)
CONCURRENCY_MODULE_PATH = ROOT / "stdlib" / "modules" / "objc3.concurrency" / "module.json"
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_public_concurrency_claims_are_bounded_to_runtime_backed_stdlib_api() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    concurrency_module = _read_json(CONCURRENCY_MODULE_PATH)
    semantic_policy = _read_json(SEMANTIC_POLICY_PATH)

    assert 8167 in catalog["issue_refs"]
    assert contract["module"] == "objc3.concurrency"

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

    module_exports = set(concurrency_module["exports"])
    module_runtime_abi = set(concurrency_module["runtime_abi"])
    module_families = set(concurrency_module["api_families"])
    concurrency_semantics = semantic_policy["concurrency_semantics"]

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
        assert row["conformance_fixture"] == (
            "tests/tooling/fixtures/stdlib_concurrency/"
            "public_concurrency_usability_claims_contract.json"
        )
        assert row["traceability_fixture"] == "stdlib/modules/objc3.concurrency/module.json"
        assert row["runnable_command"] == claim["executable_command"]
        assert row["runnable_command"].startswith(PUBLIC_COMMAND_PREFIX)
        assert row["runtime_acceptance_case"] == "stdlib-concurrency-runtime-probe"

        assert claim["behavior_fixture"] in fixture_paths
        assert claim["behavior_fixture"] in row["positive_evidence"]
        assert "stdlib/modules/objc3.concurrency/module.objc3" in row["positive_evidence"]
        assert "stdlib/modules/objc3.concurrency/smoke.objc3" in row["positive_evidence"]
        assert "tests/tooling/runtime/stdlib_concurrency_runtime_probe.cpp" in row[
            "positive_evidence"
        ]
        assert (
            "tests/tooling/fixtures/native/execution/negative/"
            "stdlib_concurrency_runtime_helper_signature_conflict.objc3"
        ) in row["negative_evidence"]
        assert {"O3S206", "O3S342", "O3S343"}.issubset(
            set(row["required_diagnostic_codes"])
        )

        assert expected["api_family"] in module_families
        assert set(expected["exports"]).issubset(module_exports)
        assert set(expected["runtime_abi"]).issubset(module_runtime_abi)
        assert set(expected["semantic_policy_keys"]).issubset(concurrency_semantics)

        source_truth = " ".join(row["source_truth_requirements"]).lower()
        for reserved in expected["reserved_until_runtime_contract"]:
            assert reserved.lower() in source_truth

        matrix_evidence = {
            (item["path"], item.get("command"))
            for item in matrix_row["evidence"]
            if item["kind"] == "test"
        }
        assert (
            "tests/tooling/fixtures/stdlib_concurrency/"
            "public_concurrency_usability_claims_contract.json",
            "npm run objc3c -- test-runtime-acceptance-concurrency",
        ) in matrix_evidence

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


def test_public_concurrency_groundwork_does_not_publish_broad_async_claims() -> None:
    catalog = _read_json(CATALOG_PATH)
    public_rows = [
        row
        for row in catalog["rows"]
        if ".stdlib.concurrency.public-" in str(row.get("support_claim", ""))
    ]

    assert {row["support_claim"] for row in public_rows} == {
        "objc3c.behavior.stdlib.concurrency.public-task-spawn-api",
        "objc3c.behavior.stdlib.concurrency.public-task-group-cancellation-api",
        "objc3c.behavior.stdlib.concurrency.public-executor-hop-api",
        "objc3c.behavior.stdlib.concurrency.public-actor-mailbox-api",
    }

    forbidden_fragments = {
        "distributed",
        "swift-abi",
        "scheduler-fairness",
        "generic-task-abi",
        "os-scheduler",
        "cross-process",
    }
    for row in public_rows:
        claim_text = row["support_claim"].lower()
        capability_text = row["capability_id"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_fragments)
        assert not any(fragment in capability_text for fragment in forbidden_fragments)
