"""Validation timing dashboard markdown rendering."""

from __future__ import annotations

from pathlib import Path


def write_validation_timing_markdown(payload: dict[str, object], path: Path) -> None:
    lines = [
        "# ObjC3 Validation Timing Dashboard",
        "",
        f"- generated: `{payload['generated_at_utc']}`",
        "- contract: `objc3c.validation.speed.dashboard.v1`",
        f"- hard-blocking: `{payload.get('hard_blocking_decision')}`",
        "",
        "## Reports",
    ]
    reports = payload.get("reports", {})
    if isinstance(reports, dict):
        for name, report in reports.items():
            if not isinstance(report, dict):
                continue
            lines.append(
                f"- `{name}`: status=`{report.get('status')}` "
                f"elapsed=`{report.get('elapsed_seconds', 0.0)}` "
                f"path=`{report.get('report_path')}`"
            )
    lines.extend(["", "## Budget Status"])
    budgets = payload.get("budgets", [])
    if isinstance(budgets, list):
        for budget in budgets:
            if isinstance(budget, dict):
                lines.append(
                    f"- `{budget.get('name')}`: `{budget.get('status')}` "
                    f"actual=`{budget.get('actual_seconds', budget.get('actual_count'))}` "
                    f"owner=`{budget.get('budget_owner')}`"
                )
    lines.extend(["", "## Owners"])
    owners = payload.get("owners", {})
    if isinstance(owners, dict):
        for key, owner in owners.items():
            lines.append(f"- `{key}`: `{owner}`")
    lines.extend(["", "## Validation Profiles"])
    profiles = payload.get("validation_profiles", {})
    if isinstance(profiles, dict):
        for profile in profiles.get("profiles", []):
            if isinstance(profile, dict):
                actions = ", ".join(
                    str(action) for action in profile.get("recommended_actions", [])
                )
                lines.append(f"- `{profile.get('profile')}`: {actions}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
