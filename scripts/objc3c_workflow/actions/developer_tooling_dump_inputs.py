"""Input policy for developer-tooling frontend dump actions."""

from __future__ import annotations

from ..environment import ROOT
from .developer_tooling_dump_contracts import MANAGED_DEVELOPER_TOOLING_DUMP_FLAGS
from .developer_tooling_paths import DEFAULT_DEVELOPER_TOOLING_SOURCE


def parse_developer_tooling_invocation(rest: list[str]) -> tuple[str, list[str]]:
    if rest and not rest[0].startswith("--"):
        source_text = rest[0]
        passthrough = rest[1:]
    else:
        source_text = str(DEFAULT_DEVELOPER_TOOLING_SOURCE.relative_to(ROOT).as_posix())
        passthrough = rest
    for forbidden in MANAGED_DEVELOPER_TOOLING_DUMP_FLAGS:
        if forbidden in passthrough:
            raise ValueError(f"{forbidden} is managed by the public developer-tooling action")
    return source_text, passthrough
