#!/usr/bin/env python3
"""Validate the bounded getting-started tutorial surface against the live docs and showcase examples."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
import shlex
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.subprocesses import python_script_command, run_capture
try:
    from scripts.objc3c_workflow.public_command_api import public_workflow_action_names
except ModuleNotFoundError:
    from objc3c_workflow.public_command_api import public_workflow_action_names


ROOT = Path(__file__).resolve().parents[1]
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
WALKTHROUGH_PATH = ROOT / "showcase" / "tutorial_walkthrough.json"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
FENCED_COMMAND_SURFACE_PATHS = (
    ROOT / "README.md",
    ROOT / "docs" / "tutorials" / "README.md",
    ROOT / "docs" / "tutorials" / "getting_started.md",
    ROOT / "docs" / "tutorials" / "build_run_verify.md",
    ROOT / "docs" / "tutorials" / "guided_walkthrough.md",
    ROOT / "docs" / "tutorials" / "objc2_to_objc3_migration.md",
    ROOT / "docs" / "tutorials" / "objc2_swift_cpp_comparison.md",
    ROOT / "showcase" / "README.md",
)
SHOWCASE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "showcase" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.tutorial.getting-started.surface.summary.v1"
RUNNER_PATH = "scripts/check_getting_started_surface.py"
PUBLIC_COMMAND_PREFIX = ("npm", "run", "objc3c", "--")


@dataclass(frozen=True)
class PublicCommandRecord:
    source_path: str
    line: int | None
    command: str
    action: str
    source_kind: str


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def _strip_shell_prompt(line: str) -> str:
    stripped = line.strip()
    if stripped.startswith("$ "):
        return stripped[2:].strip()
    if stripped.startswith("PS> "):
        return stripped[4:].strip()
    return stripped


def _parse_public_command(command: str) -> tuple[str, list[str]]:
    tokens = shlex.split(_strip_shell_prompt(command), posix=False)
    if len(tokens) < len(PUBLIC_COMMAND_PREFIX) + 1:
        raise ValueError("command does not include a workflow action")
    if tuple(tokens[: len(PUBLIC_COMMAND_PREFIX)]) != PUBLIC_COMMAND_PREFIX:
        raise ValueError("command does not use the canonical npm workflow prefix")
    action = tokens[len(PUBLIC_COMMAND_PREFIX)]
    if action == "<action>":
        return action, tokens
    if action.startswith("-"):
        raise ValueError(f"command action is not a public workflow action: {action}")
    return action, tokens


def _iter_fenced_command_lines(path: Path) -> list[tuple[int, str]]:
    in_fence = False
    commands: list[tuple[int, str]] = []
    for line_number, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        stripped = raw_line.strip()
        if stripped.startswith("```"):
            in_fence = not in_fence
            continue
        if not in_fence or "npm run objc3c" not in stripped:
            continue
        command = _strip_shell_prompt(stripped)
        if not command.startswith("npm run objc3c"):
            raise RuntimeError(f"{repo_rel(path)}:{line_number} has a non-copyable objc3c command: {stripped}")
        commands.append((line_number, command))
    return commands


def extract_fenced_public_command_records(paths: Sequence[Path]) -> list[PublicCommandRecord]:
    records: list[PublicCommandRecord] = []
    for path in paths:
        for line_number, command in _iter_fenced_command_lines(path):
            action, _tokens = _parse_public_command(command)
            if action == "<action>":
                continue
            records.append(
                PublicCommandRecord(
                    source_path=repo_rel(path),
                    line=line_number,
                    command=command,
                    action=action,
                    source_kind="fenced-doc-command",
                )
            )
    return records


def extract_walkthrough_public_command_records(walkthrough: dict[str, Any]) -> list[PublicCommandRecord]:
    steps = walkthrough.get("steps")
    expect(isinstance(steps, list), "tutorial walkthrough did not publish steps")
    records: list[PublicCommandRecord] = []
    for index, step in enumerate(steps):
        expect(isinstance(step, dict), f"tutorial walkthrough step {index} is malformed")
        command = step.get("public_command")
        expect(isinstance(command, str), f"tutorial walkthrough step {index} did not publish public_command")
        action, tokens = _parse_public_command(command)
        expect(action != "<action>", f"tutorial walkthrough step {index} used the placeholder public action")
        expect(
            len(tokens) <= len(PUBLIC_COMMAND_PREFIX) + 1 or tokens[len(PUBLIC_COMMAND_PREFIX) + 1] != "--",
            f"tutorial walkthrough step {index} has an extra separator after the workflow action",
        )
        records.append(
            PublicCommandRecord(
                source_path=repo_rel(WALKTHROUGH_PATH),
                line=None,
                command=command,
                action=action,
                source_kind="walkthrough-public-command",
            )
        )
    return records


def validate_public_command_parity(records: Sequence[PublicCommandRecord]) -> dict[str, Any]:
    registered_actions = set(public_workflow_action_names())
    missing = sorted(
        {
            record.action
            for record in records
            if record.action not in registered_actions
        }
    )
    if missing:
        sources = [
            {
                "source_path": record.source_path,
                "line": record.line,
                "command": record.command,
                "action": record.action,
                "source_kind": record.source_kind,
            }
            for record in records
            if record.action in missing
        ]
        raise RuntimeError(f"getting-started public commands reference unregistered workflow actions: {sources}")
    return {
        "status": "PASS",
        "registered_action_count": len(registered_actions),
        "checked_command_count": len(records),
        "checked_actions": sorted({record.action for record in records}),
        "checked_commands": [
            {
                "source_path": record.source_path,
                "line": record.line,
                "command": record.command,
                "action": record.action,
                "source_kind": record.source_kind,
            }
            for record in records
        ],
    }


def main() -> int:
    walkthrough = load_json(WALKTHROUGH_PATH)
    program_surface = load_json(PROGRAM_SURFACE_PATH)
    expect(
        walkthrough.get("contract_id") == "objc3c.showcase.tutorial.walkthrough.v1",
        "tutorial walkthrough contract drifted",
    )

    steps = walkthrough.get("steps")
    expect(isinstance(steps, list), "tutorial walkthrough did not publish steps")
    public_command_records = [
        *extract_fenced_public_command_records(FENCED_COMMAND_SURFACE_PATHS),
        *extract_walkthrough_public_command_records(walkthrough),
    ]
    public_command_parity = validate_public_command_parity(public_command_records)
    compile_steps = [
        step
        for step in steps
        if isinstance(step, dict)
        and (
            str(step.get("workflow_action")) == "compile-objc3c"
            or str(step.get("public_entrypoint")) == "compile:objc3c"
        )
    ]
    example_ids = [str(step.get("example_id")) for step in compile_steps]
    expect(
        example_ids == ["auroraBoard", "signalMesh", "patchKit"],
        "tutorial walkthrough compile example set drifted",
    )
    program_examples = program_surface.get("capability_demo_examples")
    expect(isinstance(program_examples, list), "program surface did not publish capability_demo_examples")
    program_examples_by_id = {
        str(entry.get("id")): entry for entry in program_examples if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    expect(list(program_examples_by_id) == example_ids, "program surface example ids drifted from getting-started walkthrough")

    documentation_result = run_capture(python_script_command(DOCUMENTATION_SURFACE_PY))
    if documentation_result.returncode != 0:
        raise RuntimeError("documentation surface validation failed")

    showcase_command = python_script_command(SHOWCASE_SURFACE_PY)
    for example_id in example_ids:
        showcase_command.extend(["--example", example_id])
    showcase_result = run_capture(showcase_command)
    if showcase_result.returncode != 0:
        raise RuntimeError("showcase surface validation failed for getting-started examples")

    showcase_summary = load_json(SHOWCASE_SUMMARY_PATH)
    expect(
        showcase_summary.get("contract_id") == "objc3c.showcase.surface.summary.v1",
        "showcase summary contract drifted",
    )
    expect(
        showcase_summary.get("selected_example_ids") == example_ids,
        "showcase summary drifted from the getting-started example set",
    )
    showcase_examples = showcase_summary.get("examples")
    expect(isinstance(showcase_examples, list), "showcase summary did not publish examples")
    for entry in showcase_examples:
        expect(isinstance(entry, dict), "showcase summary published malformed example")
        example_id = str(entry.get("example_id"))
        program_entry = program_examples_by_id.get(example_id)
        expect(program_entry is not None, f"showcase summary referenced unknown program example {example_id}")
        expect(
            entry.get("story_capabilities") == program_entry.get("story_capabilities"),
            f"getting-started showcase story_capabilities drifted for {example_id}",
        )
        expect(
            entry.get("stdlib_followup_modules") == program_entry.get("stdlib_followup_modules"),
            f"getting-started showcase stdlib_followup_modules drifted for {example_id}",
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "walkthrough_manifest": repo_rel(WALKTHROUGH_PATH),
        "program_surface_contract": repo_rel(PROGRAM_SURFACE_PATH),
        "program_publish_inputs": program_surface.get("publish_inputs"),
        "capability_demo_examples": program_examples,
        "example_ids": example_ids,
        "public_command_parity": public_command_parity,
        "child_report_paths": [repo_rel(SHOWCASE_SUMMARY_PATH)],
        "showcase_surface_summary": showcase_summary,
    }
    write_report_json(REPORT_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
