"""Runnable runtime conformance workflow action facade."""

from __future__ import annotations

from .runtime_runnable_block_arc import (
    RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION,
    RUNNABLE_BLOCK_ARC_CONFORMANCE_PY,
    action_validate_block_arc_conformance,
)
from .runtime_runnable_concurrency import (
    RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION,
    RUNNABLE_CONCURRENCY_CONFORMANCE_PY,
    action_validate_concurrency_conformance,
)
from .runtime_runnable_error import (
    RUNNABLE_ERROR_CONFORMANCE_ACTION,
    RUNNABLE_ERROR_CONFORMANCE_PY,
    action_validate_error_conformance,
)
from .runtime_runnable_interop import (
    MODULE_INTEROP_CONTRACTS_ACTION,
    MODULE_INTEROP_CONTRACTS_PY,
    RUNNABLE_INTEROP_CONFORMANCE_ACTION,
    RUNNABLE_INTEROP_CONFORMANCE_PY,
    action_validate_module_interop_contracts,
    action_validate_interop_conformance,
)
from .runtime_runnable_metaprogramming import (
    RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION,
    RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY,
    action_validate_metaprogramming_conformance,
)
from .runtime_runnable_object_model import (
    RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION,
    RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY,
    action_validate_object_model_conformance,
)
from .runtime_runnable_release_candidate import (
    RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION,
    RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY,
    action_validate_release_candidate_conformance,
)
from .runtime_runnable_groups import RuntimeRunnableActionGroup
from .runtime_runnable_storage_reflection import (
    RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION,
    RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY,
    action_validate_storage_reflection_conformance,
)

RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS: tuple[
    RuntimeRunnableActionGroup, ...
] = (
    RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION,
    RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION,
    RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION,
    RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION,
    RUNNABLE_ERROR_CONFORMANCE_ACTION,
    MODULE_INTEROP_CONTRACTS_ACTION,
    RUNNABLE_INTEROP_CONFORMANCE_ACTION,
    RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION,
    RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION,
)

__all__ = [
    "RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS",
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_ACTION",
    "RUNNABLE_BLOCK_ARC_CONFORMANCE_PY",
    "RUNNABLE_CONCURRENCY_CONFORMANCE_ACTION",
    "RUNNABLE_CONCURRENCY_CONFORMANCE_PY",
    "RUNNABLE_ERROR_CONFORMANCE_ACTION",
    "RUNNABLE_ERROR_CONFORMANCE_PY",
    "MODULE_INTEROP_CONTRACTS_ACTION",
    "MODULE_INTEROP_CONTRACTS_PY",
    "RUNNABLE_INTEROP_CONFORMANCE_ACTION",
    "RUNNABLE_INTEROP_CONFORMANCE_PY",
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_ACTION",
    "RUNNABLE_METAPROGRAMMING_CONFORMANCE_PY",
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_ACTION",
    "RUNNABLE_OBJECT_MODEL_CONFORMANCE_PY",
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_ACTION",
    "RUNNABLE_RELEASE_CANDIDATE_CONFORMANCE_PY",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION",
    "RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_PY",
    "action_validate_block_arc_conformance",
    "action_validate_concurrency_conformance",
    "action_validate_error_conformance",
    "action_validate_module_interop_contracts",
    "action_validate_interop_conformance",
    "action_validate_metaprogramming_conformance",
    "action_validate_object_model_conformance",
    "action_validate_release_candidate_conformance",
    "action_validate_storage_reflection_conformance",
]
