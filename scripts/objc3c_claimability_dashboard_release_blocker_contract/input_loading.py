from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from .errors import ContractError
from .tooling import repo_rel


@dataclass(frozen=True)
class SourceTruthInputs:
    release_blocker_text: str
    dashboard_text: str
    runbook_text: str
    source_truth_paths: list[str]
    tmp_source_truth_paths: list[str]


def read_json(path: Path) -> dict[str, Any]:
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ContractError(f"missing JSON file `{repo_rel(path)}`") from exc
    except json.JSONDecodeError as exc:
        raise ContractError(f"invalid JSON at `{repo_rel(path)}`: {exc}") from exc
    if not isinstance(payload, dict):
        raise ContractError(f"JSON object expected at `{repo_rel(path)}`")
    return payload


def require_string(payload: dict[str, Any], key: str) -> str:
    value = payload.get(key)
    if not isinstance(value, str) or not value:
        raise ContractError(f"`{key}` must be a non-empty string")
    return value


def require_string_list(payload: dict[str, Any], key: str) -> list[str]:
    value = payload.get(key)
    if not isinstance(value, list) or not value:
        raise ContractError(f"`{key}` must be a non-empty list")
    out: list[str] = []
    for index, item in enumerate(value):
        if not isinstance(item, str) or not item:
            raise ContractError(f"`{key}[{index}]` must be a non-empty string")
        out.append(item)
    return out


def load_source_truth_inputs(
    *,
    policy_path: Path,
    release_blocker_script: Path,
    dashboard_script: Path,
    runbook: Path,
) -> SourceTruthInputs:
    release_blocker_text = release_blocker_script.read_text(encoding="utf-8")
    dashboard_text = dashboard_script.read_text(encoding="utf-8")
    runbook_text = runbook.read_text(encoding="utf-8")
    source_truth_paths = [
        repo_rel(policy_path),
        repo_rel(release_blocker_script),
        repo_rel(dashboard_script),
        repo_rel(runbook),
    ]
    return SourceTruthInputs(
        release_blocker_text=release_blocker_text,
        dashboard_text=dashboard_text,
        runbook_text=runbook_text,
        source_truth_paths=source_truth_paths,
        tmp_source_truth_paths=[
            path for path in source_truth_paths if path.startswith("tmp/")
        ],
    )
