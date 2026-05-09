"""Input parsing and source resolution for playground workflow actions."""

from __future__ import annotations

import hashlib
import re
from dataclasses import dataclass
from pathlib import Path

from ..environment import ROOT
from .developer_tooling_paths import DEFAULT_PLAYGROUND_SOURCE

MANAGED_PLAYGROUND_FLAGS = (
    "--out-dir",
    "--emit-prefix",
    "--summary-out",
    "--dump-summary-json",
    "--dump-observability-json",
    "--dump-playground-repro-json",
    "--dump-runtime-inspector-json",
    "--dump-stage-trace-json",
)


@dataclass(frozen=True)
class PlaygroundInvocation:
    source_path: Path
    source_display: str
    passthrough: list[str]
    workspace_id: str


def parse_playground_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_PLAYGROUND_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in MANAGED_PLAYGROUND_FLAGS:
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public playground action")
    return source_text, passthrough


def resolve_source_path(source_text: str) -> tuple[Path, str]:
    candidate = Path(source_text)
    resolved = candidate if candidate.is_absolute() else ROOT / candidate
    resolved = resolved.resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"playground source not found: {source_text}")
    try:
        display = resolved.relative_to(ROOT).as_posix()
    except ValueError:
        display = resolved.as_posix()
    return resolved, display


def slugify_playground_workspace(source_display: str) -> str:
    safe_stem = re.sub(r"[^a-z0-9]+", "-", Path(source_display).stem.lower()).strip("-")
    if not safe_stem:
        safe_stem = "source"
    digest = hashlib.sha256(source_display.encode("utf-8")).hexdigest()[:12]
    return f"{safe_stem}-{digest}"


def resolve_playground_invocation(rest: list[str]) -> PlaygroundInvocation:
    source_text, passthrough = parse_playground_invocation(rest)
    source_path, source_display = resolve_source_path(source_text)
    return PlaygroundInvocation(
        source_path=source_path,
        source_display=source_display,
        passthrough=passthrough,
        workspace_id=slugify_playground_workspace(source_display),
    )
