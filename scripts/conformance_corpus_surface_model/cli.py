"""Command-line entrypoint for the conformance corpus surface checker."""

from __future__ import annotations

from pathlib import Path

from .loading import ConformanceCorpusPaths
from .reports import ConformanceCorpusSurfaceReportWriter
from .validation import ConformanceCorpusSurfaceModel
from .contracts import SurfaceValidationError


ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    paths = ConformanceCorpusPaths(ROOT)
    writer = ConformanceCorpusSurfaceReportWriter(paths)
    model = ConformanceCorpusSurfaceModel(paths)
    try:
        summary = model.build_summary()
    except SurfaceValidationError as exc:
        return writer.write_failure(exc.message)
    return writer.write_success(summary)


__all__ = ("ROOT", "main")
