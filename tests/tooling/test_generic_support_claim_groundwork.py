from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "

EXPECTED_GENERIC_ROWS = {
    "objc3c.behavior.language.generics.protocol-qualified-arguments": {
        "capability_id": "language.generics.protocol-qualified-arguments",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-protocol-generic-positive",
        "required_positive": {
            "tests/conformance/semantic/TYP-8013-15.json",
            "scripts/objc3c_type_semantic_model_closure/positive.py",
            "native/objc3c/src/sema/objc3_semantic_passes_generic_protocol_message_validation.inc",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_generic_unknown_protocol.objc3",
            "tests/conformance/semantic/TYP-8013-09.json",
        },
        "required_codes": {"O3S206"},
    },
    "objc3c.behavior.language.generics.variance-specialization": {
        "capability_id": "language.generics.variance-specialization",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-generic-variance-positive",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3",
            "tests/conformance/semantic/TYP-8013-12.json",
            "tests/conformance/semantic/TYP-8013-13.json",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3",
        },
        "required_codes": {"O3S206", "O3S216"},
    },
    "objc3c.behavior.runtime.generics.cross-module-metadata": {
        "capability_id": "runtime.generics.cross-module-metadata",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
        "runtime_acceptance_case": "type-semantic-model-cross-module-generic-contract",
        "required_positive": {
            "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
            "tests/conformance/semantic/TYP-8013-17.json",
            "native/objc3c/src/pipeline/runtime_import_type_system_preservation_generic.cpp",
            "tests/tooling/fixtures/objc3c/validation_generic_metadata_abi_contract/replay_run_1/module.manifest.json",
            "tests/tooling/fixtures/objc3c/validation_lightweight_generics_constraints_contract/replay_run_1/module.manifest.json",
        },
        "required_negative": {
            "scripts/objc3c_type_semantic_model_closure/cross_module.py",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3",
            "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3",
        },
        "required_codes": {"O3S206"},
    },
}

EXPECTED_GENERIC_FIXTURES = {
    "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3": ("positive", ""),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_constraint_violation.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_invariant_assignment.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_nested_generic_constraint_violation.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_generic_substitution_unknown_message.objc3": (
        "diagnostic_negative",
        "O3S216",
    ),
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_generic_unknown_protocol.objc3": (
        "diagnostic_negative",
        "O3S206",
    ),
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_generic_support_claim_groundwork_links_manifest_and_catalog_rows() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    assert 8160 in catalog["issue_refs"]

    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row for row in catalog["rows"] if isinstance(row, dict)
    }

    for claim_id, expected in EXPECTED_GENERIC_ROWS.items():
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]

        assert claim["owner_phase"] == expected["owner_phase"]
        assert claim["behavior_fixture"] == expected["behavior_fixture"]
        assert claim["executable_command"].startswith(PUBLIC_COMMAND_PREFIX)

        assert row["capability_id"] == expected["capability_id"]
        assert row["owner_phase"] == expected["owner_phase"]
        assert row["runnable_command"] == claim["executable_command"]
        assert row["runtime_acceptance_case"] == expected["runtime_acceptance_case"]

        positive = set(row["positive_evidence"])
        negative = set(row["negative_evidence"])
        assert len(positive) == len(row["positive_evidence"])
        assert len(negative) == len(row["negative_evidence"])
        assert claim["behavior_fixture"] in positive
        assert expected["required_positive"].issubset(positive)
        assert expected["required_negative"].issubset(negative)
        assert expected["required_codes"].issubset(set(row["required_diagnostic_codes"]))
        assert row["source_truth_requirements"]

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


def test_generic_fixture_manifest_rows_are_canonical_and_diagnostic_specific() -> None:
    manifest = _read_json(MANIFEST_PATH)
    fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    for fixture_path, (fixture_kind, diagnostic_code) in EXPECTED_GENERIC_FIXTURES.items():
        fixture = fixtures[fixture_path]
        assert fixture["origin"] == "hand-authored"
        assert fixture["owner_phase"] == "sema"
        assert fixture["behavior_family"] == "generics"
        assert fixture["fixture_kind"] == fixture_kind
        assert fixture["expected_diagnostic_code"] == diagnostic_code
        _assert_repo_path_exists(fixture_path)
