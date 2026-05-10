"""GitHub issue seeding helpers for the remaining spec task seed tool."""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path
from typing import Any

from remaining_task_extraction.seed_outputs import build_issue_body
from remaining_task_extraction.seed_review import ReviewedTask

ROOT = Path(__file__).resolve().parents[2]


class GhError(RuntimeError):
    pass


def run_cmd(args: list[str], input_text: str | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        args,
        cwd=ROOT,
        input=input_text,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def run_gh_json(args: list[str], input_payload: dict[str, Any] | None = None) -> Any:
    input_text = None
    full_args = ["gh", *args]
    if input_payload is not None:
        full_args.extend(["--input", "-"])
        input_text = json.dumps(input_payload)

    proc = run_cmd(full_args, input_text=input_text)
    if proc.returncode != 0:
        detail = proc.stderr.strip() or proc.stdout.strip() or f"exit {proc.returncode}"
        raise GhError(f"{' '.join(full_args)} failed: {detail}")

    stdout = proc.stdout.strip()
    if not stdout:
        return None

    try:
        return json.loads(stdout)
    except json.JSONDecodeError as exc:
        raise GhError(f"{' '.join(full_args)} returned invalid JSON: {exc}") from exc


def ensure_labels(repo: str, label_defs: dict[str, tuple[str, str]]) -> None:
    existing = run_gh_json(["label", "list", "--limit", "500", "--json", "name"])
    existing_names = {
        item["name"] for item in existing if isinstance(item, dict) and isinstance(item.get("name"), str)
    }

    for name, (color, description) in label_defs.items():
        if name in existing_names:
            continue
        run_gh_json(
            [
                "api",
                f"repos/{repo}/labels",
                "-X",
                "POST",
                "-f",
                f"name={name}",
                "-f",
                f"color={color}",
                "-f",
                f"description={description}",
            ]
        )


def fetch_milestone_map(repo: str) -> dict[str, int]:
    payload = run_gh_json(
        ["api", f"repos/{repo}/milestones?state=all&per_page=100"]
    )
    mapping: dict[str, int] = {}
    if isinstance(payload, list):
        for item in payload:
            if not isinstance(item, dict):
                continue
            title = item.get("title")
            number = item.get("number")
            if isinstance(title, str) and isinstance(number, int):
                mapping[title] = number
    return mapping


def ensure_lane_milestones(
    repo: str,
    lane_milestone_titles: dict[str, str],
    lane_milestone_due_on: dict[str, str],
    lane_name: dict[str, str],
) -> dict[str, int]:
    mapping = fetch_milestone_map(repo)
    for lane, title in lane_milestone_titles.items():
        if title in mapping:
            continue
        description = (
            f"Parallel lane {lane} ({lane_name[lane]}) task batch generated from the 510-task unchecked spec sweep."
        )
        due_on = lane_milestone_due_on[lane]
        payload = run_gh_json(
            ["api", f"repos/{repo}/milestones", "-X", "POST"],
            input_payload={
                "title": title,
                "description": description,
                "due_on": due_on,
            },
        )
        if isinstance(payload, dict) and isinstance(payload.get("number"), int):
            mapping[title] = int(payload["number"])

    return mapping


def fetch_existing_seeded_task_ids() -> set[str]:
    payload = run_gh_json(
        ["issue", "list", "--state", "all", "--limit", "2000", "--json", "title"]
    )
    ids: set[str] = set()
    if not isinstance(payload, list):
        return ids

    pattern = re.compile(r"^\[(SPT-\d{4})\]")
    for item in payload:
        if not isinstance(item, dict):
            continue
        title = item.get("title")
        if not isinstance(title, str):
            continue
        match = pattern.match(title)
        if match:
            ids.add(match.group(1))
    return ids


def create_issue(repo: str, payload: dict[str, Any]) -> dict[str, Any]:
    retries = 6
    wait_seconds = 1.0
    for attempt in range(1, retries + 1):
        proc = run_cmd(
            ["gh", "api", f"repos/{repo}/issues", "-X", "POST", "--input", "-"],
            input_text=json.dumps(payload),
        )
        if proc.returncode == 0:
            try:
                result = json.loads(proc.stdout)
            except json.JSONDecodeError as exc:
                raise GhError(f"Issue creation returned non-JSON output: {exc}") from exc
            if not isinstance(result, dict):
                raise GhError("Issue creation returned unexpected JSON shape")
            return result

        stderr = proc.stderr.strip() or proc.stdout.strip()
        too_fast = "secondary rate limit" in stderr.lower() or "abuse detection" in stderr.lower()
        transient = "502" in stderr or "503" in stderr or "504" in stderr

        if attempt < retries and (too_fast or transient):
            sleep_for = max(wait_seconds, 60.0 if too_fast else wait_seconds)
            time.sleep(sleep_for)
            wait_seconds *= 2
            continue

        raise GhError(f"Issue creation failed: {stderr or f'exit {proc.returncode}'}")

    raise GhError("Issue creation failed after retries")


def seed_issues(
    repo: str,
    tasks: list[ReviewedTask],
    milestone_map: dict[str, int],
    lane_name: dict[str, str],
    sleep_seconds: float,
    limit: int | None,
) -> tuple[int, int]:
    existing_ids = fetch_existing_seeded_task_ids()

    created = 0
    skipped = 0
    processed = 0

    for task in tasks:
        if limit is not None and processed >= limit:
            break
        processed += 1

        if task.task_id in existing_ids:
            skipped += 1
            continue

        milestone_number = milestone_map.get(task.milestone_title)
        if milestone_number is None:
            raise GhError(
                f"Missing milestone mapping for '{task.milestone_title}' while seeding {task.task_id}"
            )

        payload = {
            "title": task.title,
            "body": build_issue_body(task, lane_name),
            "milestone": milestone_number,
            "labels": task.labels,
        }

        result = create_issue(repo, payload)
        number = result.get("number")
        url = result.get("html_url")
        if isinstance(number, int) and isinstance(url, str):
            print(f"created #{number} {task.task_id} {url}")
        else:
            print(f"created {task.task_id}")

        created += 1
        time.sleep(sleep_seconds)

    return created, skipped
