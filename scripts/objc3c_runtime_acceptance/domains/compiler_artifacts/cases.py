"""Compiler artifact acceptance case facade."""

from __future__ import annotations

from pathlib import Path

_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .cases.orchestration import (  # noqa: E402
    check_artifact_registry_key_isolation_case,
)
from .cases.orchestration import check_compile_backend_parity_case  # noqa: E402


check_artifact_registry_key_isolation_case.__module__ = __name__
check_compile_backend_parity_case.__module__ = __name__


__all__ = [
    "check_artifact_registry_key_isolation_case",
    "check_compile_backend_parity_case",
]
