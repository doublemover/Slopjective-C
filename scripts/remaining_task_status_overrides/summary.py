from __future__ import annotations

from collections import Counter
from typing import Any


def render_summary(changes: list[dict[str, Any]], missing: list[str]) -> str:
    after_counts = Counter(change["after_status"] for change in changes)

    lines = [
        "override-apply summary:",
        f"- changed_rows={len(changes)}",
        f"- changed_status_counts={dict(sorted(after_counts.items()))}",
    ]
    if missing:
        lines.append(f"- missing_task_ids={missing}")
    else:
        lines.append("- missing_task_ids=[]")
    return "\n".join(lines)
