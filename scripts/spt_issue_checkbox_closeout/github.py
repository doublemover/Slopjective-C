from __future__ import annotations

import json
import re
import subprocess
from collections.abc import Callable
from typing import Any

from .models import IssueRef
from .paths import ROOT


def run_cmd(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str]) -> Any:
    proc = run_cmd(["gh", *args])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"gh {' '.join(args)} failed: {detail}")

    try:
        return json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"gh {' '.join(args)} returned invalid JSON: {exc}") from exc


def issue_refs_from_payload(
    payload: Any,
    task_id_pattern: re.Pattern[str],
) -> dict[str, IssueRef]:
    if not isinstance(payload, list):
        raise RuntimeError("unexpected gh issue list payload shape")

    mapping: dict[str, IssueRef] = {}
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        number = item.get("number")
        url = item.get("url")
        if not isinstance(title, str) or not isinstance(number, int) or not isinstance(url, str):
            continue
        match = task_id_pattern.match(title)
        if not match:
            continue
        task_id = match.group(1)
        mapping[task_id] = IssueRef(number=number, title=title, url=url)

    return mapping


def fetch_open_spt_issues(
    task_id_pattern: re.Pattern[str],
    *,
    gh_json_runner: Callable[[list[str]], Any] = run_gh_json,
) -> dict[str, IssueRef]:
    payload = gh_json_runner(
        ["issue", "list", "--state", "open", "--limit", "2000", "--json", "number,title,url"]
    )
    return issue_refs_from_payload(payload, task_id_pattern)


def close_issue(
    number: int,
    comment: str,
    *,
    command_runner: Callable[[list[str]], subprocess.CompletedProcess[str]] = run_cmd,
) -> None:
    proc = command_runner(["gh", "issue", "close", str(number), "--comment", comment])
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise RuntimeError(f"failed to close issue #{number}: {detail}")
