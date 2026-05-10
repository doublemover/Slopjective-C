from __future__ import annotations

from typing import Any


def assert_manifest_object_ir_truth_gate_positive_run_tokens(
    summary: dict[str, Any],
) -> None:
    assert summary["positive_runs"]["run1"]["ir_tokens"][
        "manifest_object_ir_truth_gate = contract=objc3c.manifest.object.ir.truth.gate.v1"
    ] is True
