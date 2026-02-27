#!/usr/bin/env python3
"""Generate deterministic v0.13 seed issue payload records."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_GRAPH_PATH = ROOT / "spec/planning/v013_seed_dependency_graph.json"
DEFAULT_OUTPUT_PATH = ROOT / "spec/planning/v013_seed_issue_payloads.json"

SEED_ID_RE = re.compile(r"^V013-[A-Z]+-[0-9]{2}$")
WAVE_ID_RE = re.compile(r"^W[0-9]+$")
ACCEPTANCE_GATE_ID_RE = re.compile(r"^AC-V013-[A-Z]+-[0-9]{2}$")
SHARD_CLASSES = {"small", "medium", "large"}
SEED_TOKEN_RE = re.compile(r"(?<![A-Z0-9-])(V013-[A-Z]+-[0-9]{2})(?![A-Z0-9-])")
ISSUE_STATES = {"open", "closed"}
SOURCE_CONTRACT_ID = "V013-TOOL-03-SEED-DAG-v1"
SOURCE_SEED_ID = "V013-TOOL-03"


class ParseError(ValueError):
    """Raised when the seed graph input cannot be parsed deterministically."""


@dataclass(frozen=True)
class SeedMetadata:
    seed_id: str
    title: str
    wave_id: str
    depends_on: tuple[str, ...]
    shard_class: str
    acceptance_gate_id: str
    priority_score: int
    duv: int
    dc: int
    tier: str


@dataclass(frozen=True)
class TemplateMetadata:
    labels: tuple[str, ...]
    validation_commands: tuple[str, ...]


@dataclass(frozen=True)
class IssueOverlayMetadata:
    number: int
    title: str
    labels: tuple[str, ...]
    issue_url: str | None
    issue_state: str
    closed_at: str | None
    matched_seed_ids: tuple[str, ...]


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="generate_seed_issue_payloads.py",
        description=(
            "Generate deterministic issue payload records from "
            "spec/planning/v013_seed_dependency_graph.json."
        ),
    )
    parser.add_argument(
        "--graph",
        type=Path,
        default=DEFAULT_GRAPH_PATH,
        help="Input seed dependency graph JSON path.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT_PATH,
        help="Output JSON path for generated issue payload records.",
    )
    parser.add_argument(
        "--issues-json",
        type=Path,
        help=(
            "Optional GitHub issue snapshot JSON array. "
            "When provided, records are overlaid with completion metadata "
            "for unambiguous seed/issue matches."
        ),
    )
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="Print generated JSON to stdout after writing output.",
    )
    return parser


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def expect_dict(value: Any, context: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ParseError(f"{context} must be an object")
    return value


def expect_list(value: Any, context: str) -> list[Any]:
    if not isinstance(value, list):
        raise ParseError(f"{context} must be an array")
    return value


def expect_nonempty_str(value: Any, context: str) -> str:
    if not isinstance(value, str):
        raise ParseError(f"{context} must be a string")
    stripped = value.strip()
    if not stripped:
        raise ParseError(f"{context} must be a non-empty string")
    return stripped


def expect_int(value: Any, context: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int):
        raise ParseError(f"{context} must be an integer")
    return value


def parse_string_list(value: Any, context: str) -> tuple[str, ...]:
    items = expect_list(value, context)
    if not items:
        raise ParseError(f"{context} must not be empty")
    parsed: list[str] = []
    for idx, item in enumerate(items):
        parsed.append(expect_nonempty_str(item, f"{context}[{idx}]"))
    return tuple(parsed)


def parse_issue_number(value: Any, context: str) -> int:
    if isinstance(value, bool):
        raise ParseError(f"{context} must be an integer issue number")
    if isinstance(value, int):
        return value
    if isinstance(value, str) and value.isdigit():
        return int(value)
    raise ParseError(f"{context} must be an integer issue number")


def parse_issue_labels(value: Any, context: str) -> tuple[str, ...]:
    if value is None:
        return ()
    rows = expect_list(value, context)
    parsed: list[str] = []
    for idx, raw_label in enumerate(rows):
        item_context = f"{context}[{idx}]"
        if isinstance(raw_label, str):
            parsed.append(expect_nonempty_str(raw_label, item_context))
            continue
        if isinstance(raw_label, dict):
            parsed.append(
                expect_nonempty_str(raw_label.get("name"), f"{item_context}.name")
            )
            continue
        raise ParseError(f"{item_context} must be a label string or object")
    return tuple(parsed)


def parse_optional_nonempty_str(value: Any, context: str) -> str | None:
    if value is None:
        return None
    return expect_nonempty_str(value, context)


def parse_issue_state(value: Any, context: str) -> str | None:
    if value is None:
        return None
    state = expect_nonempty_str(value, context).lower()
    if state not in ISSUE_STATES:
        raise ParseError(f"{context} must be one of {sorted(ISSUE_STATES)}; got {state}")
    return state


def extract_seed_ids_from_title(title: str) -> set[str]:
    return {seed_id.upper() for seed_id in SEED_TOKEN_RE.findall(title)}


def extract_seed_ids_from_labels(labels: Sequence[str]) -> set[str]:
    seed_ids: set[str] = set()
    for label in labels:
        if ":" not in label:
            continue
        prefix, _, suffix = label.partition(":")
        if prefix.strip().lower() != "seed":
            continue
        seed_id = suffix.strip().upper()
        if SEED_ID_RE.match(seed_id):
            seed_ids.add(seed_id)
    return seed_ids


def parse_issues_overlay(path: Path, payload: Any) -> list[IssueOverlayMetadata]:
    rows = expect_list(payload, f"issues snapshot {display_path(path)}")
    parsed: list[IssueOverlayMetadata] = []
    seen_numbers: set[int] = set()
    for idx, raw_issue in enumerate(rows):
        context = f"issues snapshot {display_path(path)}[{idx}]"
        issue = expect_dict(raw_issue, context)

        number = parse_issue_number(issue.get("number"), f"{context}.number")
        if number in seen_numbers:
            raise ParseError(f"{context}.number duplicates issue #{number}")
        seen_numbers.add(number)

        title = expect_nonempty_str(issue.get("title"), f"{context}.title")
        labels = parse_issue_labels(issue.get("labels"), f"{context}.labels")

        issue_url_raw = issue.get("html_url")
        if issue_url_raw is None:
            issue_url_raw = issue.get("url")
        if issue_url_raw is None:
            issue_url_raw = issue.get("issue_url")
        issue_url = parse_optional_nonempty_str(issue_url_raw, f"{context}.issue_url")

        state_raw = issue.get("state")
        if state_raw is None:
            state_raw = issue.get("issue_state")
        issue_state = parse_issue_state(state_raw, f"{context}.state")

        closed_at_raw = issue.get("closed_at")
        if closed_at_raw is None:
            closed_at_raw = issue.get("closedAt")
        closed_at = parse_optional_nonempty_str(closed_at_raw, f"{context}.closed_at")

        # Open-issues snapshots commonly omit explicit state; treat missing state
        # as open unless a closed timestamp is present.
        if issue_state is None:
            issue_state = "closed" if closed_at is not None else "open"

        matched_seed_ids = tuple(
            sorted(extract_seed_ids_from_title(title) | extract_seed_ids_from_labels(labels))
        )
        parsed.append(
            IssueOverlayMetadata(
                number=number,
                title=title,
                labels=labels,
                issue_url=issue_url,
                issue_state=issue_state,
                closed_at=closed_at,
                matched_seed_ids=matched_seed_ids,
            )
        )

    return parsed


def resolve_seed_issue_overlay(
    seeds: Sequence[SeedMetadata],
    issues: Sequence[IssueOverlayMetadata],
) -> dict[str, IssueOverlayMetadata]:
    known_seed_ids = {seed.seed_id for seed in seeds}
    candidates_by_seed: dict[str, set[int]] = {seed_id: set() for seed_id in known_seed_ids}
    candidate_seeds_by_issue: dict[int, set[str]] = {}
    issues_by_number: dict[int, IssueOverlayMetadata] = {issue.number: issue for issue in issues}

    for issue in issues:
        matching_seed_ids = {seed_id for seed_id in issue.matched_seed_ids if seed_id in known_seed_ids}
        if not matching_seed_ids:
            continue
        candidate_seeds_by_issue[issue.number] = set(matching_seed_ids)
        for seed_id in matching_seed_ids:
            candidates_by_seed[seed_id].add(issue.number)

    resolved: dict[str, IssueOverlayMetadata] = {}
    for seed_id, candidate_issue_numbers in candidates_by_seed.items():
        if len(candidate_issue_numbers) != 1:
            continue
        issue_number = next(iter(candidate_issue_numbers))
        if len(candidate_seeds_by_issue.get(issue_number, set())) != 1:
            continue
        resolved[seed_id] = issues_by_number[issue_number]

    return resolved


def completion_status_for(issue_state: str, closed_at: str | None) -> str:
    if issue_state == "closed" or closed_at is not None:
        return "completed"
    return "incomplete"


def parse_seeds(payload: dict[str, Any]) -> list[SeedMetadata]:
    graph = expect_dict(payload.get("graph"), "root.graph")
    seed_rows = expect_list(graph.get("seeds"), "root.graph.seeds")
    if not seed_rows:
        raise ParseError("root.graph.seeds must not be empty")

    parsed: list[SeedMetadata] = []
    seen_seed_ids: set[str] = set()
    for idx, raw_seed in enumerate(seed_rows):
        context = f"root.graph.seeds[{idx}]"
        seed = expect_dict(raw_seed, context)

        missing_keys = sorted(
            key
            for key in (
                "seed_id",
                "proposed_issue_title",
                "wave_id",
                "depends_on",
                "shard_class",
                "acceptance_gate_id",
                "priority",
            )
            if key not in seed
        )
        if missing_keys:
            raise ParseError(f"{context} missing required key(s): {', '.join(missing_keys)}")

        seed_id = expect_nonempty_str(seed.get("seed_id"), f"{context}.seed_id")
        if not SEED_ID_RE.match(seed_id):
            raise ParseError(f"{context}.seed_id has invalid format: {seed_id}")
        if seed_id in seen_seed_ids:
            raise ParseError(f"duplicate seed id in root.graph.seeds: {seed_id}")
        seen_seed_ids.add(seed_id)

        title = expect_nonempty_str(
            seed.get("proposed_issue_title"),
            f"{context}.proposed_issue_title",
        )
        wave_id = expect_nonempty_str(seed.get("wave_id"), f"{context}.wave_id")
        if not WAVE_ID_RE.match(wave_id):
            raise ParseError(f"{context}.wave_id has invalid format: {wave_id}")

        depends_on_raw = expect_list(seed.get("depends_on"), f"{context}.depends_on")
        depends_on: list[str] = []
        for dep_idx, raw_dep in enumerate(depends_on_raw):
            dependency = expect_nonempty_str(
                raw_dep,
                f"{context}.depends_on[{dep_idx}]",
            )
            if not SEED_ID_RE.match(dependency):
                raise ParseError(
                    f"{context}.depends_on[{dep_idx}] has invalid seed id: {dependency}"
                )
            depends_on.append(dependency)

        shard_class = expect_nonempty_str(seed.get("shard_class"), f"{context}.shard_class")
        if shard_class not in SHARD_CLASSES:
            raise ParseError(
                f"{context}.shard_class must be one of {sorted(SHARD_CLASSES)}; got {shard_class}"
            )

        acceptance_gate_id = expect_nonempty_str(
            seed.get("acceptance_gate_id"),
            f"{context}.acceptance_gate_id",
        )
        if not ACCEPTANCE_GATE_ID_RE.match(acceptance_gate_id):
            raise ParseError(
                f"{context}.acceptance_gate_id has invalid format: {acceptance_gate_id}"
            )

        priority = expect_dict(seed.get("priority"), f"{context}.priority")
        priority_score = expect_int(
            priority.get("priority_score"),
            f"{context}.priority.priority_score",
        )
        duv = expect_int(priority.get("duv"), f"{context}.priority.duv")
        dc = expect_int(priority.get("dc"), f"{context}.priority.dc")
        tier = expect_nonempty_str(priority.get("tier"), f"{context}.priority.tier")

        parsed.append(
            SeedMetadata(
                seed_id=seed_id,
                title=title,
                wave_id=wave_id,
                depends_on=tuple(depends_on),
                shard_class=shard_class,
                acceptance_gate_id=acceptance_gate_id,
                priority_score=priority_score,
                duv=duv,
                dc=dc,
                tier=tier,
            )
        )

    known_seed_ids = {seed.seed_id for seed in parsed}
    for seed in parsed:
        for dependency in seed.depends_on:
            if dependency == seed.seed_id:
                raise ParseError(f"seed {seed.seed_id} must not depend on itself")
            if dependency not in known_seed_ids:
                raise ParseError(
                    f"seed {seed.seed_id} depends on unknown seed id: {dependency}"
                )

    return parsed


def parse_templates(payload: dict[str, Any]) -> dict[str, TemplateMetadata]:
    batch_skeletons = expect_dict(payload.get("batch_skeletons"), "root.batch_skeletons")
    batches = expect_list(batch_skeletons.get("batches"), "root.batch_skeletons.batches")
    if not batches:
        raise ParseError("root.batch_skeletons.batches must not be empty")

    templates: dict[str, TemplateMetadata] = {}
    for batch_idx, raw_batch in enumerate(batches):
        batch_context = f"root.batch_skeletons.batches[{batch_idx}]"
        batch = expect_dict(raw_batch, batch_context)
        issue_templates = expect_list(
            batch.get("issue_templates"),
            f"{batch_context}.issue_templates",
        )
        for template_idx, raw_template in enumerate(issue_templates):
            context = f"{batch_context}.issue_templates[{template_idx}]"
            template = expect_dict(raw_template, context)
            seed_id = expect_nonempty_str(template.get("seed_id"), f"{context}.seed_id")
            labels = parse_string_list(template.get("labels"), f"{context}.labels")
            body_fields = expect_dict(template.get("body_fields"), f"{context}.body_fields")
            validation_commands = parse_string_list(
                body_fields.get("validation_commands"),
                f"{context}.body_fields.validation_commands",
            )

            if seed_id in templates:
                raise ParseError(f"duplicate issue template seed id: {seed_id}")
            templates[seed_id] = TemplateMetadata(
                labels=labels,
                validation_commands=validation_commands,
            )

    if not templates:
        raise ParseError("root.batch_skeletons.batches must contain at least one issue template")
    return templates


def wave_index(wave_id: str) -> int:
    return int(wave_id[1:])


def seed_sort_key(seed: SeedMetadata) -> tuple[int, int, int, int, str]:
    return (
        wave_index(seed.wave_id),
        -seed.priority_score,
        -seed.duv,
        seed.dc,
        seed.seed_id,
    )


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


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return generate(
            graph_path=args.graph,
            output_path=args.output,
            print_stdout=args.stdout,
            issues_json_path=args.issues_json,
        )
    except ParseError as exc:
        print(f"seed-issue-payloads: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
