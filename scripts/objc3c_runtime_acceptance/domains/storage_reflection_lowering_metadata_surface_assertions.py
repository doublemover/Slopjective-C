"""Storage/reflection lowering metadata assertion facade."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_arc_assertions import (
    assert_arc_accessor_lowering_metadata_surface,
)
from objc3c_runtime_acceptance.domains.storage_reflection_lowering_metadata_synthesized_assertions import (
    assert_synthesized_accessor_lowering_metadata_surface,
)


__all__ = [
    "assert_arc_accessor_lowering_metadata_surface",
    "assert_synthesized_accessor_lowering_metadata_surface",
]
