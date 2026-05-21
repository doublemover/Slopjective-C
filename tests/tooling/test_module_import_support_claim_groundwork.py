from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "

EXPECTED_MODULE_IMPORT_ROWS = {
    "objc3c.behavior.modules.public-import-lookup": {
        "capability_id": "modules.public-import-lookup",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/module_import_lookup_consumer.objc3",
        "runnable_command": "npm run objc3c -- validate-conformance-corpus",
        "runtime_acceptance_case": "module-roundtrip-public-import-lookup",
        "required_positive": {
            "tests/tooling/fixtures/native/module_import_lookup_provider.objc3",
            "tests/conformance/module_roundtrip/MOD-53-01.json",
            "native/objc3c/src/driver/objc3_driver_cross_module_imported_surfaces.h",
            "native/objc3c/src/io/objc3_cross_module_imported_modules_document.h",
        },
        "required_negative": {
            "tests/conformance/module_roundtrip/MOD-53-02.json",
            "tests/conformance/diagnostics/DIAG-GRP-07.json",
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
        },
        "required_codes": {"O3S200"},
    },
    "objc3c.behavior.runtime.modules.imported-runtime-packaging-replay": {
        "capability_id": "runtime.modules.imported-runtime-packaging-replay",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3",
        "runnable_command": "npm run objc3c -- validate-interop-conformance",
        "runtime_acceptance_case": "imported-runtime-packaging-replay",
        "required_positive": {
            "tests/tooling/fixtures/native/runtime_packaging_provider.objc3",
            "tests/tooling/runtime/import_module_execution_matrix_probe.cpp",
            "tests/tooling/runtime/multi_image_registration_reset_replay_probe.cpp",
            "native/objc3c/src/pipeline/runtime_import_preservation.cpp",
            "native/objc3c/src/pipeline/objc3_runtime_import_surface.h",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/missing_replay_proof_rejected.objc3",
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
            "scripts/objc3c_runtime_acceptance/domains/interop_packaging_semantic_mixed_image.py",
        },
        "required_codes": {"O3S200", "O3RT004"},
    },
}

EXPECTED_MODULE_IMPORT_FIXTURES = {
    "tests/tooling/fixtures/native/module_import_lookup_provider.objc3": ("sema", "module-import", "positive", ""),
    "tests/tooling/fixtures/native/module_import_lookup_consumer.objc3": ("sema", "module-import", "positive", ""),
    "tests/tooling/fixtures/native/runtime_packaging_provider.objc3": ("runtime", "interop", "positive", ""),
    "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3": ("runtime", "interop", "positive", ""),
    "tests/tooling/fixtures/native/missing_replay_proof_rejected.objc3": ("runtime", "module-import", "strict-error", "O3RT004"),
    "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3": ("sema", "module-import", "diagnostic_negative", "O3S200"),
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_module_import_support_claims_are_manifest_and_catalog_backed() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

    assert 8163 in catalog["issue_refs"]

    manifest_claims = {
        str(claim["claim_id"]): claim
        for claim in manifest["support_claims"]
        if isinstance(claim, dict)
    }
    catalog_rows = {
        str(row["support_claim"]): row
        for row in catalog["rows"]
        if isinstance(row, dict)
    }

    for claim_id, expected in EXPECTED_MODULE_IMPORT_ROWS.items():
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]

        assert claim["owner_phase"] == expected["owner_phase"]
        assert claim["behavior_fixture"] == expected["behavior_fixture"]
        assert claim["executable_command"] == expected["runnable_command"]
        assert claim["executable_command"].startswith(PUBLIC_COMMAND_PREFIX)

        assert row["capability_id"] == expected["capability_id"]
        assert row["owner_phase"] == expected["owner_phase"]
        assert row["runnable_command"] == expected["runnable_command"]
        assert row["runtime_acceptance_case"] == expected["runtime_acceptance_case"]

        positive = set(row["positive_evidence"])
        negative = set(row["negative_evidence"])
        assert len(positive) == len(row["positive_evidence"])
        assert len(negative) == len(row["negative_evidence"])
        assert claim["behavior_fixture"] in positive
        assert expected["required_positive"] <= positive
        assert expected["required_negative"] <= negative
        assert expected["required_codes"] <= set(row["required_diagnostic_codes"])
        assert row["source_truth_requirements"]

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)


def test_module_import_fixture_manifest_rows_are_canonical_and_bounded() -> None:
    manifest = _read_json(MANIFEST_PATH)
    fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    for fixture_path, (owner_phase, behavior_family, fixture_kind, diagnostic_code) in EXPECTED_MODULE_IMPORT_FIXTURES.items():
        fixture = fixtures[fixture_path]
        assert fixture["origin"] == "hand-authored"
        assert fixture["owner_phase"] == owner_phase
        assert fixture["behavior_family"] == behavior_family
        assert fixture["fixture_kind"] == fixture_kind
        assert fixture["expected_diagnostic_code"] == diagnostic_code
        _assert_repo_path_exists(fixture_path)
