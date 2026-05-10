from __future__ import annotations

from typing import Any


def assert_manifest_object_ir_truth_gate_counts_and_checks(
    summary: dict[str, Any],
) -> None:
    assert summary["counts"]["required_artifact_count"] == 12
    assert summary["counts"]["deterministic_artifact_count"] == 10
    assert summary["counts"]["required_object_section_count"] == 12
    assert summary["checks"]["deterministic_artifact_hashes"] is True
    assert summary["checks"]["negative_diagnostics_deterministic"] is True
    assert summary["checks"]["no_source_truth_under_tmp"] is True
