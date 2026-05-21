from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = (
    ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
)
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "

EXPECTED_BLOCK_ROWS = {
    "objc3c.behavior.language.blocks.escape-capture-legality": {
        "capability_id": "language.blocks.escape-capture-legality",
        "owner_phase": "sema",
        "behavior_fixture": "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
        "runtime_acceptance_case": "escaping-block-capture-legality",
        "required_positive": {
            "tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json",
            "scripts/objc3c_runtime_acceptance/domains/block_arc_surface_ownership_transfer.py",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/weak_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/unowned_object_capture_mutation_negative.objc3",
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
        },
        "required_codes": {"O3S301", "O3P313"},
    },
    "objc3c.behavior.runtime.blocks.copy-dispose-invoke": {
        "capability_id": "runtime.blocks.copy-dispose-invoke",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
        "runtime_acceptance_case": "block-helper-runtime-execution",
        "required_positive": {
            "tests/tooling/fixtures/native/executable_block_object_invoke_thunk_positive.objc3",
            "tests/tooling/runtime/block_runtime_copy_dispose_invoke_probe.cpp",
            "tests/tooling/runtime/block_runtime_owned_capture_lifetime_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/execution/negative/escaping_owned_object_block_conflicting_capture.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_owned_capture_negative.objc3",
        },
        "required_codes": {"O3S301"},
    },
    "objc3c.behavior.runtime.blocks.byref-forwarding": {
        "capability_id": "runtime.blocks.byref-forwarding",
        "owner_phase": "runtime",
        "behavior_fixture": "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
        "runtime_acceptance_case": "block-helper-runtime-execution",
        "required_positive": {
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_positive.objc3",
            "tests/tooling/fixtures/native/execution/positive/byref_capture_argument_materialization.objc3",
            "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
        },
        "required_negative": {
            "tests/tooling/fixtures/native/execution/negative/byref_capture_missing_identifier.objc3",
            "tests/tooling/fixtures/native/execution/negative/byref_capture_duplicate_binding.objc3",
            "tests/tooling/fixtures/native/escaping_block_runtime_hook_byref_negative.objc3",
        },
        "required_codes": {"O3P313", "O3S301"},
    },
}


def _read_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_block_support_claim_groundwork_links_manifest_and_catalog_rows() -> None:
    manifest = _read_json(MANIFEST_PATH)
    catalog = _read_json(CATALOG_PATH)

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

    for claim_id, expected in EXPECTED_BLOCK_ROWS.items():
        claim = manifest_claims[claim_id]
        row = catalog_rows[claim_id]

        assert claim["owner_phase"] == expected["owner_phase"]
        assert claim["behavior_fixture"] == expected["behavior_fixture"]
        assert claim["behavior_fixture"] in fixture_paths
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

        for path in [
            row["conformance_fixture"],
            row["traceability_fixture"],
            *row["positive_evidence"],
            *row["negative_evidence"],
        ]:
            _assert_repo_path_exists(path)
