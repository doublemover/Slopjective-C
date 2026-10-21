"""Block/ARC storage automation artifact facade."""

from __future__ import annotations

import inspect
from pathlib import Path


_HELPER_MODULE_DIR = Path(__file__).with_suffix("")
__path__ = [str(_HELPER_MODULE_DIR)]

from .block_arc_automation_artifacts.models import (  # noqa: E402
    BlockArcAutomationArtifacts,
    ManifestSurface,
    NegativeBatch,
)
from .block_arc_automation_artifacts.orchestration import (  # noqa: E402
    load_block_arc_automation_artifacts,
)


__all__ = [
    "BlockArcAutomationArtifacts",
    "ManifestSurface",
    "NegativeBatch",
    "load_block_arc_automation_artifacts",
]

for _exported_name in __all__:
    _exported = globals()[_exported_name]
    if inspect.isclass(_exported) or inspect.isfunction(_exported):
        try:
            _exported.__module__ = __name__
        except (AttributeError, TypeError):
            # Some Python versions report immutable typing aliases as classes.
            # Their originating module metadata is not ours to rewrite.
            pass

del _exported
del _exported_name
