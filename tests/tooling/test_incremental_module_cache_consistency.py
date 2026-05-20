from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_objc3c_incremental_module_cache_consistency import (  # noqa: E402
    build_summary,
    replay_key_for_surface,
    surface_is_consistent,
)


def test_incremental_module_cache_consistency_summary_passes() -> None:
    summary = build_summary()

    assert summary["status"] == "PASS"
    assert summary["issues"] == ["#8081", "#8082"]
    assert summary["missing_source_paths"] == []
    assert all(summary["native_checks"].values())
    assert len(set(summary["positive_replay_keys"])) == 1


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
