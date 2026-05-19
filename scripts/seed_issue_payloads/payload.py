from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from seed_issue_payloads.graph import parse_seeds
from seed_issue_payloads.graph import parse_templates
from seed_issue_payloads.graph import seed_sort_key
from seed_issue_payloads.issues import completion_status_for
from seed_issue_payloads.issues import parse_issues_overlay
from seed_issue_payloads.issues import resolve_seed_issue_overlay
from seed_issue_payloads.model import SOURCE_CONTRACT_ID
from seed_issue_payloads.model import SOURCE_SEED_ID
from seed_issue_payloads.model import IssueOverlayMetadata
from seed_issue_payloads.model import ParseError
from seed_issue_payloads.parse_utils import expect_dict
from seed_issue_payloads.parse_utils import expect_nonempty_str


def build_payload(
    graph_path: Path,
    payload: dict[str, Any],
    issues_overlay: Sequence[IssueOverlayMetadata] | None = None,
) -> dict[str, Any]:
    seeds = parse_seeds(payload)
    templates = parse_templates(payload)
    resolved_issue_overlay = resolve_seed_issue_overlay(
        seeds,
        issues_overlay if issues_overlay is not None else (),
    )

    seed_ids = {seed.seed_id for seed in seeds}
    template_seed_ids = set(templates.keys())

    missing_templates = sorted(seed_ids - template_seed_ids)
    if missing_templates:
        raise ParseError(
            "missing issue template metadata for seed id(s): "
            + ", ".join(missing_templates)
        )

    unknown_templates = sorted(template_seed_ids - seed_ids)
    if unknown_templates:
        raise ParseError(
            "issue template metadata references unknown seed id(s): "
            + ", ".join(unknown_templates)
        )

    source_contract_id = expect_nonempty_str(payload.get("contract_id"), "root.contract_id")
    if source_contract_id != SOURCE_CONTRACT_ID:
        raise ParseError(
            f"root.contract_id must be {SOURCE_CONTRACT_ID}; got {source_contract_id!r}"
        )
    source_seed_id = expect_nonempty_str(payload.get("seed_id"), "root.seed_id")
    if source_seed_id != SOURCE_SEED_ID:
        raise ParseError(
            f"root.seed_id must be {SOURCE_SEED_ID}; got {source_seed_id!r}"
        )

    records: list[dict[str, Any]] = []
    for seed in sorted(seeds, key=seed_sort_key):
        template = templates[seed.seed_id]
        milestone = f"v0.13 Seed Wave {seed.wave_id}"

        records.append(
            {
                "seed_id": seed.seed_id,
                "title": seed.title,
                "milestone": milestone,
                "milestone_title": milestone,
                "wave_id": seed.wave_id,
                "dependencies": sorted(seed.depends_on),
                "depends_on": sorted(seed.depends_on),
                "labels": list(template.labels),
                "shard_class": seed.shard_class,
                "acceptance_gate": seed.acceptance_gate_id,
                "acceptance_gate_id": seed.acceptance_gate_id,
                "validation_commands": list(template.validation_commands),
                "priority": {
                    "priority_score": seed.priority_score,
                    "duv": seed.duv,
                    "dc": seed.dc,
                    "tier": seed.tier,
                },
            }
        )
        issue_overlay = resolved_issue_overlay.get(seed.seed_id)
        if issue_overlay is not None:
            records[-1]["issue_number"] = issue_overlay.number
            records[-1]["issue_url"] = issue_overlay.issue_url
            records[-1]["issue_state"] = issue_overlay.issue_state
            records[-1]["closed_at"] = issue_overlay.closed_at
            records[-1]["completion_status"] = completion_status_for(
                issue_overlay.issue_state,
                issue_overlay.closed_at,
            )

    return {
        "contract_id": "V013-TOOL-03-SEED-ISSUE-PAYLOAD-v1",
        "seed_id": "V013-TOOL-03",
        "source_contract_id": source_contract_id,
        "source_seed_id": source_seed_id,
        "source_graph_path": display_path(graph_path),
        "record_count": len(records),
        "records": records,
    }


def generate(
    graph_path: Path,
    output_path: Path,
    print_stdout: bool,
    issues_json_path: Path | None = None,
) -> int:
    try:
        source_text = graph_path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise ParseError(f"graph file not found: {graph_path}") from exc

    try:
        parsed = json.loads(source_text)
    except json.JSONDecodeError as exc:
        raise ParseError(
            f"graph file is not valid JSON ({graph_path}): line {exc.lineno}, col {exc.colno}"
        ) from exc

    issues_overlay: list[IssueOverlayMetadata] = []
    if issues_json_path is not None:
        try:
            issues_text = issues_json_path.read_text(encoding="utf-8")
        except FileNotFoundError as exc:
            raise ParseError(f"issues snapshot file not found: {issues_json_path}") from exc

        try:
            parsed_issues = json.loads(issues_text)
        except json.JSONDecodeError as exc:
            raise ParseError(
                "issues snapshot file is not valid JSON "
                f"({issues_json_path}): line {exc.lineno}, col {exc.colno}"
            ) from exc

        issues_overlay = parse_issues_overlay(issues_json_path.resolve(), parsed_issues)

    payload = build_payload(
        graph_path.resolve(),
        expect_dict(parsed, "root"),
        issues_overlay=issues_overlay,
    )

    rendered = json.dumps(payload, indent=2) + "\n"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(rendered, encoding="utf-8")

    print(
        "seed-issue-payloads: OK "
        f"(graph={display_path(graph_path)}, output={display_path(output_path)}, "
        f"records={payload['record_count']})"
    )
    if print_stdout:
        sys.stdout.write(rendered)
    return 0
