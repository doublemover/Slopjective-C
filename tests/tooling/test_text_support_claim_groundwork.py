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


def test_runtime_backed_text_claims_are_dedicated_module_contracts() -> None:
    matrix = _read_json(MATRIX_PATH)
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)
    contract = _read_json(CONTRACT_PATH)
    text_module = _read_json(TEXT_MODULE_PATH)
    semantic_policy = _read_json(SEMANTIC_POLICY_PATH)

    assert 8162 in catalog["issue_refs"]

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
            "tests/tooling/fixtures/stdlib_text/"
            "runtime_backed_text_claims_contract.json"
        )
        assert row["traceability_fixture"] == "stdlib/modules/objc3.text/module.json"
        assert row["runnable_command"] == claim["executable_command"]
        assert row["runnable_command"].startswith(PUBLIC_COMMAND_PREFIX)
        assert row["runtime_acceptance_case"] == expected.get(
            "runtime_acceptance_case", "stdlib-foundation-next-runtime-probe"
        )

        assert claim["behavior_fixture"] in fixture_paths
        assert claim["behavior_fixture"] == TEXT_POSITIVE_FIXTURE
        assert claim["behavior_fixture"] in row["positive_evidence"]
        for evidence_path in expected.get("positive_evidence", [FOUNDATION_NEXT_PROBE]):
            assert evidence_path in row["positive_evidence"]
        assert "stdlib/modules/objc3.text/module.objc3" in row["positive_evidence"]
        assert "native/objc3c/src/runtime/stdlib/text_runtime_contract.h" in row["positive_evidence"]
        assert TEXT_NEGATIVE_FIXTURE in row["negative_evidence"]
        assert {"O3S206"}.issubset(set(row["required_diagnostic_codes"]))

        assert expected["module"] == text_module["canonical_module"] == "objc3.text"
        assert expected["api_family"] in text_module["api_families"]
        assert set(expected["exports"]).issubset(set(text_module["exports"]))
        assert set(expected["runtime_abi"]).issubset(set(text_module["runtime_abi"]))
        assert set(expected["semantic_policy_keys"]).issubset(
            set(semantic_policy["text_semantics"])
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
        if expected.get("runtime_acceptance_case") == "string-text-model-runtime-probe":
            assert (STRING_TEXT_MODEL_PROBE, "python -m pytest tests/tooling/test_string_text_model_runtime.py") in executable_evidence
        else:
            assert (FOUNDATION_NEXT_PROBE, "npm run objc3c -- test-runtime-acceptance-fast") in executable_evidence

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


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
