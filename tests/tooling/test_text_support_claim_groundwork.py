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
    / "stdlib_text"
    / "runtime_backed_text_claims_contract.json"
)
TEXT_MODULE_PATH = ROOT / "stdlib" / "modules" / "objc3.text" / "module.json"
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "
TEXT_POSITIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/"
    "stdlib_foundation_next_text_helpers.objc3"
)
TEXT_NEGATIVE_FIXTURE = (
    "tests/tooling/fixtures/native/execution/negative/"
    "stdlib_foundation_next_text_helper_signature_conflict.objc3"
)
FOUNDATION_NEXT_PROBE = "tests/tooling/runtime/stdlib_foundation_next_runtime_probe.cpp"
STRING_TEXT_MODEL_PROBE = "tests/tooling/runtime/string_text_model_runtime_probe.cpp"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_text_groundwork_does_not_publish_reserved_text_claims() -> None:
    catalog = _read_json(CATALOG_PATH)
    text_rows = [
        row for row in catalog["rows"] if ".stdlib.text." in str(row.get("support_claim", ""))
    ]

    assert {row["support_claim"] for row in text_rows} == {
        "objc3c.behavior.stdlib.text.string-view-runtime-shape",
        "objc3c.behavior.stdlib.text.byte-span-runtime-shape",
        "objc3c.behavior.stdlib.text.unicode-scalar-iteration",
        "objc3c.behavior.stdlib.text.basic-formatting",
        "objc3c.behavior.stdlib.text.equality-comparison",
        "objc3c.behavior.stdlib.text.runtime-builder-interpolation",
    }

    forbidden_fragments = {
        "owned-string",
        "normalization",
        "foundation",
        "nsstring",
    }
    for row in text_rows:
        claim_text = row["support_claim"].lower()
        capability_text = row["capability_id"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_fragments)
        assert not any(fragment in capability_text for fragment in forbidden_fragments)
