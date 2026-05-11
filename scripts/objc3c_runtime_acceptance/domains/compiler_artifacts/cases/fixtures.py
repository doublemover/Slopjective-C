"""Fixture and argument layout for compiler artifact acceptance cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.compile_backends import DIRECT_COMPILE_BACKEND
from objc3c_runtime_acceptance.progress_format import repo_display_path

from ..catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID
from ..catalog import ARTIFACT_REGISTRY_KEY_ISOLATION_FIXTURE
from ..catalog import COMPILE_BACKEND_PARITY_CACHE_ROOT_NAME
from ..catalog import COMPILE_BACKEND_PARITY_CASE_ID
from ..catalog import COMPILE_BACKEND_PARITY_DIRECT_DIR_NAME
from ..catalog import COMPILE_BACKEND_PARITY_FIXTURE
from ..catalog import COMPILE_BACKEND_PARITY_WRAPPER_DIR_NAME
from ..catalog import bootstrap_ordinal_args
from .expected_artifacts import artifact_registry_import_surface_path
from .models import ArtifactRegistryIsolationLayout
from .models import CompileBackendParityLayout


def compile_backend_parity_layout(run_dir: Path) -> CompileBackendParityLayout:
    case_dir = run_dir / COMPILE_BACKEND_PARITY_CASE_ID
    parity_cache_root = case_dir / COMPILE_BACKEND_PARITY_CACHE_ROOT_NAME
    return CompileBackendParityLayout(
        fixture=COMPILE_BACKEND_PARITY_FIXTURE,
        case_dir=case_dir,
        direct_dir=case_dir / COMPILE_BACKEND_PARITY_DIRECT_DIR_NAME,
        wrapper_dir=case_dir / COMPILE_BACKEND_PARITY_WRAPPER_DIR_NAME,
        parity_cache_root=parity_cache_root,
        parity_args=(
            "--objc3-metaprogramming-cache-root",
            repo_display_path(parity_cache_root),
        ),
    )


def artifact_registry_isolation_layout(
    run_dir: Path,
) -> ArtifactRegistryIsolationLayout:
    case_dir = run_dir / ARTIFACT_REGISTRY_KEY_ISOLATION_CASE_ID
    return ArtifactRegistryIsolationLayout(
        fixture=ARTIFACT_REGISTRY_KEY_ISOLATION_FIXTURE,
        case_dir=case_dir,
        producer_dir=case_dir / "ordinal-21-producer",
        distinct_args_dir=case_dir / "ordinal-22-distinct-args",
        consumer_dir=case_dir / "ordinal-21-consumer",
        args_a=tuple(bootstrap_ordinal_args("21")),
        args_b=tuple(bootstrap_ordinal_args("22")),
        backend=DIRECT_COMPILE_BACKEND,
    )


def artifact_registry_import_surface_args(
    layout: ArtifactRegistryIsolationLayout,
    surface_name: str,
) -> tuple[str, ...]:
    return (
        "--objc3-import-runtime-surface",
        repo_display_path(
            artifact_registry_import_surface_path(layout.case_dir, surface_name)
        ),
    )


__all__ = [
    "artifact_registry_import_surface_args",
    "artifact_registry_isolation_layout",
    "compile_backend_parity_layout",
]
