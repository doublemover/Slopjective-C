"""Filtering helpers for classified validation inventory entries."""

from __future__ import annotations

from typing import Any


def retained_static_guards(entries: list[dict[str, Any]]) -> list[dict[str, Any]]:
    return [entry for entry in entries if entry["surface_kind"] == "retained_static_guard"]


def unreferenced_check_surfaces(entries: list[dict[str, Any]]) -> list[str]:
    return [entry["path"] for entry in entries if not any(entry["references"].values())]
