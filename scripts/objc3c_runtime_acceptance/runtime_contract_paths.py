"""Filesystem roots published by runtime acceptance contracts."""

from __future__ import annotations

from .paths import ROOT


TMP_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-runtime-acceptance"
REPORT_ROOT = ROOT / "tmp" / "reports" / "runtime" / "acceptance"


__all__ = ["REPORT_ROOT", "TMP_ROOT"]
