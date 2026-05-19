"""Payload-facing contract helpers for the repo superclean surface checker."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Mapping

from .fields import REPO_SUPERCLEAN_SURFACE_MODEL
from .model import CHECKER_NAME, SurfaceReport


def missing_surface_report(surface_path: Path) -> SurfaceReport:
    return SurfaceReport(
        CHECKER_NAME,
        (f"missing source-of-truth artifact: {surface_path}",),
    )


def load_surface_payload(surface_path: Path) -> Any:
    return json.loads(surface_path.read_text(encoding="utf-8"))


def build_surface_payload() -> dict[str, Any]:
    return REPO_SUPERCLEAN_SURFACE_MODEL.expected_payload()


def write_surface_payload(surface_path: Path) -> None:
    surface_path.parent.mkdir(parents=True, exist_ok=True)
    surface_path.write_text(
        json.dumps(build_surface_payload(), indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )


def validate_surface_payload(payload: Mapping[str, Any]) -> SurfaceReport:
    return REPO_SUPERCLEAN_SURFACE_MODEL.validate(payload)


__all__ = [
    "REPO_SUPERCLEAN_SURFACE_MODEL",
    "build_surface_payload",
    "load_surface_payload",
    "missing_surface_report",
    "validate_surface_payload",
    "write_surface_payload",
]
