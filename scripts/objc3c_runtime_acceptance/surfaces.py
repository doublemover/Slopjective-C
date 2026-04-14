"""Surface builder facade for runtime acceptance reports."""

from . import core as _core


def __getattr__(name: str):
    if name.startswith("build_") or name.endswith("_SURFACE_CONTRACT_ID"):
        return getattr(_core, name)
    raise AttributeError(name)


def exported_surface_names() -> list[str]:
    return sorted(
        name
        for name in dir(_core)
        if name.startswith("build_") or name.endswith("_SURFACE_CONTRACT_ID")
    )


__all__ = ["exported_surface_names"]
