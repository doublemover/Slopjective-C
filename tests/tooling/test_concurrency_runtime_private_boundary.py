from __future__ import annotations

from pathlib import Path

from scripts.objc3c_runtime_acceptance.c_api import (
    PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY,
)
from scripts.objc3c_runtime_acceptance.domains.concurrency_runtime_abi_surface import (
    build_runtime_unified_concurrency_runtime_abi_surface,
)
from scripts.objc3c_runtime_acceptance.domains.concurrency_source_surface import (
    build_runtime_unified_concurrency_source_surface,
)


ROOT = Path(__file__).resolve().parents[2]
REQUIRED_PRIVATE_CONCURRENCY_SYMBOLS = {
    "objc3_runtime_cancel_async_continuation_i32",
    "objc3_runtime_executor_hop_i32",
}


def test_private_concurrency_runtime_boundaries_include_cancel_and_executor_helpers() -> None:
    source_surface = build_runtime_unified_concurrency_source_surface([])
    abi_surface = build_runtime_unified_concurrency_runtime_abi_surface([])

    assert REQUIRED_PRIVATE_CONCURRENCY_SYMBOLS <= set(
        PRIVATE_UNIFIED_CONCURRENCY_RUNTIME_ABI_BOUNDARY
    )
    assert REQUIRED_PRIVATE_CONCURRENCY_SYMBOLS <= set(
        source_surface["private_concurrency_runtime_boundary"]
    )
    assert REQUIRED_PRIVATE_CONCURRENCY_SYMBOLS <= set(
        abi_surface["private_unified_concurrency_runtime_abi_boundary"]
    )


def test_native_concurrency_manifest_emitters_include_cancel_and_executor_helpers() -> None:
    source_manifest = (
        ROOT
        / "native/objc3c/src/artifacts/objc3_frontend_artifact_runtime_concurrency_manifest.cpp"
    )
    abi_manifest = (
        ROOT
        / "native/objc3c/src/artifacts/objc3_frontend_artifact_runtime_concurrency_abi_manifest.cpp"
    )
    metadata_publication = (
        ROOT
        / "native/objc3c/src/ir/objc3_ir_frontend_metadata_publication_concurrency_continuation_runtime.cpp"
    )
    summary_publication = (
        ROOT
        / "native/objc3c/src/lower/contracts/lowering_summary_ownership_concurrency_surfaces.cpp"
    )

    source_manifest_text = source_manifest.read_text(encoding="utf-8")
    assert "objc3_runtime_cancel_async_continuation_i32" in source_manifest_text
    assert "objc3_runtime_executor_hop_i32" in source_manifest_text

    assert "kObjc3RuntimeCancelAsyncContinuationI32Symbol" in abi_manifest.read_text(
        encoding="utf-8"
    )
    assert (
        "kObjc3RuntimeCancelAsyncContinuationI32Symbol"
        in metadata_publication.read_text(encoding="utf-8")
    )
    summary_text = summary_publication.read_text(encoding="utf-8")
    assert "kObjc3RuntimeCancelAsyncContinuationI32Symbol" in summary_text
    assert "kObjc3RuntimeExecutorHopI32Symbol" in summary_text
