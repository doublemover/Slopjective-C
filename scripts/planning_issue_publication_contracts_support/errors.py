from __future__ import annotations


class PublicationError(RuntimeError):
    """Raised when the planning payload cannot be safely published."""
