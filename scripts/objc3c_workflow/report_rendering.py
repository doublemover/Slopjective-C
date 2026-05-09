"""Deterministic JSON rendering for workflow reports."""

from __future__ import annotations

from scripts.objc3c_shared.json_io import render_json

from .report_policy import REPORT_RENDERING_OWNER


def render_report_json(payload: object) -> str:
    return render_json(payload)


__all__ = ["REPORT_RENDERING_OWNER", "render_report_json"]
