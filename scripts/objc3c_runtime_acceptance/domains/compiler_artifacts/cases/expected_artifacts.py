"""Expected compiler artifact paths and names for acceptance cases."""

from __future__ import annotations

from pathlib import Path

from ..catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME
from ..catalog import ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME
from ..catalog import ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX


COMPILE_PROVENANCE_FILE_NAME = "module.compile-provenance.json"
ARTIFACT_REGISTRY_EMIT_PREFIX = ARTIFACT_REGISTRY_SHARED_EMIT_PREFIX
ARTIFACT_REGISTRY_IMPORT_SURFACE_NAMES = (
    ARTIFACT_REGISTRY_IMPORT_SURFACE_A_NAME,
    ARTIFACT_REGISTRY_IMPORT_SURFACE_B_NAME,
)


def compile_provenance_path(compile_dir: Path) -> Path:
    return compile_dir / COMPILE_PROVENANCE_FILE_NAME


def artifact_registry_import_surface_path(
    case_dir: Path,
    surface_name: str,
) -> Path:
    return case_dir / surface_name


__all__ = [
    "ARTIFACT_REGISTRY_EMIT_PREFIX",
    "ARTIFACT_REGISTRY_IMPORT_SURFACE_NAMES",
    "COMPILE_PROVENANCE_FILE_NAME",
    "artifact_registry_import_surface_path",
    "compile_provenance_path",
]
