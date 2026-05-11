from __future__ import annotations

import subprocess

from .subprocess_models import CommandExecution, DEFAULT_SNIPPET_CHARS
from .subprocess_output import bounded_text


def failure_snippet(result: subprocess.CompletedProcess[str] | CommandExecution, *, limit: int = DEFAULT_SNIPPET_CHARS) -> str:
    stdout = result.stdout or ""
    stderr = result.stderr or ""
    parts: list[str] = []
    if stdout:
        parts.append(f"stdout:\n{bounded_text(stdout, limit)}")
    if stderr:
        parts.append(f"stderr:\n{bounded_text(stderr, limit)}")
    return "\n".join(parts)


def raise_if_failed(
    result: subprocess.CompletedProcess[str] | CommandExecution,
    *,
    context: str,
    limit: int = DEFAULT_SNIPPET_CHARS,
) -> None:
    returncode = int(result.returncode)
    if returncode == 0:
        return
    snippet = failure_snippet(result, limit=limit)
    detail = f"\n{snippet}" if snippet else ""
    raise RuntimeError(f"{context} failed with exit code {returncode}{detail}")


__all__ = (
    "failure_snippet",
    "raise_if_failed",
)
