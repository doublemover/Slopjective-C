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
