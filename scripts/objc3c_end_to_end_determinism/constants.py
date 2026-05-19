"""Stable constants for the end-to-end determinism checker."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MODE = "objc3c-end-to-end-determinism-v1"
DEFAULT_REPLAY_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-end-to-end-determinism"
MAX_STDIO_PREVIEW_CHARS = 2000

__all__ = ["DEFAULT_REPLAY_ROOT", "MAX_STDIO_PREVIEW_CHARS", "MODE", "ROOT"]
