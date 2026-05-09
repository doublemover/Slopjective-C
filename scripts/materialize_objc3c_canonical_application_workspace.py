#!/usr/bin/env python3
"""Materialize the canonical application workspace from showcase and stdlib sources."""

from __future__ import annotations

import argparse
import json
import shutil
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "canonical_application_architecture_semantics.json"
PORTFOLIO_PATH = ROOT / "showcase" / "portfolio.json"
STDLIB_PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
DEFAULT_OUTPUT_ROOT = ROOT / "tmp" / "artifacts" / "application-architecture-testing" / "canonical-workspace"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "canonical-application-workspace-summary.json"
WORKSPACE_CONTRACT_ID = "objc3c.application.architecture.testing.canonical_workspace.v1"




def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--out-dir",
        default="",
        help="Target output directory. Defaults to tmp/artifacts/application-architecture-testing/canonical-workspace.",
    )
    return parser.parse_args()


def resolve_out_dir(raw_out_dir: str) -> Path:
    if raw_out_dir:
        candidate = Path(raw_out_dir)
        if not candidate.is_absolute():
            candidate = ROOT / candidate
        return candidate.resolve()
    return DEFAULT_OUTPUT_ROOT.resolve()


def main() -> int:
    args = parse_args()
    contract = load_json(CONTRACT_PATH)
    portfolio = load_json(PORTFOLIO_PATH)
    stdlib_program_surface = load_json(STDLIB_PROGRAM_SURFACE_PATH)
    output_root = resolve_out_dir(args.out_dir)
    if output_root.exists():
        shutil.rmtree(output_root)
    output_root.mkdir(parents=True, exist_ok=True)

    copied_paths: list[str] = []
    included_examples: list[dict[str, object]] = []
    for entry in portfolio.get("examples", []):
        if not isinstance(entry, dict):
            continue
        example_id = str(entry["id"])
        source_path = ROOT / str(entry["source"])
        workspace_manifest = ROOT / str(entry["workspace_manifest"])
        destination_root = output_root / "examples" / example_id
        destination_root.mkdir(parents=True, exist_ok=True)
        copied_source = destination_root / "main.objc3"
        copied_workspace = destination_root / "workspace.json"
        shutil.copy2(source_path, copied_source)
        shutil.copy2(workspace_manifest, copied_workspace)
        copied_paths.extend(
            [
                repo_rel(source_path),
                repo_rel(workspace_manifest),
            ]
        )
        included_examples.append(
            {
                "id": example_id,
                "source": repo_rel(copied_source),
                "workspace_manifest": repo_rel(copied_workspace),
                "story_capabilities": entry.get("story_capabilities", []),
                "stdlib_followup_modules": entry.get("stdlib_followup_modules", []),
            }
        )

    copied_stdlib_workspace = output_root / "stdlib" / "workspace.json"
    copied_stdlib_program_surface = output_root / "stdlib" / "program_surface.json"
    copied_stdlib_workspace.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(ROOT / "stdlib" / "workspace.json", copied_stdlib_workspace)
    shutil.copy2(STDLIB_PROGRAM_SURFACE_PATH, copied_stdlib_program_surface)
    copied_paths.extend(["stdlib/workspace.json", "stdlib/program_surface.json"])

    readme_path = output_root / "README.md"
    readme_path.write_text(
        "\n".join(
            [
                "# Canonical Application Workspace",
                "",
                "This machine-owned workspace packages the checked-in showcase and stdlib program surfaces into one copyable application architecture root.",
                "",
                "Replay commands:",
                "- `python scripts/materialize_objc3c_canonical_application_workspace.py`",
                "- `python scripts/check_application_architecture_template_harness.py`",
                "- `npm run objc3c -- validate-showcase`",
                "- `npm run objc3c -- validate-runnable-showcase`",
                "- `npm run objc3c -- validate-stdlib-program`",
                "- `npm run objc3c -- validate-runnable-stdlib-program`",
                "- `npm run objc3c -- package-runnable-toolchain`",
            ]
        )
        + "\n",
        encoding="utf-8",
    )

    workspace_manifest = output_root / "workspace.json"
    workspace_manifest.write_text(
        json.dumps(
            {
                "contract_id": WORKSPACE_CONTRACT_ID,
                "schema_version": 1,
                "canonical_application_architecture_contract": repo_rel(CONTRACT_PATH),
                "portfolio_manifest": repo_rel(PORTFOLIO_PATH),
                "stdlib_program_surface": repo_rel(STDLIB_PROGRAM_SURFACE_PATH),
                "workspace_root": repo_rel(output_root),
                "readme": repo_rel(readme_path),
                "architecture_layers": contract["architecture_layers"],
                "included_examples": included_examples,
                "public_actions": contract["required_evidence_actions"],
                "package_bridge": "objc3c",
                "copied_roots": [
                    "examples",
                    "stdlib",
                ],
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

    payload = {
        "contract_id": "objc3c.application.architecture.testing.canonical_workspace.summary.v1",
        "status": "PASS",
        "workspace_contract_id": WORKSPACE_CONTRACT_ID,
        "canonical_application_architecture_contract": repo_rel(CONTRACT_PATH),
        "workspace_root": repo_rel(output_root),
        "workspace_manifest": repo_rel(workspace_manifest),
        "readme": repo_rel(readme_path),
        "example_count": len(included_examples),
        "architecture_layer_count": len(contract["architecture_layers"]),
        "copied_path_count": len(copied_paths),
        "included_examples": included_examples,
        "public_actions": contract["required_evidence_actions"],
        "package_bridge": "objc3c",
        "stdlib_publish_model": stdlib_program_surface.get("publish_model"),
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"workspace_root: {repo_rel(output_root)}")
    print(f"workspace_path: {repo_rel(workspace_manifest)}")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
