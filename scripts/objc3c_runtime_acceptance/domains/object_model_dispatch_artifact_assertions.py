"""Canonical object dispatch compile artifact assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect

from ..runtime_contract_object_model import RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID


def assert_canonical_dispatch_compile_artifacts(
    manifest: dict[str, Any],
    ll_text: str,
) -> None:
    dispatch_table_reflection_record_lowering_surface = manifest.get(
        "runtime_dispatch_table_reflection_record_lowering_surface", {}
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("contract_id")
        == RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        "expected canonical dispatch compile manifest to publish dispatch-table/reflection-record lowering surface",
    )
    expect(
        "; executable_realization_records = contract=objc3c.executable.realization.records.v1"
        in ll_text,
        "expected canonical dispatch LLVM IR to publish executable realization records",
    )
    expect(
        dispatch_table_reflection_record_lowering_surface.get("selector_pool_section_root_symbol")
        == "@__objc3_sec_selector_pool",
        "expected canonical dispatch lowering surface to preserve the selector pool section root",
    )


__all__ = ["assert_canonical_dispatch_compile_artifacts"]
