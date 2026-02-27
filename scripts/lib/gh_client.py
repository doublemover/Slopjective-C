#!/usr/bin/env python3
"""Shared GitHub CLI client with pagination, retries, and UTF-8 decode policy."""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any, Callable, Sequence

TRANSIENT_HTTP_STATUSES = {429, 500, 502, 503, 504}
HTTP_STATUS_RE = re.compile(r"\bHTTP\s+(\d{3})\b", re.IGNORECASE)
TRANSIENT_PATTERNS = (
    re.compile(r"\brate limit", re.IGNORECASE),
    re.compile(r"\btimeout\b", re.IGNORECASE),
    re.compile(r"\btimed out\b", re.IGNORECASE),
    re.compile(r"\btemporar(?:y|ily)\b", re.IGNORECASE),
    re.compile(r"\bconnection (?:reset|closed|refused)\b", re.IGNORECASE),
    re.compile(r"\bnetwork\b", re.IGNORECASE),
)

CommandRunner = Callable[[Sequence[str], Path], subprocess.CompletedProcess[bytes]]
SleepFn = Callable[[float], None]


class GhClientError(RuntimeError):
    """Raised when a GitHub CLI command fails or returns invalid payloads."""


def decode_utf8_safe(value: bytes | str | None) -> str:
    """Decode subprocess payloads with explicit UTF-8 replacement semantics."""

    if value is None:
        return ""
    if isinstance(value, str):
        return value
    return value.decode("utf-8", errors="replace")


def _default_runner(args: Sequence[str], cwd: Path) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        args,
        cwd=cwd,
        capture_output=True,
        text=False,
        check=False,
    )


class GhClient:
    """Small wrapper around `gh` with retry/backoff and pagination helpers."""

    def __init__(
        self,
        *,
        root: Path,
        max_retries: int = 3,
        backoff_base_seconds: float = 0.25,
        runner: CommandRunner | None = None,
        sleeper: SleepFn | None = None,
    ) -> None:
        if max_retries < 0:
            raise ValueError("max_retries must be >= 0")
        if backoff_base_seconds < 0:
            raise ValueError("backoff_base_seconds must be >= 0")
        self._root = root
        self._max_retries = max_retries
        self._backoff_base_seconds = backoff_base_seconds
        self._runner = runner or _default_runner
        self._sleeper = sleeper or time.sleep

    def run_json(self, args: Sequence[str]) -> Any:
        """Run `gh ...args` and parse stdout JSON with retry/backoff on transients."""

        attempt = 0
        while True:
            proc = self._runner(["gh", *args], self._root)
            stdout = decode_utf8_safe(proc.stdout)
            stderr = decode_utf8_safe(proc.stderr)

            if proc.returncode == 0:
                try:
                    return json.loads(stdout)
                except json.JSONDecodeError as exc:
                    raise GhClientError(
                        f"gh {' '.join(args)} returned invalid JSON: {exc}"
                    ) from exc

            detail = stderr.strip() or stdout.strip() or f"exit code {proc.returncode}"
            if attempt < self._max_retries and self._is_transient_failure(detail):
                delay = self._backoff_base_seconds * (2**attempt)
                self._sleeper(delay)
                attempt += 1
                continue

            raise GhClientError(f"gh {' '.join(args)} failed: {detail}")

    def api_json(self, endpoint: str, *, paginate: bool = False) -> Any:
        args = ["api", endpoint]
        if paginate:
            args.extend(["--paginate", "--slurp"])
        return self.run_json(args)

    def list_issues(self, *, state: str) -> list[dict[str, Any]]:
        """Return all repository issues for state, paginated and PR-filtered."""

        endpoint = f"repos/{{owner}}/{{repo}}/issues?state={state}&per_page=100"
        payload = self.api_json(endpoint, paginate=True)
        if not isinstance(payload, list):
            raise GhClientError(
                f"gh api {endpoint} returned unexpected paginated payload shape"
            )

        issues: list[dict[str, Any]] = []
        for page in payload:
            if not isinstance(page, list):
                raise GhClientError(
                    f"gh api {endpoint} returned unexpected paginated payload shape"
                )
            for item in page:
                if not isinstance(item, dict):
                    continue
                if "pull_request" in item:
                    continue
                issues.append(item)

        return issues

    def issue_numbers(self, *, state: str) -> set[int]:
        numbers: set[int] = set()
        for item in self.list_issues(state=state):
            number = item.get("number")
            if isinstance(number, int):
                numbers.add(number)
        return numbers

    @staticmethod
    def _is_transient_failure(detail: str) -> bool:
        status_match = HTTP_STATUS_RE.search(detail)
        if status_match:
            try:
                status = int(status_match.group(1))
            except ValueError:
                status = None
            if status in TRANSIENT_HTTP_STATUSES:
                return True

        return any(pattern.search(detail) for pattern in TRANSIENT_PATTERNS)
