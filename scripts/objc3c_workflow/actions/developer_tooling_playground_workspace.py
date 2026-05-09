"""Workspace materialization for public playground workflow actions."""

from __future__ import annotations

import json
import sys

from .developer_tooling_playground_editor import (
    load_editor_surface as _load_editor_surface,
)
from .developer_tooling_playground_inputs import (
    PlaygroundInvocation,
    resolve_playground_invocation,
)
from .developer_tooling_playground_manifest import (
    build_playground_workspace_payload as _workspace_payload,
)
from .developer_tooling_playground_paths import (
    PlaygroundWorkspacePaths,
    playground_workspace_paths,
)
from .developer_tooling_playground_runner import (
    ensure_frontend_runner_ready,
    playground_subprocess_env,
    run_editor_tooling,
    run_frontend_playground,
)
from ..environment import ROOT


def _playground_workspace_paths(
    invocation: PlaygroundInvocation,
) -> PlaygroundWorkspacePaths:
    return playground_workspace_paths(invocation)


def run_playground_workspace(rest: list[str], *, emit_payload: bool) -> int:
    try:
        invocation = resolve_playground_invocation(rest)
    except (ValueError, FileNotFoundError) as exc:
        print(str(exc), file=sys.stderr)
        return 2

    rc = ensure_frontend_runner_ready()
    if rc != 0:
        return rc

    paths = _playground_workspace_paths(invocation)
    paths.artifact_root.mkdir(parents=True, exist_ok=True)
    paths.report_root.mkdir(parents=True, exist_ok=True)

    env = playground_subprocess_env()
    result = run_frontend_playground(
        invocation.source_display,
        paths.artifact_root.relative_to(ROOT).as_posix(),
        paths.summary_path.relative_to(ROOT).as_posix(),
        invocation.passthrough,
        env=env,
    )
    if result.stderr:
        sys.stderr.write(result.stderr)
    if result.returncode != 0:
        if result.stdout:
            sys.stdout.write(result.stdout)
        return result.returncode

    try:
        playground_payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        print(
            f"playground-workspace: invalid JSON from frontend runner: {exc}",
            file=sys.stderr,
        )
        return 1
    paths.dump_path.write_text(
        json.dumps(playground_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    editor_result = run_editor_tooling(invocation.source_display, env=env)
    if editor_result.stdout:
        sys.stdout.write(editor_result.stdout)
    if editor_result.stderr:
        sys.stderr.write(editor_result.stderr)
    if editor_result.returncode != 0:
        return editor_result.returncode

    try:
        editor_surface_payload, published_paths = _load_editor_surface(
            editor_result.stdout
        )
    except (FileNotFoundError, RuntimeError) as exc:
        print(str(exc), file=sys.stderr)
        return 1

    workspace_payload = _workspace_payload(
        invocation,
        paths=paths,
        playground_payload=playground_payload,
        editor_surface_payload=editor_surface_payload,
        published_paths=published_paths,
    )
    paths.workspace_manifest_path.write_text(
        json.dumps(workspace_payload, indent=2) + "\n",
        encoding="utf-8",
    )

    if emit_payload:
        sys.stdout.write(json.dumps(playground_payload, indent=2) + "\n")
    print(f"workspace_path: {paths.workspace_manifest_path.relative_to(ROOT).as_posix()}")
    print(f"summary_path: {paths.summary_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {paths.dump_path.relative_to(ROOT).as_posix()}")
    print(f"artifact_root: {paths.artifact_root.relative_to(ROOT).as_posix()}")
    return 0
