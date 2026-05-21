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


EXPECTED_RUNTIME_SPLIT_ROWS = {
    "objc3c.behavior.runtime.error-live-bridge-cleanup": {
        "capability_id": "runtime.errors.live-bridge-cleanup",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
        "command": "npm run objc3c -- validate-error-conformance",
        "required_evidence": {
            "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
            "tests/tooling/runtime/error_runtime_bridge_helper_probe.cpp",
            "tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/bridge_legality_nserror_missing_out_negative.objc3",
            "tests/tooling/fixtures/native/throwing_call_requires_try_negative.objc3",
        },
    },
    "objc3c.behavior.runtime.concurrency-task-continuation-lifecycle": {
        "capability_id": "runtime.concurrency.task-continuation-lifecycle",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
        "command": "npm run objc3c -- validate-concurrency-conformance",
        "required_evidence": {
            "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
            "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
            "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/non_async_task_runtime_rejected.objc3",
            "tests/tooling/fixtures/native/task_group_without_scope_rejected.objc3",
        },
    },
    "objc3c.behavior.runtime.concurrency-actor-mailbox-isolation": {
        "capability_id": "runtime.concurrency.actor-mailbox-isolation",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3",
        "command": "npm run objc3c -- validate-concurrency-conformance",
        "required_evidence": {
            "tests/native/runtime/concurrency/actor_executor_contract.objc3",
            "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
            "tests/tooling/runtime/actor_runtime_executor_contract_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/actor_nonisolated_executor_rejected.objc3",
            "tests/tooling/fixtures/native/non_actor_actor_hop_rejected.objc3",
        },
    },
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_runtime_split_support_claims_are_matrix_manifest_and_catalog_backed() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}
    manifest_claims = {claim["claim_id"]: claim for claim in manifest["support_claims"]}
    catalog_rows = {row["support_claim"]: row for row in catalog["rows"]}

    assert 8155 in catalog["issue_refs"]

    for claim_id, expected in EXPECTED_RUNTIME_SPLIT_ROWS.items():
        matrix_row = matrix_rows[expected["capability_id"]]
        manifest_claim = manifest_claims[claim_id]
        catalog_row = catalog_rows[claim_id]

        assert matrix_row["state"] == "implemented"
        assert matrix_row["support_claims"] == [claim_id]
        assert manifest_claim["owner_phase"] == expected["owner_phase"]
        assert manifest_claim["behavior_fixture"] == expected["behavior_fixture"]
        assert manifest_claim["executable_command"] == expected["command"]

        assert catalog_row["capability_id"] == expected["capability_id"]
        assert catalog_row["owner_phase"] == expected["owner_phase"]
        assert catalog_row["runnable_command"] == expected["command"]
        assert expected["behavior_fixture"] in catalog_row["positive_evidence"]
        assert expected["required_evidence"] <= set(catalog_row["positive_evidence"])
        assert expected["required_negative"] <= set(catalog_row["negative_evidence"])

        executable_evidence = {
            (item["path"], item.get("command"))
            for item in matrix_row["evidence"]
            if item["kind"] == "test"
        }
        assert (expected["behavior_fixture"], expected["command"]) in executable_evidence

        for path in [
            expected["behavior_fixture"],
            catalog_row["conformance_fixture"],
            catalog_row["traceability_fixture"],
            *catalog_row["positive_evidence"],
            *catalog_row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)
