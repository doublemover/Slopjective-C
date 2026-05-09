from __future__ import annotations

import importlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.model import (
    build_debug_payload,
    build_language_server_payload,
    build_navigation_payload,
    extract_symbols,
)
from objc3c_editor_tooling.paths import default_source_argument, paths_for_source, resolve_source


OWNER_MODULES = (
    "objc3c_editor_tooling.paths",
    "objc3c_editor_tooling.input_loading",
    "objc3c_editor_tooling.model",
    "objc3c_editor_tooling.validation",
    "objc3c_editor_tooling.rendering",
    "objc3c_editor_tooling.publication",
    "objc3c_editor_tooling.cli",
)


def test_editor_tooling_owner_modules_are_explicit() -> None:
    for module_name in OWNER_MODULES:
        assert importlib.import_module(module_name)


def test_editor_tooling_entrypoint_delegates_to_owner_modules() -> None:
    script_text = (ROOT / "scripts" / "build_objc3c_editor_tooling_surface.py").read_text(encoding="utf-8")
    assert "objc3c_editor_tooling.cli" in script_text
    assert "subprocess" not in script_text
    assert "hashlib" not in script_text
    assert "write_json_file" not in script_text
    assert "build_format_summary_for_source" not in script_text


def test_editor_tooling_paths_preserve_default_surface_layout() -> None:
    source = resolve_source(default_source_argument())
    paths = paths_for_source(source)

    assert source.display_path == "tests/tooling/fixtures/native/hello.objc3"
    assert source.slug == "hello-3bb3df22f2ea"
    assert paths.compile_summary.relative_to(ROOT).as_posix() == (
        "tmp/reports/developer-tooling/editor-surface/hello-3bb3df22f2ea/compile-summary.json"
    )
    assert paths.artifact_dir.relative_to(ROOT).as_posix() == (
        "tmp/artifacts/developer-tooling/editor-surface/hello-3bb3df22f2ea"
    )


def test_editor_tooling_model_keeps_manifest_backed_capabilities_and_fail_closed_debug() -> None:
    manifest = {
        "functions": [{"name": "main", "line": 10, "column": 4}],
        "interfaces": [{"name": "Widget", "line": 2, "column": 11}],
        "implementations": [{"class_name": "Widget", "line": 20, "column": 16}],
        "categories": [{"class_name": "Widget", "category_name": "Debug", "line": 32, "column": 12}],
    }

    symbols = extract_symbols(manifest)
    language_server = build_language_server_payload(
        {"observability": {"status_name": "OK"}},
        "tmp/artifacts/module.manifest.json",
        symbols,
    )
    navigation = build_navigation_payload("tests/tooling/fixtures/native/hello.objc3", "tmp/artifacts/module.manifest.json", symbols)
    debug = build_debug_payload({"runtime_inspector": {"contract_id": "runtime.inspect.v1"}}, None, symbols)

    assert [symbol["name"] for symbol in symbols] == ["Widget", "main", "Widget", "Widget(Debug)"]
    assert language_server["summary_status_name"] == "OK"
    assert language_server["supported_capability_ids"] == [
        "publishDiagnostics",
        "documentSymbol",
        "workspaceSymbol",
        "definition",
    ]
    assert navigation["symbol_count"] == 4
    assert debug["supported"] is True
    assert debug["statement_level_stepping"] is False
    assert debug["support_class"] == "declaration-breakpoint-preview"
