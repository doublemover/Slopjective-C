from __future__ import annotations

import json
import sys
from pathlib import Path

SCRIPTS_ROOT = Path(__file__).resolve().parents[2] / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.check_objc3c_advanced_runtime_closure import (  # noqa: E402
    ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE,
    ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT,
    ADVANCED_CLOSURE_NEGATIVE_MATRIX,
    REQUIRED_INTERACTION_FEATURE_SETS,
    ROOT,
    validate_advanced_runtime_closure,
)


def _read_json(relative_path: str) -> dict[str, object]:
    path = ROOT / relative_path
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_checked_path(relative_path: str) -> None:
    assert not relative_path.startswith(("tmp/", "temp/", "generated/", "build/", "dist/"))
    assert (ROOT / relative_path).exists()


def test_advanced_runtime_closure_enforces_combined_identity_contract() -> None:
    payload = validate_advanced_runtime_closure()

    assert payload["status"] == "PASS"
    assert payload["advanced_runtime_negative_matrix_case_count"] >= 14
    assert (
        payload["advanced_runtime_negative_matrix_interaction_count"]
        == len(REQUIRED_INTERACTION_FEATURE_SETS)
    )
    assert (
        payload["advanced_runtime_combined_identity_contract"]
        == ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT
    )
    assert payload["advanced_runtime_combined_identity_runtime_state_record_count"] >= 8
    assert payload["advanced_runtime_combined_identity_source_graph_record_count"] >= 7
    assert payload["advanced_runtime_combined_identity_debug_map_record_count"] >= 7
    assert (
        payload["advanced_runtime_combined_identity_abi_interaction_record_count"]
        >= 6
    )
    assert (
        payload["advanced_runtime_combined_identity_interaction_count"]
        == len(REQUIRED_INTERACTION_FEATURE_SETS)
    )
    assert (
        payload["advanced_runtime_canonical_source_debug_map"]
        == ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE
    )
    assert payload["advanced_runtime_canonical_source_map_record_count"] == 7
    assert payload["advanced_runtime_canonical_debug_map_record_count"] == 7
    assert payload["advanced_runtime_canonical_native_line_table_record_count"] == 7


def test_combined_identity_contract_links_source_graph_debug_map_and_negatives() -> None:
    contract = _read_json(ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT)
    negative_matrix = _read_json(ADVANCED_CLOSURE_NEGATIVE_MATRIX)

    assert contract["issue_ref"] == 8199
    assert contract["umbrella_support_promoted"] is False
    _assert_checked_path(str(contract["positive_fixture"]))
    _assert_checked_path(str(contract["negative_matrix"]))
    _assert_checked_path(str(contract["language_semantics_contract"]))
    _assert_checked_path(str(contract["debug_source_map_validator"]))
    _assert_checked_path(str(contract["canonical_compiler_emitted_source_debug_map_bundle"]))

    source_graph_ids = {
        str(record["record_id"])
        for record in contract["compiler_owned_source_graph_records"]  # type: ignore[index]
    }
    debug_source_graph_ids = {
        str(record["source_graph_record_id"])
        for record in contract["debug_map_records"]  # type: ignore[index]
    }
    assert debug_source_graph_ids <= source_graph_ids

    case_ids = {
        str(case["case_id"]) for case in negative_matrix["cases"]  # type: ignore[index]
    }
    interaction_ids = {
        str(record["interaction_id"])
        for record in contract["interaction_records"]  # type: ignore[index]
    }
    assert interaction_ids == set(REQUIRED_INTERACTION_FEATURE_SETS)
    for record in contract["interaction_records"]:  # type: ignore[index]
        assert set(record["negative_case_ids"]) <= case_ids


def test_canonical_source_debug_map_links_every_combined_identity_record() -> None:
    contract = _read_json(ADVANCED_CLOSURE_COMBINED_IDENTITY_CONTRACT)
    bundle = _read_json(ADVANCED_CLOSURE_CANONICAL_SOURCE_DEBUG_MAP_BUNDLE)

    source_map_ids = {
        str(record["entry_id"]) for record in bundle["source_maps"]  # type: ignore[index]
    }
    debug_map_ids = {
        str(record["entry_id"]) for record in bundle["debug_maps"]  # type: ignore[index]
    }
    line_table_source_map_ids = {
        str(record["source_map_entry_id"])
        for record in bundle["native_line_tables"]  # type: ignore[index]
    }
    source_graph_record_kinds = {
        str(record["record_id"]): str(record["source_map_record_kind"])
        for record in contract["compiler_owned_source_graph_records"]  # type: ignore[index]
    }
    source_map_kinds = {
        str(record["entry_id"]): str(record["record_kind"])
        for record in bundle["source_maps"]  # type: ignore[index]
    }

    for record in contract["debug_map_records"]:  # type: ignore[index]
        source_map_id = str(record["source_map_entry_id"])
        assert source_map_id in source_map_ids
        assert str(record["debug_map_entry_id"]) in debug_map_ids
        assert source_map_id in line_table_source_map_ids
        assert (
            source_map_kinds[source_map_id]
            == source_graph_record_kinds[str(record["source_graph_record_id"])]
        )
