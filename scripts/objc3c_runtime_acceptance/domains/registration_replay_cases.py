"""Registration runtime acceptance reset/replay cases facade."""

from __future__ import annotations

from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .registration_replay_cases.catalog import (  # noqa: E402
    EXPORTED_CASE_NAMES as _EXPORTED_CASE_NAMES,
)
from .registration_replay_cases.orchestration import (  # noqa: E402
    check_multi_image_registration_reset_replay_case,
)


def exported_case_names() -> list[str]:
    return sorted(_EXPORTED_CASE_NAMES)


check_multi_image_registration_reset_replay_case.__module__ = __name__


__all__ = [*_EXPORTED_CASE_NAMES, "exported_case_names"]
