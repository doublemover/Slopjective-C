from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import display_path

from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.model import EditorToolingModel, diagnostics_summary
from objc3c_editor_tooling.paths import EditorToolingPaths


def render_editor_surface(
    *,
    paths: EditorToolingPaths,
    inputs: EditorToolingInputs,
    model: EditorToolingModel,
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.editor.surface.v1",
        "source_path": paths.source.display_path,
        "summary_path": display_path(paths.compile_summary),
        "manifest_path": inputs.manifest_path_text,
        "diagnostics_path": inputs.diagnostics_path_text,
        "diagnostics": diagnostics_summary(inputs),
        "language_server": model.language_server,
        "navigation": model.navigation,
        "source_index": model.source_index,
        "source_graph": model.source_graph,
        "workspace_index": model.workspace_index,
        "artifact_inspector": model.artifact_inspector,
        "formatter": model.formatter,
        "debug": model.debug,
    }
