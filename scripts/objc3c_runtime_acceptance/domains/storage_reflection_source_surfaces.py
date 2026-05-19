"""Storage/reflection runtime acceptance source-surface facade."""

from __future__ import annotations

from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .storage_reflection_source_surfaces.public_api import (  # noqa: E402
    build_runtime_property_atomicity_synthesis_reflection_source_surface,
    build_runtime_property_ivar_storage_accessor_source_surface,
)


__all__ = [
    "build_runtime_property_ivar_storage_accessor_source_surface",
    "build_runtime_property_atomicity_synthesis_reflection_source_surface",
]

for _exported_name in __all__:
    globals()[_exported_name].__module__ = __name__

del _exported_name
