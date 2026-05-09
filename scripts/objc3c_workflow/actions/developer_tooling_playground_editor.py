"""Editor-surface loading and drill commands for playground workspaces."""

from __future__ import annotations

from objc3c_tooling.json_io import load_json_object as load_json

from ..commands import extract_output_line
from ..environment import ROOT, WORKFLOW_COMMAND_TEXT


def load_editor_surface(
    editor_result_stdout: str,
) -> tuple[dict[str, object], dict[str, str]]:
    editor_surface_path_text = extract_output_line(editor_result_stdout, "dump_path:")
    published_paths = {
        "editor_surface_path": editor_surface_path_text,
        "language_server_capabilities_path": extract_output_line(
            editor_result_stdout,
            "capabilities_path:",
        ),
        "navigation_path": extract_output_line(editor_result_stdout, "navigation_path:"),
        "formatter_path": extract_output_line(editor_result_stdout, "formatter_path:"),
        "debug_path": extract_output_line(editor_result_stdout, "debug_path:"),
    }
    if not editor_surface_path_text:
        raise RuntimeError(
            "playground-workspace: editor tooling action did not publish dump_path"
        )
    editor_surface_path = ROOT / editor_surface_path_text
    if not editor_surface_path.is_file():
        raise FileNotFoundError(
            f"playground-workspace: missing editor tooling surface {editor_surface_path_text}"
        )
    return load_json(editor_surface_path), published_paths


def workspace_drill_commands(
    source_display: str,
    debug_payload: dict[str, object],
) -> dict[str, str]:
    return {
        "inspect_editor_tooling": (
            f"{WORKFLOW_COMMAND_TEXT} inspect-editor-tooling {source_display}"
        ),
        "format_preview": f"{WORKFLOW_COMMAND_TEXT} format-objc3c {source_display}",
        "object_symbol_inventory": str(
            debug_payload.get("object_symbol_inventory_command", "")
        ),
    }
