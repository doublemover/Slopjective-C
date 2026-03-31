#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import json
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
TMP = ROOT / "tmp"
PUBLISH_ROOT = TMP / "github-publish"
PLANNING_ROOT = TMP / "planning"
REPORT_DIR = TMP / "reports" / "draft_backlog_publish"
PAYLOAD_DIR = REPORT_DIR / "payloads"
REPORT_PATH = REPORT_DIR / "publish_report.json"
REPO = "doublemover/Slopjective-C"
PROGRAM_CODES = [
    "cleanup_acceleration_program",
    "runtime_envelope_completion_program",
    "post_m324_adoption_program",
]
GH_HEADERS = [
    "-H",
    "Accept: application/vnd.github+json",
    "-H",
    "X-GitHub-Api-Version: 2022-11-28",
]
SLEEP_SECONDS = 0.45
MAX_RETRIES = 5
ISSUE_CODE_RE = re.compile(r"^\[(M\d{3})\]\[Lane-[A-E]\]\[([A-Z]\d{3})\]")
GOVERNANCE_PREFLIGHT = [
    [sys.executable, "scripts/build_governance_budget_inventory_summary.py"],
    [sys.executable, "scripts/build_governance_policy_summary.py"],
    [sys.executable, "scripts/build_governance_maintainer_review_summary.py"],
    [sys.executable, "scripts/check_governance_sustainability_budget_enforcement.py"],
]


def decode(value: bytes | str | None) -> str:
    if value is None:
        return ""
    if isinstance(value, str):
        return value
    return value.decode("utf-8", errors="replace")


def run_gh(args: list[str], *, input_path: Path | None = None, expect_json: bool = True) -> Any:
    cmd = ["gh", *args]
    if input_path is not None:
        cmd.extend(["--input", str(input_path)])
    attempt = 0
    while True:
        proc = subprocess.run(
            cmd,
            cwd=ROOT,
            capture_output=True,
            text=False,
            check=False,
            timeout=180,
        )
        stdout = decode(proc.stdout)
        stderr = decode(proc.stderr)
        if proc.returncode == 0:
            if not expect_json:
                return stdout
            return json.loads(stdout) if stdout.strip() else None
        detail = (stderr or stdout or f"exit {proc.returncode}").strip()
        lowered = detail.lower()
        retryable = any(token in lowered for token in ["rate limit", "secondary rate limit", "abuse detection", "timeout", "temporar", "502", "503", "504", "429"])
        if attempt < MAX_RETRIES and retryable:
            delay = max(2.0, SLEEP_SECONDS * (2 ** attempt) * 4)
            time.sleep(delay)
            attempt += 1
            continue
        raise RuntimeError(f"gh {' '.join(args)} failed: {detail}")


def api(endpoint: str, *, method: str = "GET", payload: dict[str, Any] | None = None, paginate: bool = False) -> Any:
    args = ["api", *GH_HEADERS]
    if method != "GET":
        args.extend(["-X", method])
    if paginate:
        args.extend(["--paginate", "--slurp"])
    args.append(endpoint)
    input_path = None
    if payload is not None:
        PAYLOAD_DIR.mkdir(parents=True, exist_ok=True)
        digest = hashlib.sha1((endpoint + json.dumps(payload, sort_keys=True)).encode("utf-8")).hexdigest()[:16]
        input_path = PAYLOAD_DIR / f"{digest}.json"
        input_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return run_gh(args, input_path=input_path)


def rate_limit() -> dict[str, Any]:
    return api("rate_limit")


def slug_color(name: str) -> str:
    digest = hashlib.sha1(name.encode("utf-8")).hexdigest()
    return digest[:6]


def label_meta(name: str) -> tuple[str, str]:
    if name.startswith("lane:"):
        color = {
            "lane:A": "1d76db",
            "lane:B": "0e8a16",
            "lane:C": "fbca04",
            "lane:D": "5319e7",
            "lane:E": "d93f0b",
        }.get(name, "0052cc")
        return color, f"Backlog execution lane {name.split(':', 1)[1]}"
    if name.startswith("kind:"):
        color = {
            "kind:inventory": "bfd4f2",
            "kind:contract": "c5def5",
            "kind:artifact": "d4c5f9",
            "kind:implementation": "0e8a16",
            "kind:integration": "fbca04",
            "kind:closeout": "d93f0b",
        }.get(name, "ededed")
        return color, f"Backlog work kind {name.split(':', 1)[1]}"
    if name.startswith("priority:"):
        color = {"priority:P0": "b60205", "priority:P1": "d93f0b", "priority:P2": "fbca04"}.get(name, "cccccc")
        return color, f"Backlog priority {name.split(':', 1)[1]}"
    if name.startswith("publication:"):
        return "6e7781", f"Publication scope {name.split(':', 1)[1]}"
    if name.startswith("source:"):
        return "bfdadc", f"Backlog source {name.split(':', 1)[1]}"
    if name.startswith("type:"):
        return "c2e0c6", f"Backlog type {name.split(':', 1)[1]}"
    if name.startswith("milestone:"):
        return slug_color(name), f"Backlog milestone tag {name.split(':', 1)[1]}"
    if name.startswith("domain:"):
        return slug_color(name), f"Backlog domain {name.split(':', 1)[1]}"
    if name.startswith("area:"):
        return slug_color(name), f"Backlog area {name.split(':', 1)[1]}"
    return slug_color(name), f"Backlog label {name}"


