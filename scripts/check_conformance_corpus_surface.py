#!/usr/bin/env python3
"""Validate the checked-in conformance corpus surface and emit a summary."""

from __future__ import annotations

from pathlib import Path

from check_conformance_corpus_surface_model import (
    ConformanceCorpusPaths,
    ConformanceCorpusSurfaceModel,
    ConformanceCorpusSurfaceReportWriter,
    SurfaceValidationError,
)


ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    paths = ConformanceCorpusPaths(ROOT)
    writer = ConformanceCorpusSurfaceReportWriter(paths)
    model = ConformanceCorpusSurfaceModel(paths)
    try:
        summary = model.build_summary()
    except SurfaceValidationError as exc:
        return writer.write_failure(exc.message)
    return writer.write_success(summary)


if __name__ == "__main__":
    raise SystemExit(main())
