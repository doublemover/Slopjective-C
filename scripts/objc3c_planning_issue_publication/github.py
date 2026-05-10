"""GitHub CLI execution helpers for planning issue publication."""

from __future__ import annotations

import json
import subprocess
import time
from typing import Any, Sequence

from .constants import ROOT
from .contracts import PublicationError


def run_gh(
    args: Sequence[str],
    input_payload: Any | None = None,
    retries: int = 3,
) -> subprocess.CompletedProcess[str]:
    command = ["gh", *args]
    stdin = None if input_payload is None else json.dumps(input_payload)
    for attempt in range(1, retries + 1):
        result = subprocess.run(
            command,
            cwd=ROOT,
            input=stdin,
            text=True,
            capture_output=True,
            check=False,
        )
        if result.returncode == 0:
            return result
        stderr = result.stderr.lower()
        retryable = any(
            token in stderr
            for token in (
                "secondary rate limit",
                "abuse detection",
                "502",
                "503",
                "504",
                "timeout",
            )
        )
        if not retryable or attempt == retries:
            detail = result.stderr.strip() or result.stdout.strip()
            raise PublicationError(
                f"GitHub command failed ({' '.join(command)}): {detail}"
            )
        time.sleep(60 if "secondary rate limit" in stderr or "abuse detection" in stderr else 5 * attempt)
    raise AssertionError("unreachable")


def gh_json(args: Sequence[str], input_payload: Any | None = None) -> Any:
    result = run_gh(args, input_payload=input_payload)
    if not result.stdout.strip():
        return None
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        command = " ".join(["gh", *args])
        raise PublicationError(f"GitHub command did not return JSON: {command}") from exc


def fetch_milestones(repo: str) -> dict[str, dict[str, Any]]:
    records: list[dict[str, Any]] = []
    page = 1
    while True:
        payload = gh_json(["api", f"repos/{repo}/milestones?state=all&per_page=100&page={page}"])
        if not isinstance(payload, list):
            raise PublicationError("GitHub milestones response must be an array")
        records.extend(payload)
        if len(payload) < 100:
            break
        page += 1
    return {record["title"]: record for record in records}
