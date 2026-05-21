from __future__ import annotations

import hashlib
from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import ROOT, display_path


FRONTEND_RUNNER = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
DEFAULT_SOURCE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "editor-surface"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "developer-tooling" / "editor-surface"


@dataclass(frozen=True)
class EditorToolingSource:
    path: Path
    display_path: str
    slug: str


@dataclass(frozen=True)
class EditorToolingPaths:
    source: EditorToolingSource
    report_dir: Path
    artifact_dir: Path
    compile_summary: Path
    editor_surface: Path
    language_server_capabilities: Path
    navigation_index: Path
    workspace_index: Path
    artifact_inspector: Path
    formatter_output: Path
    formatted_source: Path
    debug_map: Path
    frontend_runner: Path = FRONTEND_RUNNER


def default_source_argument() -> str:
    return DEFAULT_SOURCE.relative_to(ROOT).as_posix()


def resolve_source(source_text: str) -> EditorToolingSource:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else (ROOT / candidate)
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"source not found: {source_text}")
    source_display = display_path(resolved)
    return EditorToolingSource(path=resolved, display_path=source_display, slug=slugify(source_display))


def slugify(display_path_text: str) -> str:
    digest = hashlib.sha256(display_path_text.encode("utf-8")).hexdigest()[:12]
    stem = Path(display_path_text).stem.lower()
    safe = "".join(ch if ch.isalnum() else "-" for ch in stem).strip("-")
    if not safe:
        safe = "source"
    return f"{safe}-{digest}"


def paths_for_source(source: EditorToolingSource) -> EditorToolingPaths:
    report_dir = REPORT_ROOT / source.slug
    artifact_dir = ARTIFACT_ROOT / source.slug
    return EditorToolingPaths(
        source=source,
        report_dir=report_dir,
        artifact_dir=artifact_dir,
        compile_summary=report_dir / "compile-summary.json",
        editor_surface=report_dir / "editor-surface.json",
        language_server_capabilities=report_dir / "language-server-capabilities.json",
        navigation_index=report_dir / "navigation-index.json",
        workspace_index=report_dir / "workspace-index.json",
        artifact_inspector=report_dir / "artifact-inspector.json",
        formatter_output=report_dir / "formatter-output.json",
        formatted_source=report_dir / "formatted-source.objc3",
        debug_map=report_dir / "debug-map.json",
    )
