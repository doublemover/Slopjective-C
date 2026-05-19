#!/usr/bin/env python3
"""Validate the build-emitted repo superclean source-of-truth artifact."""

from __future__ import annotations

from pathlib import Path

from check_repo_superclean_surface_model import (
    SurfaceReportWriter,
    load_surface_payload,
    validate_surface_payload,
    write_surface_payload,
)


ROOT = Path(__file__).resolve().parents[1]
SURFACE_PATH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "repo_superclean_source_of_truth.json"


def main() -> int:
    writer = SurfaceReportWriter()
    if not SURFACE_PATH.is_file():
        write_surface_payload(SURFACE_PATH)

    payload = load_surface_payload(SURFACE_PATH)
    return writer.write(validate_surface_payload(payload))


if __name__ == "__main__":
    raise SystemExit(main())
