from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_report_json


def write_summary_json(summary_path: Path, summary_payload: dict[str, Any]) -> None:
    write_report_json(summary_path, summary_payload, sort_keys=False)


def render_summary_markdown(summary_payload: dict[str, Any]) -> str:
    selected_ids = summary_payload.get("selected_example_ids", [])
    examples = summary_payload.get("examples", [])
    lines = [
        "# Showcase Surface Summary",
        "",
        f"Contract: {summary_payload.get('contract_id')}",
        f"Selected examples: {', '.join(str(example_id) for example_id in selected_ids)}",
        "",
        "| Example | Module | Source | Output |",
        "| --- | --- | --- | --- |",
    ]
    if isinstance(examples, list):
        for entry in examples:
            if not isinstance(entry, dict):
                continue
            lines.append(
                "| "
                + " | ".join(
                    [
                        str(entry.get("example_id", "")),
                        str(entry.get("module_name", "")),
                        str(entry.get("source", "")),
                        str(entry.get("out_dir", "")),
                    ]
                )
                + " |"
            )
    return "\n".join(lines) + "\n"
