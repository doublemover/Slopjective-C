"""Deterministic JSON rendering for workflow reports."""

from __future__ import annotations

from scripts.objc3c_shared.json_io import render_json


def render_report_json(payload: object) -> str:
    return render_json(payload)


__all__ = ["render_report_json"]
