"""Release-governance gate owner input parsing."""

from __future__ import annotations

from .models import ReleaseGateLookup


def parse_release_gate_lookup(gate_id: str) -> ReleaseGateLookup:
    return ReleaseGateLookup(gate_id=gate_id)


__all__ = ["parse_release_gate_lookup"]
