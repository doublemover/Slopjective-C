"""Compiler artifact runtime acceptance domain facade."""

from __future__ import annotations

from .cases import check_artifact_registry_key_isolation_case
from .cases import check_compile_backend_parity_case


check_artifact_registry_key_isolation_case.__module__ = __name__
check_compile_backend_parity_case.__module__ = __name__


__all__ = [
    "check_artifact_registry_key_isolation_case",
    "check_compile_backend_parity_case",
]
