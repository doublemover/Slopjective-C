"""Conformance-corpus workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from .application_surface_paths import (
    CONFORMANCE_CORPUS_INTEGRATION_PY,
    CONFORMANCE_MINIMA_PS1,
    RUNNABLE_CONFORMANCE_CORPUS_E2E_PY,
)


def action_validate_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)])


def action_check_conformance_minima(_: list[str]) -> int:
    return pwsh_file(CONFORMANCE_MINIMA_PS1)


def action_validate_runnable_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONFORMANCE_CORPUS_E2E_PY)])
