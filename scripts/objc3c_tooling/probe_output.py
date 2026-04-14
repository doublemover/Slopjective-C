from __future__ import annotations

import json
import subprocess
from typing import Any, Iterable

from objc3c_tooling.subprocesses import bounded_text


def _result_context(result: subprocess.CompletedProcess[str], *, limit: int) -> str:
    stdout = result.stdout or ""
    stderr = result.stderr or ""
    parts: list[str] = []
    if stdout:
        parts.append(f"stdout:\n{bounded_text(stdout, limit)}")
    if stderr:
        parts.append(f"stderr:\n{bounded_text(stderr, limit)}")
    return "\n".join(parts)


def parse_json_output(
    result: subprocess.CompletedProcess[str],
    label: str,
    *,
    require_success: bool = True,
    snippet_chars: int = 4000,
) -> dict[str, Any]:
    if require_success and result.returncode != 0:
        detail = _result_context(result, limit=snippet_chars)
        suffix = f"\n{detail}" if detail else ""
        raise RuntimeError(f"{label} failed with exit code {result.returncode}{suffix}")
    stdout = (result.stdout or "").strip()
    if not stdout:
        detail = _result_context(result, limit=snippet_chars)
        suffix = f"\n{detail}" if detail else ""
        raise RuntimeError(f"{label} produced no JSON output{suffix}")
    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(
            f"{label} produced invalid JSON: {exc}\nstdout:\n{bounded_text(stdout, snippet_chars)}"
        ) from exc
    if not isinstance(payload, dict):
        raise RuntimeError(f"{label} did not produce a JSON object")
    return payload


def parse_key_value_output(
    result: subprocess.CompletedProcess[str],
    label: str,
    *,
    require_success: bool = True,
    required_keys: Iterable[str] = (),
    allow_duplicate_keys: bool = False,
    snippet_chars: int = 4000,
) -> dict[str, Any]:
    if require_success and result.returncode != 0:
        detail = _result_context(result, limit=snippet_chars)
        suffix = f"\n{detail}" if detail else ""
        raise RuntimeError(f"{label} failed with exit code {result.returncode}{suffix}")
    stdout = (result.stdout or "").strip()
    if not stdout:
        detail = _result_context(result, limit=snippet_chars)
        suffix = f"\n{detail}" if detail else ""
        raise RuntimeError(f"{label} produced no key/value output{suffix}")
    payload: dict[str, Any] = {}
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if "=" not in line:
            raise RuntimeError(f"{label} produced malformed line: {bounded_text(line, snippet_chars)}")
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if not key:
            raise RuntimeError(f"{label} produced key/value line with an empty key")
        if not allow_duplicate_keys and key in payload:
            raise RuntimeError(f"{label} produced duplicate key: {key}")
        if value and value.lstrip("-").isdigit():
            payload[key] = int(value)
        else:
            payload[key] = value
    if not payload:
        raise RuntimeError(f"{label} produced no parseable key/value output")
    missing = sorted(set(required_keys) - set(payload))
    if missing:
        raise RuntimeError(f"{label} missing required key(s): {', '.join(missing)}")
    return payload
