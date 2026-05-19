from __future__ import annotations

from typing import Any


def assert_object_model_ir_lowering_layout_offsets(summary: dict[str, Any]) -> None:
    assert summary["ir_offsets_by_property"]["token"] == 8
    assert summary["ir_offsets_by_property"]["childFlag"] == 16
    assert summary["expected_layout"]["childFlag"]["owner_size"] == 24
