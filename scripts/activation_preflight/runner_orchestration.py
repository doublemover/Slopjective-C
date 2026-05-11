"""Activation preflight orchestration facade."""

from __future__ import annotations

from pathlib import Path

# Keep the historical module import while hosting focused helpers next to it.
__path__ = [str(Path(__file__).with_suffix(""))]

from scripts.activation_preflight.runner_orchestration.flow import run_preflight
from scripts.activation_preflight.runner_orchestration.types import CommandRunner


__all__ = ["CommandRunner", "run_preflight"]
