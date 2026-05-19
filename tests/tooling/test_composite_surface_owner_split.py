from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.composite_surface_acceptance_keys import (
    ACCEPTANCE_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_block_storage_keys import (
    BLOCK_STORAGE_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_claim_keys import (
    CLAIM_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_error_keys import (
    ERROR_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_interop_keys import (
    INTEROP_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_metaprogramming_keys import (
    METAPROGRAMMING_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surface_reflection_keys import (
    REFLECTION_COMPOSITE_SURFACE_KEYS,
)
from scripts.objc3c_workflow.composite_surfaces import COMPOSITE_SURFACE_KEYS

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "composite_surface_error_keys",
    "composite_surface_acceptance_keys",
    "composite_surface_block_storage_keys",
    "composite_surface_claim_keys",
    "composite_surface_metaprogramming_keys",
    "composite_surface_interop_keys",
    "composite_surface_reflection_keys",
)


def test_composite_surfaces_is_owner_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "composite_surfaces.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "runtime_error_execution_cleanup_source_surface" not in facade_text
    assert "runtime_reflection_visibility_coherence_diagnostics_surface" not in (
        facade_text
    )


def test_composite_surface_keys_preserve_owner_order() -> None:
    expected = (
        *ERROR_COMPOSITE_SURFACE_KEYS,
        *ACCEPTANCE_COMPOSITE_SURFACE_KEYS,
        *BLOCK_STORAGE_COMPOSITE_SURFACE_KEYS,
        *CLAIM_COMPOSITE_SURFACE_KEYS,
        *METAPROGRAMMING_COMPOSITE_SURFACE_KEYS,
        *INTEROP_COMPOSITE_SURFACE_KEYS,
        *REFLECTION_COMPOSITE_SURFACE_KEYS,
    )

    assert COMPOSITE_SURFACE_KEYS == expected
    assert len(COMPOSITE_SURFACE_KEYS) == len(set(COMPOSITE_SURFACE_KEYS))
    assert COMPOSITE_SURFACE_KEYS[0] == "runtime_state_publication_surface"
    assert COMPOSITE_SURFACE_KEYS[-1] == (
        "runtime_reflection_visibility_coherence_diagnostics_surface"
    )