def run_preflight_checks(report: dict[str, Any]) -> None:
    results: list[dict[str, Any]] = []
    for command in GOVERNANCE_PREFLIGHT:
        completed = subprocess.run(
            command,
            cwd=ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )
        results.append(
            {
                "command": " ".join(command),
                "exit_code": completed.returncode,
                "ok": completed.returncode == 0,
                "stdout": completed.stdout.strip(),
                "stderr": completed.stderr.strip(),
            }
        )
        if completed.returncode != 0:
            report["governance_preflight"] = results
            raise RuntimeError(f"governance preflight failed: {' '.join(command)}")
    report["governance_preflight"] = results


def load_programs() -> list[dict[str, Any]]:
    programs: list[dict[str, Any]] = []
    for code in PROGRAM_CODES:
        publish_dir = PUBLISH_ROOT / code
        planning_dir = PLANNING_ROOT / code
        manifest = json.loads((publish_dir / "program_manifest.json").read_text(encoding="utf-8"))
        for milestone in manifest["milestones"]:
            milestone_code = milestone["code"]
            milestone["full_title"] = f"{milestone_code} {milestone['title']}"
            milestone_md = planning_dir / "milestones" / f"{milestone_code}.md"
            milestone["description"] = milestone_md.read_text(encoding="utf-8")
        programs.append({
            "code": code,
            "planning_dir": planning_dir,
            "publish_dir": publish_dir,
            "manifest": manifest,
        })
    return programs


def list_existing_labels() -> dict[str, dict[str, Any]]:
    payload = api(f"repos/{REPO}/labels?per_page=100", paginate=True)
    labels: dict[str, dict[str, Any]] = {}
    for page in payload:
        for item in page:
            labels[item["name"]] = item
    return labels


def ensure_labels(programs: list[dict[str, Any]], report: dict[str, Any]) -> None:
    existing = list_existing_labels()
    needed: set[str] = set()
    for program in programs:
        for milestone in program["manifest"]["milestones"]:
            for issue in milestone["issues"]:
                needed.update(issue["label_names"])
    created: list[str] = []
    for name in sorted(needed):
        if name in existing:
            continue
        color, description = label_meta(name)
        api(f"repos/{REPO}/labels", method="POST", payload={"name": name, "color": color, "description": description})
        created.append(name)
        time.sleep(SLEEP_SECONDS)
    report["labels_created"] = created
    report["label_count"] = len(needed)


def list_all_milestones() -> list[dict[str, Any]]:
    payload = api(f"repos/{REPO}/milestones?state=all&per_page=100", paginate=True)
    milestones: list[dict[str, Any]] = []
    for page in payload:
        milestones.extend(page)
    return milestones


def milestone_code_from_title(title: str) -> str | None:
    head = title.split(" ", 1)[0]
    return head if re.fullmatch(r"M\d{3}", head) else None


def list_milestones_by_code() -> dict[str, dict[str, Any]]:
    milestones: dict[str, dict[str, Any]] = {}
    for item in list_all_milestones():
        code = milestone_code_from_title(item["title"])
        if code is not None:
            milestones[code] = item
    return milestones


def list_issues_by_code() -> dict[str, dict[str, Any]]:
    payload = api(f"repos/{REPO}/issues?state=all&per_page=100", paginate=True)
    issues: dict[str, dict[str, Any]] = {}
    for page in payload:
        for item in page:
            if "pull_request" in item:
                continue
            title = item.get("title", "")
            match = ISSUE_CODE_RE.match(title)
            if match is None:
                continue
            milestone_code = match.group(1)
            issue_short = match.group(2)
            full_code = f"{milestone_code}-{issue_short}"
            issues[full_code] = item
    return issues


def list_open_milestones() -> dict[str, dict[str, Any]]:
    payload = api(f"repos/{REPO}/milestones?state=open&per_page=100", paginate=True)
    milestones: dict[str, dict[str, Any]] = {}
    for page in payload:
        for item in page:
            milestones[item["title"]] = item
    return milestones


def list_open_issues() -> dict[str, dict[str, Any]]:
    payload = api(f"repos/{REPO}/issues?state=open&per_page=100", paginate=True)
    issues: dict[str, dict[str, Any]] = {}
    for page in payload:
        for item in page:
            if "pull_request" in item:
                continue
            issues[item["title"]] = item
    return issues


