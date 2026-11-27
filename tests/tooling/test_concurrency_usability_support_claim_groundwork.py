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
