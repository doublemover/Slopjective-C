"""Surface contract assertion facade for storage/reflection layout lowering."""

from __future__ import annotations

from .assertions import assert_accessor_layout_surface
from .assertions import assert_executable_synthesized_accessor_surface
from .assertions import assert_ivar_layout_surface
from .assertions import assert_property_source_surface_links
from .data import ManifestSurface


assert_accessor_layout_surface.__module__ = __name__
assert_executable_synthesized_accessor_surface.__module__ = __name__
assert_ivar_layout_surface.__module__ = __name__
assert_property_source_surface_links.__module__ = __name__


__all__ = [
    "ManifestSurface",
    "assert_accessor_layout_surface",
    "assert_executable_synthesized_accessor_surface",
    "assert_ivar_layout_surface",
    "assert_property_source_surface_links",
]
