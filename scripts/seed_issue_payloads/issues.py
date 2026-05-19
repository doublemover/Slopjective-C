from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from seed_issue_payloads.model import ISSUE_STATES
from seed_issue_payloads.model import SEED_ID_RE
from seed_issue_payloads.model import SEED_TOKEN_RE
from seed_issue_payloads.model import IssueOverlayMetadata
from seed_issue_payloads.model import ParseError
from seed_issue_payloads.model import SeedMetadata
from seed_issue_payloads.parse_utils import expect_dict
from seed_issue_payloads.parse_utils import expect_list
from seed_issue_payloads.parse_utils import expect_nonempty_str
from seed_issue_payloads.parse_utils import parse_optional_nonempty_str


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
