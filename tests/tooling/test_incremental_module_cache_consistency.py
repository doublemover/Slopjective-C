from __future__ import annotations

from collections.abc import Callable
from copy import deepcopy
import json
from pathlib import Path
import sys

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

import check_objc3c_incremental_module_cache_consistency as checker  # noqa: E402
from check_objc3c_incremental_module_cache_consistency import (  # noqa: E402
    build_summary,
    replay_key_for_surface,
    surface_is_consistent,
)


def write_mutated_contract(
    tmp_path: Path,
    mutate: Callable[[dict[str, object]], None],
) -> Path:
    contract = json.loads(checker.CONTRACT_PATH.read_text(encoding="utf-8"))
    mutate(contract)
    contract_path = tmp_path / "incremental_runtime_metadata_consistency.json"
    contract_path.write_text(json.dumps(contract, indent=2) + "\n", encoding="utf-8")
    return contract_path


def diagnostic_codes(summary: dict[str, object]) -> set[str]:
    diagnostics = summary["diagnostics"]
    assert isinstance(diagnostics, list)
    return {
        str(diagnostic["code"])
        for diagnostic in diagnostics
        if isinstance(diagnostic, dict)
    }


def test_incremental_module_cache_consistency_summary_passes() -> None:
    summary = build_summary()

    assert summary["status"] == "PASS"
    assert summary["issues"] == ["#8081", "#8082"]
    assert summary["missing_source_paths"] == []
    assert all(summary["native_checks"].values())
    assert len(set(summary["positive_replay_keys"])) == 1
    assert summary["diagnostics"] == []
    assert len(summary["source_sha256"]) == 4
    assert all(not path.startswith("tmp/") for path in summary["source_paths"])


def test_incremental_module_cache_checker_runs_from_nothing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    summary_path = tmp_path / "fresh" / "reports" / "summary.json"
    assert not summary_path.parent.exists()
    monkeypatch.setattr(checker, "SUMMARY_PATH", summary_path)

    assert checker.main() == 0

    summary = json.loads(summary_path.read_text(encoding="utf-8"))
    assert summary["status"] == "PASS"
    assert summary["diagnostics"] == []
    assert summary["missing_source_paths"] == []


def test_incremental_module_cache_rejects_missing_checked_in_cache_input(tmp_path: Path) -> None:
    missing_path = "tests/tooling/fixtures/module_cache/missing_replay_manifest.json"

    def mutate(contract: dict[str, object]) -> None:
        manifests = contract["positive_replay_manifests"]
        assert isinstance(manifests, list)
        manifests[0] = missing_path

        source_truth = contract["checked_in_source_truth"]
        assert isinstance(source_truth, dict)
        cache_inputs = source_truth["cache_inputs"]
        assert isinstance(cache_inputs, list)
        cache_inputs[0] = missing_path
        input_sha256 = source_truth["input_sha256"]
        assert isinstance(input_sha256, dict)
        input_sha256[missing_path] = "0" * 64

    summary = build_summary(write_mutated_contract(tmp_path, mutate))

    assert summary["status"] == "FAIL"
    assert missing_path in summary["missing_source_paths"]
    assert checker.DIAG_MISSING_INPUT in diagnostic_codes(summary)


def test_incremental_module_cache_rejects_tmp_artifact_source_truth(tmp_path: Path) -> None:
    tmp_source_path = "tmp/reports/module-cache/stale/module.manifest.json"

    def mutate(contract: dict[str, object]) -> None:
        manifests = contract["positive_replay_manifests"]
        assert isinstance(manifests, list)
        manifests[0] = tmp_source_path

    summary = build_summary(write_mutated_contract(tmp_path, mutate))

    assert summary["status"] == "FAIL"
    assert checker.DIAG_TMP_INPUT in diagnostic_codes(summary)
    assert any(tmp_source_path in failure for failure in summary["failures"])


def test_incremental_module_cache_rejects_stale_cache_input_digest(tmp_path: Path) -> None:
    stale_path = (
        "tests/tooling/fixtures/objc3c/validation_incremental_module_cache_contract/"
        "replay_run_1/module.manifest.json"
    )

    def mutate(contract: dict[str, object]) -> None:
        source_truth = contract["checked_in_source_truth"]
        assert isinstance(source_truth, dict)
        input_sha256 = source_truth["input_sha256"]
        assert isinstance(input_sha256, dict)
        input_sha256[stale_path] = "0" * 64

    summary = build_summary(write_mutated_contract(tmp_path, mutate))

    assert summary["status"] == "FAIL"
    assert checker.DIAG_STALE_INPUT in diagnostic_codes(summary)
    assert any("digest drifted" in failure and stale_path in failure for failure in summary["failures"])


def test_incremental_module_cache_candidate_partition_allows_deterministic_invalidation() -> None:
    surface = {
        "incremental_module_cache_invalidation_sites": 7,
        "namespace_segment_sites": 5,
        "import_edge_candidate_sites": 5,
        "object_pointer_type_sites": 8,
        "pointer_declarator_sites": 3,
        "normalized_sites": 4,
        "cache_invalidation_candidate_sites": 3,
        "contract_violation_sites": 0,
        "deterministic": True,
    }

    assert surface_is_consistent(surface)
    assert replay_key_for_surface(
        surface,
        lane_contract="objc3c.incremental.module.cache.invalidation.lowering.v1",
    ).endswith(
        "cache_invalidation_candidate_sites=3;contract_violation_sites=0;"
        "deterministic=true;lane_contract=objc3c.incremental.module.cache.invalidation.lowering.v1"
    )


def test_incremental_module_cache_candidate_partition_rejects_count_drift() -> None:
    surface = {
        "incremental_module_cache_invalidation_sites": 7,
        "normalized_sites": 4,
        "cache_invalidation_candidate_sites": 4,
        "contract_violation_sites": 0,
        "deterministic": True,
    }

    assert not surface_is_consistent(surface)


def test_incremental_module_cache_runtime_replay_key_rejects_drift() -> None:
    summary = build_summary()
    surface = deepcopy(summary["positive_cache_candidate_surface"])
    surface["runtime_metadata_replay_key"] = "incremental_module_cache_invalidation_sites=5"

    assert surface_is_consistent(surface)
    assert surface["runtime_metadata_replay_key"] != replay_key_for_surface(
        surface,
        lane_contract="objc3c.incremental.module.cache.invalidation.lowering.v1",
    )
