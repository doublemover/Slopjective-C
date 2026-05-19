"""Report rendering for the objc3c fuzz-safety runner."""

from __future__ import annotations

import json


def canonical_json_text(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def render_human_summary(summary: dict[str, object]) -> str:
    status = summary["status"]
    config = summary["config"]
    assert isinstance(config, dict)
    lines = [
        f"status: {status}",
        f"mode: {summary['mode']}",
        f"case_count: {config['case_count']}",
        f"violation_count: {summary['violation_count']}",
    ]
    if status == "FAIL":
        lines.append("violations:")
        violations = summary["violations"]
        assert isinstance(violations, list)
        for item in violations:
            assert isinstance(item, dict)
            lines.append(
                f"- [{item['check_id']}] {item['case_id']}: {item['detail']}"
            )
    return "\n".join(lines) + "\n"
