from __future__ import annotations

from typing import Any


def assert_cross_module_semantic_contracts_diagnostics_summary(
    summary: dict[str, Any],
) -> None:
    assert summary["status"] == "PASS"
    model = summary["cross_module_semantic_contracts_diagnostics_model"]
    assert (
        model["contract_id"]
        == "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1"
    )
    assert model["deterministic"] is True
    assert model["ready_for_lowering_and_runtime"] is True
    assert model["module_import_graph_sites"] >= 4
    assert model["cross_module_conformance_sites"] >= 4
    assert model["interop_import_module_annotation_sites"] >= 1
    assert model["contract_violation_sites"] == 0
