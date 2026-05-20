#!/usr/bin/env python3
"""Validate showcase integration through the live checked-in compile and runtime paths."""

from __future__ import annotations

import json
import shutil
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
DEMO_PACKAGES_PATH = ROOT / "showcase" / "demo_packages.json"
SURFACE_REPORT = ROOT / "tmp" / "reports" / "showcase" / "summary.json"
RUNTIME_REPORT = ROOT / "tmp" / "reports" / "showcase" / "runtime-summary.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.showcase.integration.summary.v1"
EXPECTED_EXAMPLE_IDS = ["auroraBoard", "signalMesh", "patchKit"]
EXPECTED_DEMO_PACKAGE_IDS = [
    "showcase:auroraBoard",
    "showcase:signalMesh",
    "showcase:patchKit",
]
EXPECTED_COVERAGE_DOMAINS = ["object-model", "concurrency", "interop"]


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    surface_result = run_capture(python_script_command(SHOWCASE_SURFACE_PY))
    if surface_result.returncode != 0:
        raise RuntimeError("showcase surface validation failed")

    runtime_result = run_capture(
        [PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(SHOWCASE_RUNTIME_PS1)]
    )
    if runtime_result.returncode != 0:
        raise RuntimeError("showcase runtime validation failed")

    surface_summary = load_json(SURFACE_REPORT)
    runtime_summary = load_json(RUNTIME_REPORT)
    program_surface = load_json(PROGRAM_SURFACE_PATH)
    demo_packages = load_json(DEMO_PACKAGES_PATH)
    expect(
        surface_summary.get("contract_id") == "objc3c.showcase.surface.summary.v1",
        "showcase surface report published the wrong contract id",
    )
    expect(
        runtime_summary.get("contract_id") == "objc3c.showcase.runtime.summary.v1",
        "showcase runtime report published the wrong contract id",
    )

    selected_ids = surface_summary.get("selected_example_ids")
    expect(selected_ids == EXPECTED_EXAMPLE_IDS, "showcase surface report drifted from the full example set")
    expect(
        surface_summary.get("demo_packages_contract_id") == "objc3c.showcase.demo.packages.v1",
        "showcase surface report drifted from the demo package contract",
    )
    expect(
        surface_summary.get("demo_package_ids") == EXPECTED_DEMO_PACKAGE_IDS,
        "showcase surface demo package ids drifted",
    )
    expect(
        surface_summary.get("demo_package_coverage_domains") == EXPECTED_COVERAGE_DOMAINS,
        "showcase surface demo package coverage drifted",
    )
    program_examples = program_surface.get("capability_demo_examples")
    expect(isinstance(program_examples, list), "program surface did not publish capability_demo_examples")
    program_examples_by_id = {
        str(entry.get("id")): entry for entry in program_examples if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    expect(list(program_examples_by_id) == EXPECTED_EXAMPLE_IDS, "program surface example ids drifted from showcase integration")

    surface_examples = surface_summary.get("examples")
    expect(isinstance(surface_examples, list), "showcase surface report did not publish examples")
    for entry in surface_examples:
        expect(isinstance(entry, dict), "showcase surface report published malformed example")
        example_id = str(entry.get("example_id"))
        program_entry = program_examples_by_id.get(example_id)
        expect(program_entry is not None, f"showcase surface report referenced unknown program example {example_id}")
        expect(
            entry.get("story_capabilities") == program_entry.get("story_capabilities"),
            f"showcase surface story_capabilities drifted for {example_id}",
        )
        expect(
            entry.get("stdlib_followup_modules") == program_entry.get("stdlib_followup_modules"),
            f"showcase surface stdlib_followup_modules drifted for {example_id}",
        )
        demo_package = entry.get("demo_package")
        expect(isinstance(demo_package, dict), f"showcase surface demo package missing for {example_id}")
        assert isinstance(demo_package, dict)
        expect(
            demo_package.get("package_id") == f"showcase:{example_id}",
            f"showcase surface demo package id drifted for {example_id}",
        )
        expect(
            demo_package.get("coverage_domain") in EXPECTED_COVERAGE_DOMAINS,
            f"showcase surface demo package coverage drifted for {example_id}",
        )

    runtime_examples = runtime_summary.get("examples")
    expect(isinstance(runtime_examples, list), "showcase runtime report did not publish examples")
    runtime_ids = [str(entry.get("example_id")) for entry in runtime_examples if isinstance(entry, dict)]
    expect(runtime_ids == EXPECTED_EXAMPLE_IDS, "showcase runtime report drifted from the full example set")

    actual_exit_codes = {
        str(entry.get("example_id")): entry.get("actual_exit_code")
        for entry in runtime_examples
        if isinstance(entry, dict)
    }
    expect(
        actual_exit_codes == {"auroraBoard": 33, "signalMesh": 13, "patchKit": 7},
        "showcase runtime report drifted from the expected runnable exits",
    )
    for entry in surface_examples:
        if not isinstance(entry, dict):
            continue
        demo_package = entry.get("demo_package", {})
        if not isinstance(demo_package, dict):
            continue
        example_id = str(entry.get("example_id"))
        expect(
            demo_package.get("expected_exit_code") == actual_exit_codes.get(example_id),
            f"demo package expected exit drifted from runtime proof for {example_id}",
        )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_showcase_integration.py",
        "example_ids": EXPECTED_EXAMPLE_IDS,
        "child_report_paths": [repo_rel(SURFACE_REPORT), repo_rel(RUNTIME_REPORT)],
        "program_surface_contract": repo_rel(PROGRAM_SURFACE_PATH),
        "demo_packages_manifest": repo_rel(DEMO_PACKAGES_PATH),
        "demo_packages": demo_packages.get("packages"),
        "program_publish_inputs": program_surface.get("publish_inputs"),
        "capability_demo_examples": program_examples,
        "showcase_surface_summary": surface_summary,
        "showcase_runtime_summary": runtime_summary,
    }

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
