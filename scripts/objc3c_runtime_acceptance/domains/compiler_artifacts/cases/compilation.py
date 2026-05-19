"""Compiler artifact case compilation helpers."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.fixture_compilation import run_fixture_compile
from objc3c_runtime_acceptance.registry import (
    ACCEPTANCE_ARTIFACT_REGISTRY,
)

from ..assertions import expect_artifact_registry_compile_success
from ..assertions import expect_backend_compile_success
from ..assertions import expect_direct_artifact_registry_backend
from ..catalog import ARTIFACT_REGISTRY_REUSE_POLICY
from .expected_artifacts import ARTIFACT_REGISTRY_EMIT_PREFIX
from .models import ArtifactRegistryIsolationLayout
from .models import CompileBackendParityLayout
from .models import CompileBackendRun


def compile_parity_fixture(
    layout: CompileBackendParityLayout,
    out_dir: Path,
    *,
    backend: str,
    backend_label: str,
) -> CompileBackendRun:
    result, selected_backend = run_fixture_compile(
        layout.fixture,
        out_dir,
        backend=backend,
        extra_args=list(layout.parity_args),
    )
    expect_backend_compile_success(backend_label, result)
    return CompileBackendRun(result=result, selected_backend=selected_backend)


def compile_direct_for_registry(
    layout: ArtifactRegistryIsolationLayout,
    out_dir: Path,
    extra_args: tuple[str, ...],
) -> CompileBackendRun:
    result, selected_backend = run_fixture_compile(
        layout.fixture,
        out_dir,
        extra_args=list(extra_args),
        backend=layout.backend,
        reuse_policy=ARTIFACT_REGISTRY_REUSE_POLICY,
    )
    expect_artifact_registry_compile_success(layout.fixture, result)
    expect_direct_artifact_registry_backend(selected_backend)
    ACCEPTANCE_ARTIFACT_REGISTRY.validate_artifacts(
        out_dir,
        ARTIFACT_REGISTRY_EMIT_PREFIX,
    )
    return CompileBackendRun(result=result, selected_backend=selected_backend)


__all__ = [
    "compile_direct_for_registry",
    "compile_parity_fixture",
]
