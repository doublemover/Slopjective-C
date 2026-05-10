from __future__ import annotations

from .constants import ROOT
from .models import LintError


def format_lint_error(error: LintError) -> str:
    rel = error.path.relative_to(ROOT)
    return f"{rel}:{error.line}: {error.message}"


def render_failure_report(errors: list[LintError]) -> str:
    lines = [format_lint_error(error) for error in errors]
    lines.extend(("", f"Found {len(errors)} spec-lint issue(s)."))
    return "\n".join(lines)


def render_success_report() -> str:
    return "spec-lint: OK"
