"""Path and JSON loading helpers for the conformance corpus surface checker."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Mapping

from objc3c_tooling.json_io import load_json_any
from objc3c_tooling.paths import repo_rel

from .contracts import SurfaceValidationError


@dataclass(frozen=True)
class ConformanceCorpusPaths:
    root: Path

    @property
    def corpus_surface_path(self) -> Path:
        return self.root / "tests" / "conformance" / "corpus_surface.json"

    @property
    def longitudinal_suites_path(self) -> Path:
        return self.root / "tests" / "conformance" / "longitudinal_suites.json"

    @property
    def summary_path(self) -> Path:
        return self.root / "tmp" / "reports" / "conformance" / "corpus-surface-summary.json"

    def require_path(self, relative_path: str, *, kind: str) -> Path:
        path = self.root / relative_path
        if not path.exists():
            raise RuntimeError(f"missing {kind}: {relative_path}")
        return path


def load_surface(paths: ConformanceCorpusPaths) -> Mapping[str, Any]:
    if not paths.corpus_surface_path.is_file():
        raise SurfaceValidationError(
            f"missing corpus surface contract: {repo_rel(paths.corpus_surface_path)}"
        )
    surface = load_json_any(paths.corpus_surface_path)
    if not isinstance(surface, dict):
        raise SurfaceValidationError("corpus surface contract is not an object")
    return surface


def load_longitudinal_suites(paths: ConformanceCorpusPaths) -> Mapping[str, Any]:
    longitudinal_suites = load_json_any(paths.longitudinal_suites_path)
    if not isinstance(longitudinal_suites, dict):
        raise SurfaceValidationError("longitudinal_suites contract is not an object")
    return longitudinal_suites


def load_manifest_payload(manifest_path: Path) -> Mapping[str, Any]:
    payload = load_json_any(manifest_path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"{repo_rel(manifest_path)} is not a manifest object")
    return payload


__all__ = (
    "ConformanceCorpusPaths",
    "load_longitudinal_suites",
    "load_manifest_payload",
    "load_surface",
)
