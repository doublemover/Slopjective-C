from __future__ import annotations

import copy
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_performance_governance_contract_linkage import (  # noqa: E402
    EXPECTED_RUNTIME_CONTRACT_PATHS,
    build_runtime_contract_evidence,
    load_json,
    validate_runtime_contract_linkage,
)


def _budget_model() -> dict[str, object]:
    return load_json(ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json")


def test_runtime_contract_linkage_passes_for_checked_in_governance_surface() -> None:
    summary = validate_runtime_contract_linkage()

    assert summary["status"] == "PASS"
    assert tuple(summary["contract_paths"]) == EXPECTED_RUNTIME_CONTRACT_PATHS
    assert summary["replay_workload_count"] == 7
    assert summary["metadata_resilience_workload_count"] == 1
    assert summary["stress_sanitizer_workload_count"] == 3
    assert summary["release_evidence"]["support_authority"] is False
    assert "storage_ownership_reflection_wall_clock_ms" in summary["runtime_budget_metric_ids"]


def test_runtime_contract_linkage_rejects_missing_budget_link() -> None:
    budget_model = copy.deepcopy(_budget_model())
    linkage = budget_model["runtime_contract_linkage"]
    assert isinstance(linkage, dict)
    linkage["workload_budget_links"] = [
        row
        for row in linkage["workload_budget_links"]
        if row["workload_id"] != "storage-ownership-reflection"
    ]

    with pytest.raises(RuntimeError, match="does not exactly cover runtime-hot-path budget metrics"):
        validate_runtime_contract_linkage(budget_model=budget_model)


def test_runtime_contract_linkage_rejects_source_surface_omission() -> None:
    source_surface = load_json(
        ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "source_surface.json"
    )
    source_surface["checked_in_sources"] = [
        path
        for path in source_surface["checked_in_sources"]
        if path != "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json"
    ]

    with pytest.raises(RuntimeError, match="checked_in_sources missing"):
        validate_runtime_contract_linkage(source_surface=source_surface)


def test_runtime_contract_release_evidence_projects_summary_counts() -> None:
    budget_model = _budget_model()
    runtime_summary = {
        "runtime_contract_summary": {
            "contract_files": [
                {"path": "tests/tooling/fixtures/runtime_performance/workload_replay_contract.json"},
                {"path": "tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json"},
                {"path": "tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json"},
            ],
            "replay": {"row_count": 7},
            "metadata_resilience": {
                "invalid_json_case_count": 1,
                "fuzz_contract_count": 1,
            },
            "stress_sanitizer": {
                "sanitizer_contract_count": 2,
                "stress_scale_contract_count": 3,
            },
        }
    }

    evidence = build_runtime_contract_evidence(
        runtime_summary=runtime_summary,
        budget_model=budget_model,
    )

    assert evidence["status"] == "PASS"
    assert evidence["support_authority"] is False
    assert evidence["contract_paths"] == list(EXPECTED_RUNTIME_CONTRACT_PATHS)
    assert evidence["summary_counts"]["stress_sanitizer.stress_scale_contract_count"] == 3


def test_runtime_contract_release_evidence_fails_closed_on_missing_summary() -> None:
    evidence = build_runtime_contract_evidence(
        runtime_summary={},
        budget_model=_budget_model(),
    )

    assert evidence["status"] == "FAIL"
    assert evidence["failures"] == ["runtime performance summary missing runtime_contract_summary"]
