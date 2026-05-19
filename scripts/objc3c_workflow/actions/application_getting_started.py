"""Getting-started tutorial workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from .application_surface_paths import GETTING_STARTED_INTEGRATION_PY


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])


__all__ = ["action_validate_getting_started"]
