"""Deterministic runtime debug trace support for objc3c developer tooling."""

from __future__ import annotations

import sys
from pathlib import Path

SCRIPT_ROOT = Path(__file__).resolve().parents[1]
if str(SCRIPT_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPT_ROOT))

from .contracts import (
    RUNTIME_DEBUG_TRACE_CONTRACT_ID,
    RUNTIME_DEBUG_TRACE_SCHEMA_ID,
    RUNTIME_DEBUG_TRACE_SCHEMA_PATH,
    RUNTIME_DEBUG_TRACE_SUMMARY_PATH,
)
from .payload import build_runtime_debug_trace_payload
from .validation import validate_runtime_debug_trace_payload

__all__ = [
    "RUNTIME_DEBUG_TRACE_CONTRACT_ID",
    "RUNTIME_DEBUG_TRACE_SCHEMA_ID",
    "RUNTIME_DEBUG_TRACE_SCHEMA_PATH",
    "RUNTIME_DEBUG_TRACE_SUMMARY_PATH",
    "build_runtime_debug_trace_payload",
    "validate_runtime_debug_trace_payload",
]
