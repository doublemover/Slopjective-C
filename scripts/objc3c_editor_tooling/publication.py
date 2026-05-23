from __future__ import annotations

from dataclasses import dataclass

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import display_path

from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.model import EditorToolingModel
from objc3c_editor_tooling.paths import EditorToolingPaths
from objc3c_editor_tooling.rendering import render_editor_surface


@dataclass(frozen=True)
class PublishedEditorToolingSurface:
    summary_path: str
    dump_path: str
    capabilities_path: str
    navigation_path: str
    workspace_index_path: str
    source_graph_path: str
    artifact_inspector_path: str
    formatter_path: str
    debug_path: str


def publish_editor_tooling_surface(
    *,
    paths: EditorToolingPaths,
    inputs: EditorToolingInputs,
    model: EditorToolingModel,
) -> PublishedEditorToolingSurface:
    paths.report_dir.mkdir(parents=True, exist_ok=True)
    editor_surface = render_editor_surface(paths=paths, inputs=inputs, model=model)
    paths.formatted_source.write_text(model.formatted_source_text, encoding="utf-8")
    write_json_file(paths.editor_surface, editor_surface)
    write_json_file(paths.language_server_capabilities, model.language_server)
    write_json_file(paths.navigation_index, model.navigation)
    write_json_file(paths.workspace_index, model.workspace_index)
    write_json_file(paths.source_graph, model.source_graph)
    write_json_file(paths.artifact_inspector, model.artifact_inspector)
    write_json_file(paths.formatter_output, model.formatter)
    write_json_file(paths.debug_map, model.debug)
    return PublishedEditorToolingSurface(
        summary_path=display_path(paths.compile_summary),
        dump_path=display_path(paths.editor_surface),
        capabilities_path=display_path(paths.language_server_capabilities),
        navigation_path=display_path(paths.navigation_index),
        workspace_index_path=display_path(paths.workspace_index),
        source_graph_path=display_path(paths.source_graph),
        artifact_inspector_path=display_path(paths.artifact_inspector),
        formatter_path=display_path(paths.formatter_output),
        debug_path=display_path(paths.debug_map),
    )
