"""Native tool resolution for mixed-module differential cases."""

from __future__ import annotations

from dataclasses import dataclass

from .runtime_acceptance import find_clangxx


@dataclass
class NativeToolResolver:
    _clangxx: str | None = None

    def clangxx(self) -> str:
        if self._clangxx is None:
            self._clangxx = find_clangxx()
        return self._clangxx


__all__ = ["NativeToolResolver"]
