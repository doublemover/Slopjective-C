#!/usr/bin/env python3
"""Validate the live reader-facing documentation surface and machine-appendix boundary."""

from __future__ import annotations

from check_documentation_surface_model import (
    DOCUMENTATION_SURFACE_MODEL,
    DocumentationSurfaceReportWriter,
)


def main() -> int:
    return DocumentationSurfaceReportWriter().write(DOCUMENTATION_SURFACE_MODEL.validate())


if __name__ == "__main__":
    raise SystemExit(main())
