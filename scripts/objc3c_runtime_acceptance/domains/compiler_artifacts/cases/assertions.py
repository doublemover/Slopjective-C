"""Compiler artifact case-level assertions."""

from __future__ import annotations

from typing import Any

from ..assertions import expect_artifact_digest_parity
from ..assertions import expect_compile_truthfulness_parity
from ..assertions import expect_direct_compile_backend_provenance
from ..assertions import expect_distinct_artifact_cache_keys
from ..assertions import expect_no_distinct_args_reuse
from ..assertions import expect_reuse_event_paths
from ..assertions import expect_same_args_reuse
from .models import ArtifactRegistryIsolationLayout
from .models import CompileBackendParityArtifacts


def expect_compile_backend_parity_artifacts(
    artifacts: CompileBackendParityArtifacts,
) -> None:
    compared_fields = list(artifacts.compared_truthfulness_fields)
    expect_compile_truthfulness_parity(
        artifacts.direct_truthfulness,
        artifacts.wrapper_truthfulness,
        compared_fields,
    )
    expect_artifact_digest_parity(
        artifacts.direct_provenance,
        artifacts.wrapper_provenance,
    )
    expect_direct_compile_backend_provenance(artifacts.direct_provenance)


def expect_artifact_registry_distinct_args_not_reused(
    reuse_before: int,
    reuse_after_distinct_args: int,
) -> None:
    expect_no_distinct_args_reuse(
        reuse_before,
        reuse_after_distinct_args,
    )


def expect_artifact_registry_same_args_reused(
    layout: ArtifactRegistryIsolationLayout,
    *,
    reuse_before: int,
    reuse_after_same_args: int,
    reuse_event: dict[str, Any],
) -> None:
    expect_same_args_reuse(reuse_before, reuse_after_same_args)
    expect_reuse_event_paths(
        reuse_event,
        producer_dir=layout.producer_dir,
        consumer_dir=layout.consumer_dir,
    )


def expect_artifact_registry_cache_isolation(
    changed_args_key_a: str,
    changed_args_key_b: str,
    changed_import_surface_key_a: str,
    changed_import_surface_key_b: str,
) -> None:
    expect_distinct_artifact_cache_keys(
        changed_args_key_a,
        changed_args_key_b,
        changed_import_surface_key_a,
        changed_import_surface_key_b,
    )


__all__ = [
    "expect_artifact_registry_cache_isolation",
    "expect_artifact_registry_distinct_args_not_reused",
    "expect_artifact_registry_same_args_reused",
    "expect_compile_backend_parity_artifacts",
]