def create_or_reuse_milestones(programs: list[dict[str, Any]], report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    existing_by_code = list_milestones_by_code()
    created: list[dict[str, Any]] = []
    updated: list[dict[str, Any]] = []
    milestone_map: dict[str, dict[str, Any]] = {}
    for program in programs:
        for milestone in program["manifest"]["milestones"]:
            title = milestone["full_title"]
            code = milestone["code"]
            current = existing_by_code.get(code)
            if current is not None:
                current = api(
                    f"repos/{REPO}/milestones/{current['number']}",
                    method="PATCH",
                    payload={
                        "title": title,
                        "description": milestone["description"],
                        "state": "open",
                    },
                )
                updated.append({"code": code, "title": title, "number": current["number"], "state": current["state"]})
            else:
                current = api(
                    f"repos/{REPO}/milestones",
                    method="POST",
                    payload={"title": title, "description": milestone["description"]},
                )
                created.append({"code": code, "title": title, "number": current["number"]})
            existing_by_code[code] = current
            time.sleep(SLEEP_SECONDS)
            milestone_map[code] = current
    report["milestones_created"] = created
    report["milestones_updated"] = updated
    return milestone_map


def create_or_reuse_issues(programs: list[dict[str, Any]], milestone_map: dict[str, dict[str, Any]], report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    existing_by_code = list_issues_by_code()
    created: list[dict[str, Any]] = []
    updated: list[dict[str, Any]] = []
    issue_map: dict[str, dict[str, Any]] = {}
    for program in programs:
        for milestone in program["manifest"]["milestones"]:
            milestone_number = milestone_map[milestone["code"]]["number"]
            for issue in milestone["issues"]:
                title = issue["title"]
                code = issue["code"]
                current = existing_by_code.get(code)
                payload = {
                    "title": title,
                    "body": issue["body"],
                    "milestone": milestone_number,
                    "labels": issue["label_names"],
                    "state": "open",
                }
                if current is not None:
                    current = api(
                        f"repos/{REPO}/issues/{current['number']}",
                        method="PATCH",
                        payload=payload,
                    )
                    updated.append({"code": code, "number": current["number"], "title": title, "state": current["state"]})
                else:
                    current = api(
                        f"repos/{REPO}/issues",
                        method="POST",
                        payload=payload,
                    )
                    created.append({"code": code, "number": current["number"], "title": title})
                existing_by_code[code] = current
                time.sleep(SLEEP_SECONDS)
                issue_map[code] = current
    report["issues_created"] = created
    report["issues_updated"] = updated
    report["issue_count"] = len(issue_map)
    return issue_map


def ensure_dependencies(programs: list[dict[str, Any]], issue_map: dict[str, dict[str, Any]], report: dict[str, Any]) -> None:
    created: list[dict[str, Any]] = []
    skipped: list[dict[str, Any]] = []
    for program in programs:
        for milestone in program["manifest"]["milestones"]:
            for issue in milestone["issues"]:
                target = issue_map[issue["code"]]
                for blocker_code in issue["blocked_by_issue_codes"]:
                    blocker = issue_map[blocker_code]
                    try:
                        api(
                            f"repos/{REPO}/issues/{target['number']}/dependencies/blocked_by",
                            method="POST",
                            payload={"issue_id": blocker["id"]},
                        )
                        created.append({"issue": issue["code"], "blocked_by": blocker_code})
                        time.sleep(SLEEP_SECONDS)
                    except RuntimeError as exc:
                        message = str(exc).lower()
                        if "already exists" in message or "unprocessable" in message or "422" in message:
                            skipped.append({"issue": issue["code"], "blocked_by": blocker_code, "reason": str(exc)})
                            continue
                        raise
    report["dependencies_created"] = created
    report["dependencies_skipped"] = skipped


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    PAYLOAD_DIR.mkdir(parents=True, exist_ok=True)
    programs = load_programs()
    report: dict[str, Any] = {
        "generated_at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "repo": REPO,
        "program_codes": PROGRAM_CODES,
        "rate_limit_before": rate_limit(),
    }
    run_preflight_checks(report)
    ensure_labels(programs, report)
    milestone_map = create_or_reuse_milestones(programs, report)
    issue_map = create_or_reuse_issues(programs, milestone_map, report)
    ensure_dependencies(programs, issue_map, report)
    report["rate_limit_after"] = rate_limit()
    report["milestones"] = {code: {"title": item["title"], "number": item["number"]} for code, item in milestone_map.items()}
    report["issues"] = {code: {"title": item["title"], "number": item["number"], "id": item["id"]} for code, item in issue_map.items()}
    REPORT_PATH.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({
        "report": str(REPORT_PATH.relative_to(ROOT)).replace('\\', '/'),
        "milestones": len(milestone_map),
        "issues": len(issue_map),
        "dependencies_created": len(report["dependencies_created"]),
    }, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
