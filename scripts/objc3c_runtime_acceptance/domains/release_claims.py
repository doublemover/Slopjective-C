"""Release Claims runtime acceptance case facade."""

from .. import core as _core

_TOKENS = ('claim', 'release', 'strict_profile', 'scaffold', 'publication', 'deprecated', 'descaffolding',)


def _matches(name: str) -> bool:
    lowered = name.lower()
    return (name.startswith("check_") or name.startswith("build_")) and any(token in lowered for token in _TOKENS)


def __getattr__(name: str):
    if _matches(name):
        return getattr(_core, name)
    raise AttributeError(name)


def exported_case_names() -> list[str]:
    return sorted(name for name in dir(_core) if _matches(name))


__all__ = ["exported_case_names"]
