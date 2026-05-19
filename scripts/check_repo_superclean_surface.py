#!/usr/bin/env python3
"""Validate the build-emitted repo superclean source-of-truth artifact."""

from __future__ import annotations

from check_repo_superclean_surface_model import (
    SurfaceReportWriter,
    load_surface_payload,
    missing_surface_report,
    validate_surface_payload,
)
from repo_superclean_surface.paths import REPO_SUPERCLEAN_SOURCE_OF_TRUTH


SURFACE_PATH = REPO_SUPERCLEAN_SOURCE_OF_TRUTH


def main() -> int:
    writer = SurfaceReportWriter()
    if not SURFACE_PATH.is_file():
        return writer.write(missing_surface_report(SURFACE_PATH))

    payload = load_surface_payload(SURFACE_PATH)
    return writer.write(validate_surface_payload(payload))


if __name__ == "__main__":
    raise SystemExit(main())
