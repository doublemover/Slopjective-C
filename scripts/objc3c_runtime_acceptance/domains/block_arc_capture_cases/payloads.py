"""Block/ARC capture legality payload shaping."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from ...fixture_compilation import (
    compile_fixture_outputs,
    compile_negative_diagnostic_batch,
)
from .catalog import (
    CASE_ID,
    COPY_DISPOSE_SURFACE_KEY,
    ESCAPE_SURFACE_KEY,
    NEGATIVE_EXPECTATIONS,
    SEMANTIC_SURFACE_PATH,
)
from .data import (
    CaptureFixtureFacts,
    CaptureFixtureSpec,
    DiagnosticBatch,
    ManifestSurface,
)


def capture_fixture_facts(
    spec: CaptureFixtureSpec,
    case_dir: Path,
) -> CaptureFixtureFacts:
    _, _, manifest_path = compile_fixture_outputs(
        spec.fixture,
        case_dir / spec.output_dir_name,
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    semantic_surface = _manifest_surface_at_path(manifest, SEMANTIC_SURFACE_PATH)
    return CaptureFixtureFacts(
        key=spec.key,
        escape_surface=semantic_surface.get(ESCAPE_SURFACE_KEY, {}),
        copy_dispose_surface=semantic_surface.get(COPY_DISPOSE_SURFACE_KEY, {}),
    )


def compile_capture_negative_batch(case_dir: Path) -> DiagnosticBatch:
    return compile_negative_diagnostic_batch(
        case_id=CASE_ID,
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=list(NEGATIVE_EXPECTATIONS),
    )


def build_capture_legality_summary(
    facts_by_key: dict[str, CaptureFixtureFacts],
    negative_batch: DiagnosticBatch,
) -> dict[str, Any]:
    bad_call_negative = negative_batch["results"][0]
    missing_capture_negative = negative_batch["results"][1]
    argument = facts_by_key["argument"]
    return_value = facts_by_key["return"]
    byref = facts_by_key["byref"]
    owned = facts_by_key["owned"]

    return {
        "argument_escape_to_heap_sites": argument.escape_surface.get(
            "escape_to_heap_sites"
        ),
        "return_escape_to_heap_sites": return_value.escape_surface.get(
            "escape_to_heap_sites"
        ),
        "bad_call_diagnostic_count": bad_call_negative["diagnostic_count"],
        "missing_capture_diagnostic_count": missing_capture_negative[
            "diagnostic_count"
        ],
        "byref_escape_to_heap_sites": byref.escape_surface.get(
            "escape_to_heap_sites"
        ),
        "byref_copy_helper_required_sites": byref.copy_dispose_surface.get(
            "copy_helper_required_sites"
        ),
        "owned_escape_to_heap_sites": owned.escape_surface.get(
            "escape_to_heap_sites"
        ),
        "owned_copy_helper_required_sites": owned.copy_dispose_surface.get(
            "copy_helper_required_sites"
        ),
        "negative_diagnostics_batch": negative_batch,
    }


def _manifest_surface_at_path(
    manifest: dict[str, Any],
    path: tuple[str, ...],
) -> ManifestSurface:
    surface: dict[str, Any] = manifest
    for key in path:
        surface = surface.get(key, {})
    return surface


__all__ = [
    "build_capture_legality_summary",
    "capture_fixture_facts",
    "compile_capture_negative_batch",
]
