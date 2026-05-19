#!/usr/bin/env python3
"""Validate the bounded getting-started tutorial surface against the live docs and showcase examples."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
WALKTHROUGH_PATH = ROOT / "showcase" / "tutorial_walkthrough.json"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
SHOWCASE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "showcase" / "summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "tutorials" / "getting-started-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.tutorial.getting-started.surface.summary.v1"
RUNNER_PATH = "scripts/check_getting_started_surface.py"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    walkthrough = load_json(WALKTHROUGH_PATH)
    program_surface = load_json(PROGRAM_SURFACE_PATH)
    expect(
        walkthrough.get("contract_id") == "objc3c.showcase.tutorial.walkthrough.v1",
        "tutorial walkthrough contract drifted",
    )

    steps = walkthrough.get("steps")
    expect(isinstance(steps, list), "tutorial walkthrough did not publish steps")
    compile_steps = [
        step
        for step in steps
        if isinstance(step, dict) and str(step.get("public_entrypoint")) == "compile:objc3c"
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
        "child_report_paths": [repo_rel(SHOWCASE_SUMMARY_PATH)],
        "showcase_surface_summary": showcase_summary,
    }
    write_report_json(REPORT_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
