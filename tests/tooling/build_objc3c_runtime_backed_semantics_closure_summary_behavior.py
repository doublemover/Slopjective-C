from __future__ import annotations

from typing import Any


def assert_runtime_backed_semantics_summary_behavior(
    summary: dict[str, Any],
) -> None:
    assert summary["counts"]["positive_fixture_count"] == 7
    assert summary["counts"]["negative_fixture_count"] == 10
    assert summary["counts"]["runtime_helper_symbol_count"] == 36
    assert summary["counts"]["required_ir_token_count"] == 17
    assert summary["checks"]["no_source_truth_under_tmp"] is True
