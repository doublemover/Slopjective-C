#!/usr/bin/env python3
"""Fail-closed task-hygiene surface checks for the post-compatibility command model."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"
REGISTRY_JSON = ROOT / "spec" / "governance" / "objc3c_task_hygiene_registry.json"

REMOVED_FAMILY_PATTERNS = (
    r"^check:objc3c:m",
    r"^test:tooling:m",
    r"^check:compiler-closeout:m",
    r"^run:objc3c:",
    r"^plan:compiler-dispatch:",
    r"^refresh:compiler-dispatch:",
    r"^dev:objc3c:",
)


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _read_package_scripts() -> dict[str, str]:
    payload = _read_json(PACKAGE_JSON)
    scripts = payload.get("scripts")
    if not isinstance(scripts, dict):
        raise ValueError("package.json scripts field must be an object")
    return {str(key): str(value) for key, value in scripts.items()}


def _removed_aliases_present(scripts: dict[str, str]) -> list[str]:
    hits: list[str] = []
    for script_name in scripts:
        if any(re.match(pattern, script_name) for pattern in REMOVED_FAMILY_PATTERNS):
            hits.append(script_name)
    return sorted(hits)


def _check_package_contracts(scripts: dict[str, str], registry: dict) -> list[str]:
    errors: list[str] = []
    surface = registry

    if scripts.get("check:task-hygiene") != "python scripts/ci/run_task_hygiene_gate.py":
        errors.append("package.json scripts.check:task-hygiene must route directly to python scripts/ci/run_task_hygiene_gate.py")

    removed_hits = _removed_aliases_present(scripts)
    if removed_hits:
        errors.append(f"package.json still exposes removed alias families: {removed_hits}")

    if len(scripts) > 25:
        errors.append(f"package.json scripts count exceeds cleanup budget: {len(scripts)} > 25")

    sequence = surface.get("sequence", [])
    if not isinstance(sequence, list) or not sequence:
        errors.append("task hygiene registry must publish a non-empty sequence")
        return errors

    required_sources = {
        "check:planning-hygiene",
        "check:compiler-closeout:m155",
        "check:compiler-closeout:m156",
        "check:compiler-closeout:m157",
        "check:compiler-closeout:m158",
        "check:compiler-closeout:m159",
        "check:compiler-closeout:m160",
        "check:catalog-status-integrity",
        "check:catalog-status-metadata",
        "check:open-blocker-audit:repo-root:fixtures",
        "extract:open-issues",
        "check:issue-drift",
    }
    seen_sources = {entry.get("source_script") for entry in sequence if isinstance(entry, dict)}
    missing_sources = sorted(required_sources - seen_sources)
    if missing_sources:
        errors.append(f"task hygiene registry missing required entries: {missing_sources}")

    forbidden_tokens = (
        "npm run check:compiler-closeout:",
        "npm run check:objc3c:m",
        "npm run test:tooling:m",
        "npm run run:objc3c:",
        "npm run plan:compiler-dispatch:",
        "npm run refresh:compiler-dispatch:",
        "npm run dev:objc3c:",
    )
    for entry in sequence:
        if not isinstance(entry, dict):
            errors.append("task hygiene registry entries must be objects")
            continue
        command = entry.get("command")
        if not isinstance(command, str) or not command.strip():
            errors.append(f"task hygiene registry entry {entry!r} must publish a non-empty command string")
            continue
        for forbidden in forbidden_tokens:
            if forbidden in command:
                errors.append(f"task hygiene registry command still depends on removed alias family: {forbidden}")
    return errors


def _check_workflow_contracts(workflow_text: str) -> list[str]:
    errors: list[str] = []
    if "npm run check:objc3c:library-cli-parity:source:m144" in workflow_text:
        errors.append(".github/workflows/task-hygiene.yml must not invoke removed package alias check:objc3c:library-cli-parity:source:m144")
    if "python scripts/check_objc3c_library_cli_parity.py --source tests/tooling/fixtures/native/hello.objc3" not in workflow_text:
        errors.append(".github/workflows/task-hygiene.yml must directly invoke the library CLI parity source check")
    return errors


def main() -> int:
    scripts = _read_package_scripts()
    registry = _read_json(REGISTRY_JSON)
    workflow_text = TASK_HYGIENE_WORKFLOW.read_text(encoding="utf-8")

    errors = _check_package_contracts(scripts, registry)
    errors.extend(_check_workflow_contracts(workflow_text))

    if errors:
        print("task-hygiene contract check failed:")
        for error in errors:
            print(f"- {error}")
        return 1

    print("task-hygiene contract check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
