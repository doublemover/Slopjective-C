"""Runnable runtime end-to-end workflow action facade."""

from __future__ import annotations

from .runtime_runnable_block_arc import (
    RUNNABLE_BLOCK_ARC_E2E_ACTION,
    RUNNABLE_BLOCK_ARC_E2E_PY,
    action_validate_runnable_block_arc,
)
from .runtime_runnable_bootstrap import (
    RUNNABLE_BOOTSTRAP_E2E_ACTION,
    RUNNABLE_BOOTSTRAP_E2E_PY,
    action_validate_runnable_bootstrap,
)
from .runtime_runnable_concurrency import (
    RUNNABLE_CONCURRENCY_E2E_ACTION,
    RUNNABLE_CONCURRENCY_E2E_PY,
    action_validate_runnable_concurrency,
)
from .runtime_runnable_error import (
    RUNNABLE_ERROR_E2E_ACTION,
    RUNNABLE_ERROR_E2E_PY,
    action_validate_runnable_error,
)
from .runtime_runnable_interop import (
    RUNNABLE_INTEROP_E2E_ACTION,
    RUNNABLE_INTEROP_E2E_PY,
    action_validate_runnable_interop,
)
from .runtime_runnable_metaprogramming import (
    RUNNABLE_METAPROGRAMMING_E2E_ACTION,
    RUNNABLE_METAPROGRAMMING_E2E_PY,
    action_validate_runnable_metaprogramming,
)
from .runtime_runnable_object_model import (
    RUNNABLE_OBJECT_MODEL_E2E_ACTION,
    RUNNABLE_OBJECT_MODEL_E2E_PY,
    action_validate_runnable_object_model,
)
from .runtime_runnable_release_candidate import (
    RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION,
    RUNNABLE_RELEASE_CANDIDATE_E2E_PY,
    action_validate_runnable_release_candidate,
)
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_runnable_storage_reflection import (
    RUNNABLE_STORAGE_REFLECTION_E2E_ACTION,
    RUNNABLE_STORAGE_REFLECTION_E2E_PY,
    action_validate_runnable_storage_reflection,
)

RUNTIME_RUNNABLE_E2E_ACTION_GROUPS: tuple[RuntimeRunnableActionGroup, ...] = (
    RUNNABLE_BOOTSTRAP_E2E_ACTION,
    RUNNABLE_BLOCK_ARC_E2E_ACTION,
    RUNNABLE_CONCURRENCY_E2E_ACTION,
    RUNNABLE_OBJECT_MODEL_E2E_ACTION,
    RUNNABLE_STORAGE_REFLECTION_E2E_ACTION,
    RUNNABLE_ERROR_E2E_ACTION,
    RUNNABLE_INTEROP_E2E_ACTION,
    RUNNABLE_METAPROGRAMMING_E2E_ACTION,
    RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION,
)

__all__ = [
    "RUNTIME_RUNNABLE_E2E_ACTION_GROUPS",
    "RUNNABLE_BLOCK_ARC_E2E_ACTION",
    "RUNNABLE_BLOCK_ARC_E2E_PY",
    "RUNNABLE_BOOTSTRAP_E2E_ACTION",
    "RUNNABLE_BOOTSTRAP_E2E_PY",
    "RUNNABLE_CONCURRENCY_E2E_ACTION",
    "RUNNABLE_CONCURRENCY_E2E_PY",
    "RUNNABLE_ERROR_E2E_ACTION",
    "RUNNABLE_ERROR_E2E_PY",
    "RUNNABLE_INTEROP_E2E_ACTION",
    "RUNNABLE_INTEROP_E2E_PY",
    "RUNNABLE_METAPROGRAMMING_E2E_ACTION",
    "RUNNABLE_METAPROGRAMMING_E2E_PY",
    "RUNNABLE_OBJECT_MODEL_E2E_ACTION",
    "RUNNABLE_OBJECT_MODEL_E2E_PY",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_ACTION",
    "RUNNABLE_RELEASE_CANDIDATE_E2E_PY",
    "RUNNABLE_STORAGE_REFLECTION_E2E_ACTION",
    "RUNNABLE_STORAGE_REFLECTION_E2E_PY",
    "action_validate_runnable_block_arc",
    "action_validate_runnable_bootstrap",
    "action_validate_runnable_concurrency",
    "action_validate_runnable_error",
    "action_validate_runnable_interop",
    "action_validate_runnable_metaprogramming",
    "action_validate_runnable_object_model",
    "action_validate_runnable_release_candidate",
    "action_validate_runnable_storage_reflection",
]
