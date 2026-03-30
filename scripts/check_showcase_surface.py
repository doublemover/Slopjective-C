#!/usr/bin/env python3
"""Validate the live showcase example surface through the public compiler path."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
PORTFOLIO = ROOT / "showcase" / "portfolio.json"
GUIDED_WALKTHROUGH = ROOT / "showcase" / "tutorial_walkthrough.json"
WORKSPACE_CONTRACT_ID = "objc3c.showcase.example.workspace.v1"
GUIDED_WALKTHROUGH_CONTRACT_ID = "objc3c.showcase.tutorial.walkthrough.v1"
MODULE_DECL_RE = re.compile(r"^\s*module\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", re.MULTILINE)
SHOWCASE_SUMMARY_CONTRACT_ID = "objc3c.showcase.surface.summary.v1"


def fail(message: str) -> int:
    print(f"showcase-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def run(command: list[str]) -> int:
    result = subprocess.run(command, cwd=ROOT, check=False)
    return result.returncode


def repo_relative(path: Path) -> str:
    return path.resolve().relative_to(ROOT).as_posix()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate the live showcase example surface through the public compiler path."
    )
    parser.add_argument(
        "--example",
        action="append",
        default=[],
        help="Compile only the named showcase example id. Repeatable.",
    )
    parser.add_argument(
        "--capability",
        action="append",
        default=[],
        help="Compile only showcase examples advertising the named story capability. Repeatable.",
    )
    return parser.parse_args(argv)


def main() -> int:
    args = parse_args(sys.argv[1:])
    if not PORTFOLIO.is_file():
        return fail(f"missing showcase portfolio: {PORTFOLIO}")

    payload = json.loads(PORTFOLIO.read_text(encoding="utf-8"))
    if payload.get("contract_id") != "objc3c.showcase.portfolio.surface.v1":
        return fail("unexpected contract_id")
    if payload.get("schema_version") != 1:
        return fail("unexpected schema_version")
    if payload.get("showcase_root") != "showcase":
        return fail("showcase_root drifted")
    if payload.get("machine_output_root") != "tmp/artifacts/showcase":
        return fail("machine_output_root drifted")
    if payload.get("machine_report_root") != "tmp/reports/showcase":
        return fail("machine_report_root drifted")
    if payload.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return fail("package_stage_root drifted")

    entrypoints = payload.get("public_entrypoints")
    if entrypoints != {
        "build_native": "build:objc3c-native",
        "compile_example": "compile:objc3c",
        "check_surface": "check:showcase:surface",
        "validate_showcase": "test:showcase",
        "validate_runnable_showcase": "test:showcase:e2e",
        "package_runnable_toolchain": "package:objc3c-native:runnable-toolchain",
        "execution_smoke": "test:objc3c:execution-smoke",
        "execution_replay": "test:objc3c:execution-replay-proof",
    }:
        return fail("public_entrypoints drifted")

    build_run_package_surface = payload.get("build_run_package_surface")
    if build_run_package_surface != {
        "emit_prefix": "module",
        "workspace_layout": "checked-in showcase directories rooted at showcase/<example-id>",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
        "build_native_entrypoint": "build:objc3c-native",
        "compile_entrypoint": "compile:objc3c",
        "surface_check_entrypoint": "check:showcase:surface",
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
        "package_entrypoint": "package:objc3c-native:runnable-toolchain",
        "execution_smoke_entrypoint": "test:objc3c:execution-smoke",
        "execution_replay_entrypoint": "test:objc3c:execution-replay-proof",
    }:
        return fail("build_run_package_surface drifted")
    tutorial_build_run_verify_surface = payload.get("tutorial_build_run_verify_surface")
    if tutorial_build_run_verify_surface != {
        "getting_started_readme": "docs/tutorials/getting_started.md",
        "build_run_verify_readme": "docs/tutorials/build_run_verify.md",
        "migration_readme": "docs/tutorials/objc2_to_objc3_migration.md",
        "build_native_entrypoint": "build:objc3c-native",
        "compile_entrypoint": "compile:objc3c",
        "surface_check_entrypoint": "check:showcase:surface",
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
        "artifact_root": "tmp/artifacts/showcase",
        "report_root": "tmp/reports/showcase",
        "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
    }:
        return fail("tutorial_build_run_verify_surface drifted")
    if payload.get("guided_walkthrough_manifest") != "showcase/tutorial_walkthrough.json":
        return fail("guided_walkthrough_manifest drifted")
    runtime_presentation_surface = payload.get("runtime_presentation_surface")
    if runtime_presentation_surface != {
        "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
        "runtime_library_resolution_model": "registration-manifest-runtime-archive-path-is-authoritative",
        "driver_linker_flag_consumption_model": "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands",
        "integrated_validation_entrypoint": "test:showcase",
        "packaged_validation_entrypoint": "test:showcase:e2e",
        "shared_execution_smoke_entrypoint": "test:objc3c:execution-smoke",
        "shared_execution_replay_entrypoint": "test:objc3c:execution-replay-proof",
        "presentation_readme": "showcase/README.md",
    }:
        return fail("runtime_presentation_surface drifted")

    examples = payload.get("examples")
    if not isinstance(examples, list) or len(examples) != 3:
        return fail("examples inventory drifted")

    ids = [entry.get("id") for entry in examples if isinstance(entry, dict)]
    if ids != ["auroraBoard", "signalMesh", "patchKit"]:
        return fail("example ids drifted")

    if not GUIDED_WALKTHROUGH.is_file():
        return fail(f"missing guided walkthrough manifest: {repo_relative(GUIDED_WALKTHROUGH)}")
    walkthrough_payload = json.loads(GUIDED_WALKTHROUGH.read_text(encoding="utf-8"))
    if walkthrough_payload.get("contract_id") != GUIDED_WALKTHROUGH_CONTRACT_ID:
        return fail("guided walkthrough contract_id drifted")
    if walkthrough_payload.get("schema_version") != 1:
        return fail("guided walkthrough schema_version drifted")
    if walkthrough_payload.get("tutorial_readme") != "docs/tutorials/guided_walkthrough.md":
        return fail("guided walkthrough tutorial_readme drifted")
    if walkthrough_payload.get("build_run_verify_readme") != "docs/tutorials/build_run_verify.md":
        return fail("guided walkthrough build_run_verify_readme drifted")
    if walkthrough_payload.get("portfolio_contract") != "showcase/portfolio.json":
        return fail("guided walkthrough portfolio_contract drifted")
    walkthrough_steps = walkthrough_payload.get("steps")
    if walkthrough_steps != [
        {
            "id": "build-native",
            "public_entrypoint": "build:objc3c-native",
        },
        {
            "id": "compile-auroraBoard",
            "public_entrypoint": "compile:objc3c",
            "example_id": "auroraBoard",
            "source": "showcase/auroraBoard/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/auroraBoard",
        },
        {
            "id": "compile-signalMesh",
            "public_entrypoint": "compile:objc3c",
            "example_id": "signalMesh",
            "source": "showcase/signalMesh/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/signalMesh",
        },
        {
            "id": "compile-patchKit",
            "public_entrypoint": "compile:objc3c",
            "example_id": "patchKit",
            "source": "showcase/patchKit/main.objc3",
            "artifact_root": "tmp/artifacts/showcase/patchKit",
        },
        {
            "id": "check-showcase-surface",
            "public_entrypoint": "check:showcase:surface",
            "report_root": "tmp/reports/showcase",
        },
        {
            "id": "validate-showcase",
            "public_entrypoint": "test:showcase",
            "report_root": "tmp/reports/showcase",
        },
    ]:
        return fail("guided walkthrough steps drifted")

    requested_ids = set(args.example)
    if requested_ids:
        unknown_ids = sorted(requested_ids.difference(ids))
        if unknown_ids:
            return fail(f"unknown showcase example ids: {', '.join(unknown_ids)}")

    requested_capabilities = set(args.capability)
    known_capabilities = {
        capability
        for entry in examples
        if isinstance(entry, dict)
        for capability in entry.get("story_capabilities", [])
        if isinstance(capability, str)
    }
    if requested_capabilities:
        unknown_capabilities = sorted(requested_capabilities.difference(known_capabilities))
        if unknown_capabilities:
            return fail(
                f"unknown showcase capabilities: {', '.join(unknown_capabilities)}"
            )

    selected_examples: list[dict[str, object]] = []
    for entry in examples:
        if not isinstance(entry, dict):
            return fail("example entry must be an object")
        example_id = entry.get("id")
        if not isinstance(example_id, str):
            return fail("example entry missing id/source")
        workspace_manifest = entry.get("workspace_manifest")
        if not isinstance(workspace_manifest, str):
            return fail(f"example entry missing workspace_manifest for {example_id}")
        capabilities = {
            capability
            for capability in entry.get("story_capabilities", [])
            if isinstance(capability, str)
        }
        if requested_ids and example_id not in requested_ids:
            continue
        if requested_capabilities and not (capabilities & requested_capabilities):
            continue
        workspace_manifest_path = ROOT / workspace_manifest
        if not workspace_manifest_path.is_file():
            return fail(f"missing showcase workspace manifest: {workspace_manifest}")
        workspace_payload = json.loads(workspace_manifest_path.read_text(encoding="utf-8"))
        if workspace_payload.get("contract_id") != WORKSPACE_CONTRACT_ID:
            return fail(f"workspace manifest contract drifted for {example_id}")
        if workspace_payload.get("schema_version") != 1:
            return fail(f"workspace manifest schema drifted for {example_id}")
        if workspace_payload.get("example_id") != example_id:
            return fail(f"workspace manifest example_id drifted for {example_id}")
        if workspace_payload.get("workspace_root") != f"showcase/{example_id}":
            return fail(f"workspace manifest workspace_root drifted for {example_id}")
        if workspace_payload.get("entry_source") != entry.get("source"):
            return fail(f"workspace manifest entry_source drifted for {example_id}")
        if workspace_payload.get("emit_prefix") != "module":
            return fail(f"workspace manifest emit_prefix drifted for {example_id}")
        if workspace_payload.get("artifact_root") != f"tmp/artifacts/showcase/{example_id}":
            return fail(f"workspace manifest artifact_root drifted for {example_id}")
        if workspace_payload.get("package_stage_root") != f"showcase/{example_id}":
            return fail(f"workspace manifest package_stage_root drifted for {example_id}")
        if workspace_payload.get("story_capabilities") != entry.get("story_capabilities"):
            return fail(f"workspace manifest story_capabilities drifted for {example_id}")
        runtime_surface = workspace_payload.get("runtime_surface")
        if runtime_surface != {
            "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
            "runtime_library_resolution_model": "registration-manifest-runtime-archive-path-is-authoritative",
            "driver_linker_flag_consumption_model": "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands",
            "runtime_output_root": f"tmp/artifacts/showcase/{example_id}/runtime",
            "expected_exit_code": {"auroraBoard": 33, "signalMesh": 13, "patchKit": 7}[example_id],
        }:
            return fail(f"workspace manifest runtime_surface drifted for {example_id}")
        presentation = workspace_payload.get("presentation")
        expected_headlines = {
            "auroraBoard": "categories, reflection, synthesized behaviors",
            "signalMesh": "status bridging, actor-shaped messaging, runtime messaging",
            "patchKit": "derives, macros, property behaviors, interop",
        }
        if presentation != {
            "title": entry.get("title"),
            "headline": expected_headlines[example_id],
        }:
            return fail(f"workspace manifest presentation drifted for {example_id}")
        entry_with_workspace = dict(entry)
        entry_with_workspace["_workspace_payload"] = workspace_payload
        selected_examples.append(entry_with_workspace)

    if not selected_examples:
        return fail("selection produced no showcase examples")

    if run([sys.executable, str(RUNNER), "build-native-binaries"]) != 0:
        return fail("build-native-binaries failed")

    machine_output_root = ROOT / "tmp" / "artifacts" / "showcase"
    machine_output_root.mkdir(parents=True, exist_ok=True)
    machine_report_root = ROOT / "tmp" / "reports" / "showcase"
    machine_report_root.mkdir(parents=True, exist_ok=True)

    compile_results: list[dict[str, object]] = []

    for entry in selected_examples:
        example_id = entry.get("id")
        source = entry.get("source")
        if not isinstance(example_id, str) or not isinstance(source, str):
            return fail("example entry missing id/source")
        source_path = ROOT / source
        if not source_path.is_file():
            return fail(f"missing showcase source: {source}")
        source_text = source_path.read_text(encoding="utf-8")
        module_match = MODULE_DECL_RE.search(source_text)
        if module_match is None:
            return fail(f"missing module declaration in showcase source: {source}")
        workspace_payload = entry.get("_workspace_payload")
        if not isinstance(workspace_payload, dict):
            return fail(f"missing selected workspace payload for {example_id}")
        if workspace_payload.get("module_name") != module_match.group(1):
            return fail(f"workspace manifest module_name drifted for {example_id}")
        out_dir = machine_output_root / example_id
        command = [
            sys.executable,
            str(RUNNER),
            "compile-objc3c",
            source,
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ]
        if run(command) != 0:
            return fail(f"compile failed for showcase example {example_id}")
        required_artifacts = {
            "workspace_manifest": ROOT / str(entry["workspace_manifest"]),
            "compile_provenance": out_dir / "module.compile-provenance.json",
            "llvm_ir": out_dir / "module.ll",
            "object": out_dir / "module.obj",
            "manifest": out_dir / "module.manifest.json",
            "runtime_registration_manifest": out_dir / "module.runtime-registration-manifest.json",
        }
        for artifact_label, artifact_path in required_artifacts.items():
            if not artifact_path.is_file():
                return fail(
                    f"missing required showcase artifact {artifact_label} for {example_id}: "
                    f"{repo_relative(artifact_path)}"
                )
        compile_results.append(
            {
                "example_id": example_id,
                "module_name": module_match.group(1),
                "source": source,
                "workspace_manifest": str(entry["workspace_manifest"]),
                "story_capabilities": list(entry.get("story_capabilities", [])),
                "out_dir": repo_relative(out_dir),
                "artifacts": {label: repo_relative(path) for label, path in required_artifacts.items()},
            }
        )
        print(f"out_dir: {repo_relative(out_dir)}")

    summary_payload = {
        "contract_id": SHOWCASE_SUMMARY_CONTRACT_ID,
        "schema_version": 1,
        "portfolio_contract_id": payload["contract_id"],
        "showcase_root": payload["showcase_root"],
        "machine_output_root": payload["machine_output_root"],
        "machine_report_root": payload["machine_report_root"],
        "package_stage_root": payload["package_stage_root"],
        "selected_example_ids": [
            entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
        ],
        "examples": compile_results,
    }
    summary_path = machine_report_root / "summary.json"
    summary_path.write_text(json.dumps(summary_payload, indent=2) + "\n", encoding="utf-8")

    selected_ids = ", ".join(
        entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
    )
    print(f"summary_path: {repo_relative(summary_path)}")
    print(f"showcase-surface: OK ({selected_ids})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
