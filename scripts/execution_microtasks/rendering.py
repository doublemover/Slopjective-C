from __future__ import annotations

from datetime import date

from execution_microtasks.model import Issue
from execution_microtasks.normalization import normalize_line_endings


def render_markdown(*, issues: list[Issue], closed_count: int, generated_on: date) -> str:
    open_count = len(issues)
    minimum_target = closed_count * 2
    planned_microtasks = open_count * 4

    lines: list[str] = [
        "# Execution Microtask Backlog (2x+ Expansion)",
        "",
        f"_Generated on {generated_on.isoformat()} from GitHub issue snapshot._",
        "",
        "## Baseline Metrics",
        "",
        f"- Snapshot date: **{generated_on.isoformat()}**",
        f"- Closed issues (completed tasks): **{closed_count}**",
        f"- Open issues (remaining tasks): **{open_count}**",
        f"- Minimum future-task target (2x completed): **{minimum_target}**",
        f"- Planned microtasks in this backlog: **{planned_microtasks}** (4 per open issue)",
        "",
        "## Microtasks By Open Issue",
        "",
    ]

    if not issues:
        lines.append("No open issues currently.")
        lines.append("")
    else:
        for issue in issues:
            lines.append(f"### Issue #{issue.number}: {issue.title}")
            if issue.labels:
                lines.append(f"_Labels: {', '.join(issue.labels)}_")
            lines.append("")
            lines.append(
                f"1. **Implementation**: Implement issue #{issue.number} requirements and "
                "update code/docs as needed."
            )
            lines.append(
                f"2. **Verification**: Add or update automated/manual checks for issue "
                f"#{issue.number} and capture evidence."
            )
            lines.append(
                f"3. **Integration**: Validate dependent workflows and cross-component "
                f"behavior for issue #{issue.number}."
            )
            lines.append(
                "4. **Closeout sync**: Post commit and test evidence on issue "
                f"#{issue.number}, then synchronize related epic/workpack status."
            )
            lines.append("")

    lines.extend(
        [
            "## Totals Summary",
            "",
            f"- Issues represented: **{open_count}**",
            f"- Total generated microtasks: **{planned_microtasks}**",
            f"- Implementation microtasks: **{open_count}**",
            f"- Verification microtasks: **{open_count}**",
            f"- Integration microtasks: **{open_count}**",
            f"- Closeout sync microtasks: **{open_count}**",
        ]
    )

    return normalize_line_endings("\n".join(lines) + "\n")
