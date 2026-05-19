"""Support classification report assembly."""

from __future__ import annotations

import json
from collections.abc import Callable

from objc3c_tooling.reports import expected_json_report

from .models import SupportClassificationReport, SupportClassificationSource
from .rendering import render_support_classification_markdown
from .summary import build_support_classification_summary


def build_support_classification_report(
    source: SupportClassificationSource,
    *,
    path_exists: Callable[[str], bool],
) -> SupportClassificationReport:
    summary = build_support_classification_summary(source, path_exists=path_exists)
    markdown = render_support_classification_markdown(summary)
    return SupportClassificationReport(
        summary=summary,
        markdown=markdown,
        json_report=expected_json_report(summary, sort_keys=False),
        console_json=json.dumps(summary, indent=2),
        status=str(summary["status"]),
    )


__all__ = ["build_support_classification_report"]
