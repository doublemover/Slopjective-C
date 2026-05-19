"""Path layout for playground workspace materialization."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from .developer_tooling_paths import PLAYGROUND_ARTIFACT_ROOT, PLAYGROUND_REPORT_ROOT
from .developer_tooling_playground_inputs import PlaygroundInvocation


@dataclass(frozen=True)
class PlaygroundWorkspacePaths:
    workspace_root: Path
    artifact_root: Path
    report_root: Path
    workspace_manifest_path: Path
    summary_path: Path
    dump_path: Path


def playground_workspace_paths(
    invocation: PlaygroundInvocation,
) -> PlaygroundWorkspacePaths:
    workspace_root = PLAYGROUND_ARTIFACT_ROOT / invocation.workspace_id
    artifact_root = workspace_root / "build"
    report_root = PLAYGROUND_REPORT_ROOT / invocation.workspace_id
    return PlaygroundWorkspacePaths(
        workspace_root=workspace_root,
        artifact_root=artifact_root,
        report_root=report_root,
        workspace_manifest_path=workspace_root / "workspace.json",
        summary_path=report_root / "compile-summary.json",
        dump_path=report_root / "playground-repro.json",
    )
