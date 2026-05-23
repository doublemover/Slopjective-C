from __future__ import annotations

import json
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPTS_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.objc3c_debug_maps import (
    NATIVE_DEBUG_INFO_CONTRACT_ID,
    REQUIRED_CAPABILITY_ROWS,
    REQUIRED_OPTIMIZATION_TRANSFORMS,
    REQUIRED_SOURCE_MAP_RECORD_KINDS,
    inspect_bundle_path,
    load_bundle,
    validate_bundle,
    validate_bundle_path,
)
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS


FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "debug_source_maps"
)
POSITIVE_FIXTURE = FIXTURE_ROOT / "positive.json"
NEGATIVE_CASES = FIXTURE_ROOT / "negative_cases.json"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def set_nested(payload: dict[str, Any], path: list[object], value: object) -> None:
    cursor: Any = payload
    for part in path[:-1]:
        cursor = cursor[part]
    cursor[path[-1]] = value


def mutated_fixture(tmp_path: Path, case: dict[str, Any]) -> Path:
    payload = deepcopy(load_json(POSITIVE_FIXTURE))
    mutations = case["mutations"] if "mutations" in case else [case["mutation"]]
    for mutation in mutations:
        set_nested(payload, mutation["path"], mutation["value"])
    path = tmp_path / f"{case['case_id']}.json"
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return path


def diagnostic_codes(path: Path) -> set[str]:
    return {
        diagnostic.code
        for diagnostic in validate_bundle_path(path).diagnostics
    }


def test_debug_source_map_positive_fixture_validates_source_debug_and_line_links() -> None:
    bundle = load_bundle(POSITIVE_FIXTURE)
    result = validate_bundle(bundle)

    assert result.ok is True
    assert result.diagnostics == ()
    assert {entry.record_kind for entry in bundle.source_maps} == REQUIRED_SOURCE_MAP_RECORD_KINDS
    assert {row.row_id for row in bundle.capability_rows} >= REQUIRED_CAPABILITY_ROWS
    assert {claim.transform_kind for claim in bundle.optimization_claims} == REQUIRED_OPTIMIZATION_TRANSFORMS
    assert len(bundle.source_graph_nodes) == len(REQUIRED_SOURCE_MAP_RECORD_KINDS)
    assert len(bundle.debug_maps) == len(REQUIRED_SOURCE_MAP_RECORD_KINDS)
    assert len(bundle.native_line_tables) == len(REQUIRED_SOURCE_MAP_RECORD_KINDS)
    assert len(bundle.inline_frames) == 1
    assert len(bundle.native_inline_ranges) == 1
    assert len(bundle.inline_debug_chains) == 1
    assert bundle.inline_frame_failures == ()
    assert bundle.native_debug_info.contract_id == NATIVE_DEBUG_INFO_CONTRACT_ID
    assert bundle.native_debug_info.emitted_native_debug_info is True
    assert bundle.native_debug_info.native_line_table_emitted is True
    assert bundle.native_debug_info.statement_stepping_integrated is False
    assert bundle.statement_stepping_supported is False


def test_debug_map_inspection_summary_is_stable_and_diagnostic_backed() -> None:
    payload = inspect_bundle_path(POSITIVE_FIXTURE)

    assert payload["ok"] is True
    assert payload["contract_id"] == "objc3c.debug-map.inspection.v1"
    assert payload["source_map_count"] == len(REQUIRED_SOURCE_MAP_RECORD_KINDS)
    assert payload["native_debug_info_evidence_id"] == "debugmaps.foundation.native-debug-info"
    assert payload["inline_frame_count"] == 1
    assert payload["native_inline_range_count"] == 1
    assert payload["inline_debug_chain_count"] == 1
    assert payload["inline_frame_failure_count"] == 0
    assert set(payload["record_kinds"]) == REQUIRED_SOURCE_MAP_RECORD_KINDS
    assert set(payload["capability_rows"]) >= REQUIRED_CAPABILITY_ROWS
    assert payload["diagnostics"] == []


def test_debug_source_map_public_action_is_registered() -> None:
    action = ACTION_SPECS["validate-debug-source-maps"]

    assert action.backend == "python:scripts/check_objc3c_debug_source_maps.py"
    assert action.validation_tier == "repo"
    assert "validate-debug-source-maps" in ACTION_HANDLERS

    inspect_action = ACTION_SPECS["inspect-debug-map"]
    assert inspect_action.backend == "python:scripts/check_objc3c_debug_source_maps.py --inspect"
    assert inspect_action.pass_through_args is True
    assert "inspect-debug-map" in ACTION_HANDLERS


def test_debug_source_map_negative_case_catalog_is_explicit() -> None:
    cases = load_json(NEGATIVE_CASES)["cases"]

    assert [case["case_id"] for case in cases] == [
        "missing-source-span",
        "stale-source-graph-id",
        "stale-source-digest",
        "generated-code-without-provenance",
        "optimization-without-source-map-preservation",
        "statement-stepping-without-line-table",
        "package-boundary-mismatch",
        "native-line-table-drift",
        "native-debug-info-row-drift",
        "unstable-source-path",
        "missing-capability-row",
        "inline-frame-missing-callee",
        "inline-frame-native-range-drift",
        "inline-frame-non-inlining-proof",
        "inline-frame-failure-record",
    ]


def test_debug_source_maps_reject_missing_source_span(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][0]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_stale_source_graph_id(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][1]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_stale_source_digest(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][2]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_generated_code_without_provenance(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][3]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_optimization_without_source_map_preservation(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][4]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_statement_stepping_without_line_table(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][5]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_package_boundary_mismatch(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][6]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_native_line_table_drift(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][7]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_native_debug_info_row_drift(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][8]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_unstable_source_path(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][9]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_missing_capability_row(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][10]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_inline_frame_missing_callee(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][11]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_inline_frame_native_range_drift(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][12]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_inline_frame_non_inlining_proof(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][13]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_reject_inline_frame_failure_record(tmp_path: Path) -> None:
    case = load_json(NEGATIVE_CASES)["cases"][14]

    assert case["expected_code"] in diagnostic_codes(mutated_fixture(tmp_path, case))


def test_debug_source_maps_return_structured_diagnostic_for_invalid_json(tmp_path: Path) -> None:
    malformed = tmp_path / "malformed.json"
    malformed.write_text("{not-json", encoding="utf-8")

    result = validate_bundle_path(malformed)

    assert result.ok is False
    assert {
        "bundle-json-invalid",
        "contract-id",
        "source-missing",
        "source-digest-invalid",
        "schema-missing-field",
        "schema-type",
    } <= {diagnostic.code for diagnostic in result.diagnostics}
