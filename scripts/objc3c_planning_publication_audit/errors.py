"""Errors raised by the planning-publication drift audit."""

from __future__ import annotations


class DriftAuditError(RuntimeError):
    """Raised when audit inputs are malformed."""
