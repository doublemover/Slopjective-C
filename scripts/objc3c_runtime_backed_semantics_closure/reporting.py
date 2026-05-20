from __future__ import annotations


def render_markdown(summary: dict) -> str:
    counts = summary["counts"]
    lines = [
        "# Runtime-Backed Semantics Closure",
        "",
        f"- Issue: `{summary['issue']}`",
        f"- Contract: `{summary['contract_id']}`",
        f"- Status: `{summary['status']}`",
        f"- Positive executable fixtures: `{counts['positive_fixture_count']}`",
        f"- Negative fail-closed fixtures: `{counts['negative_fixture_count']}`",
        f"- Private runtime helper symbols: `{counts['runtime_helper_symbol_count']}`",
        f"- Durable replay fixture directories: `{counts['durable_replay_fixture_dir_count']}`",
        f"- Report output: `{summary['report_directory']}` (not source truth)",
        f"- Scratch output: `{summary['scratch_directory']}` (not source truth)",
        "",
        "## Checks",
        "",
    ]
    for name, value in summary["checks"].items():
        lines.append(f"- `{name}`: `{str(value).lower()}`")
    lines.extend(["", "## Fixture Coverage", ""])
    for name, item in summary["positive_compile"].items():
        lines.append(f"- `{name}`: `{item['fixture']}` compiled=`{str(item['compiled']).lower()}`")
    for name, item in summary["negative_compile"].items():
        lines.append(
            f"- `{name}`: `{item['fixture']}` rejected=`{str(item['rejected']).lower()}` "
            f"codes=`{','.join(item['observed_codes'])}`"
        )
    lines.extend(["", "## Runtime Helper Boundary", ""])
    for symbol, present in summary["runtime_helper_ir_declarations"].items():
        lines.append(f"- `{symbol}`: ir_declared=`{str(present).lower()}`")
    lines.append("")
    return "\n".join(lines)
