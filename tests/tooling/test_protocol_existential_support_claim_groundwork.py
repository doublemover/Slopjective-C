from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"

CLAIM_ID = "objc3c.behavior.language.protocols.protocol-qualified-existential-value-flow"
CAPABILITY_ID = "language.protocols.protocol-qualified-existential-value-flow"
BEHAVIOR_FIXTURE = (
    "tests/tooling/fixtures/native/protocol_qualified_existential_value_flow.objc3"
)
PUBLIC_COMMAND = "npm run objc3c -- validate-conformance-corpus"

REQUIRED_POSITIVE_EVIDENCE = {
    BEHAVIOR_FIXTURE,
    "tests/tooling/fixtures/native/type_semantic_model_closure_positive.objc3",
    "tests/conformance/semantic/TYP-8013-01.json",
    "scripts/objc3c_type_semantic_model_closure/positive.py",
    "scripts/objc3c_type_semantic_model_closure/summary_checks.py",
}

REQUIRED_NEGATIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_duplicate_protocol_composition.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_unknown_protocol_composition.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
    "tests/conformance/semantic/TYP-8013-02.json",
    "tests/conformance/semantic/TYP-8013-06.json",
    "tests/conformance/semantic/TYP-8013-07.json",
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_protocol_existential_claim_is_manifest_and_catalog_backed() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    assert 8164 in catalog["issue_refs"]

    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)
    }

    claim = manifest_claims[CLAIM_ID]
    row = catalog_rows[CLAIM_ID]

    assert claim["owner_phase"] == "sema"
    assert claim["behavior_fixture"] == BEHAVIOR_FIXTURE
    assert claim["executable_command"] == PUBLIC_COMMAND

    assert row["capability_id"] == CAPABILITY_ID
    assert row["owner_phase"] == "sema"
    assert row["runnable_command"] == PUBLIC_COMMAND
    assert row["runtime_acceptance_case"] == (
        "type-semantic-model-protocol-qualified-existential-value-flow"
    )
    assert set(row["required_diagnostic_codes"]) >= {"O3S206", "O3S216"}
    assert REQUIRED_POSITIVE_EVIDENCE <= set(row["positive_evidence"])
    assert REQUIRED_NEGATIVE_EVIDENCE <= set(row["negative_evidence"])
    assert row["source_truth_requirements"]

    for path in [
        row["conformance_fixture"],
        row["traceability_fixture"],
        *row["positive_evidence"],
        *row["negative_evidence"],
    ]:
        _assert_repo_path_exists(path)


def test_protocol_existential_fixture_manifest_row_is_narrow_positive_evidence() -> None:
    manifest = _read_json(MANIFEST_PATH)
    fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    fixture = fixtures[BEHAVIOR_FIXTURE]
    assert fixture["origin"] == "hand-authored"
    assert fixture["owner_phase"] == "sema"
    assert fixture["behavior_family"] == "objc"
    assert fixture["fixture_kind"] == "positive"
    assert fixture["expected_diagnostic_code"] == ""
    _assert_repo_path_exists(BEHAVIOR_FIXTURE)
