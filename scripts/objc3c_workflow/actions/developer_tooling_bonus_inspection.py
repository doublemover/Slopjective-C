"""Bonus-tool integration inspection workflow action."""

from __future__ import annotations

import json
import sys

from ..environment import ROOT
from .developer_tooling_bonus_artifacts import (
    bonus_tool_integration_report_path,
    ensure_bonus_artifact_source as _ensure_bonus_artifact_source,
)
from .developer_tooling_bonus_inputs import (
    load_bonus_surface_inputs as _load_bonus_surface_inputs,
)
from .developer_tooling_bonus_payload import bonus_tool_payload as _bonus_tool_payload


def action_inspect_bonus_tool_integration(_: list[str]) -> int:
    try:
        rc = _ensure_bonus_artifact_source()
        if rc != 0:
            return rc
        payload = _bonus_tool_payload(_load_bonus_surface_inputs())
    except (FileNotFoundError, ValueError) as exc:
        print(str(exc), file=sys.stderr)
        return 1

    dump_path = bonus_tool_integration_report_path()
    dump_path.parent.mkdir(parents=True, exist_ok=True)
    dump_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {dump_path.relative_to(ROOT).as_posix()}")
    print(f"dump_path: {dump_path.relative_to(ROOT).as_posix()}")
    return 0
