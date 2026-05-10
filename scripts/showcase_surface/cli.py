from __future__ import annotations

import argparse
import sys
from typing import Any

from scripts.objc3c_workflow.public_command_api import public_workflow_command

from .commands import run
from .fixtures import load_json_payload, read_module_name
from .paths import (
    GUIDED_WALKTHROUGH,
    MACHINE_OUTPUT_ROOT,
    MACHINE_REPORT_ROOT,
    PORTFOLIO,
    ROOT,
    SUMMARY_PATH,
    repo_relative,
)
from .rendering import write_summary_json
from .summary import (
    build_compile_result,
    build_summary_payload,
    required_showcase_artifacts,
)
from .validation import (
    known_story_capabilities,
    showcase_example_ids,
    validate_guided_walkthrough_contract,
    validate_portfolio_contract,
    validate_requested_capabilities,
    validate_requested_ids,
    validate_showcase_entry,
    validate_workspace_contract,
)


def fail(message: str) -> int:
    print(f"showcase-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


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


def _select_examples(
    examples: list[Any],
    requested_ids: set[str],
    requested_capabilities: set[str],
) -> tuple[str | None, list[dict[str, object]]]:
    selected_examples: list[dict[str, object]] = []
    for entry in examples:
        entry_error = validate_showcase_entry(entry)
        if entry_error is not None:
            return entry_error, selected_examples
        if not isinstance(entry, dict):
            raise RuntimeError("showcase example validation did not return an object")

        example_id = entry.get("id")
        workspace_manifest = entry.get("workspace_manifest")
        if not isinstance(example_id, str) or not isinstance(workspace_manifest, str):
            raise RuntimeError("showcase example validation did not publish required fields")

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
            return f"missing showcase workspace manifest: {workspace_manifest}", selected_examples
        workspace_payload = load_json_payload(workspace_manifest_path)
        workspace_error = validate_workspace_contract(entry, workspace_payload)
        if workspace_error is not None:
            return workspace_error, selected_examples

        entry_with_workspace = dict(entry)
        entry_with_workspace["_workspace_payload"] = workspace_payload
        selected_examples.append(entry_with_workspace)

    return None, selected_examples


def main() -> int:
    args = parse_args(sys.argv[1:])
    if not PORTFOLIO.is_file():
        return fail(f"missing showcase portfolio: {PORTFOLIO}")

    payload = load_json_payload(PORTFOLIO)
    portfolio_error = validate_portfolio_contract(payload)
    if portfolio_error is not None:
        return fail(portfolio_error)
    examples = payload.get("examples")
    if not isinstance(examples, list):
        raise RuntimeError("showcase portfolio validation did not publish examples")

    if not GUIDED_WALKTHROUGH.is_file():
        return fail(f"missing guided walkthrough manifest: {repo_relative(GUIDED_WALKTHROUGH)}")
    walkthrough_payload = load_json_payload(GUIDED_WALKTHROUGH)
    walkthrough_error = validate_guided_walkthrough_contract(walkthrough_payload)
    if walkthrough_error is not None:
        return fail(walkthrough_error)

    ids = showcase_example_ids(examples)
    requested_ids = set(args.example)
    ids_error = validate_requested_ids(requested_ids, ids)
    if ids_error is not None:
        return fail(ids_error)

    requested_capabilities = set(args.capability)
    capability_error = validate_requested_capabilities(
        requested_capabilities,
        known_story_capabilities(examples),
    )
    if capability_error is not None:
        return fail(capability_error)

    selection_error, selected_examples = _select_examples(
        examples,
        requested_ids,
        requested_capabilities,
    )
    if selection_error is not None:
        return fail(selection_error)

    if not selected_examples:
        return fail("selection produced no showcase examples")

    if run(public_workflow_command("build-native-binaries")) != 0:
        return fail("build-native-binaries failed")

    MACHINE_OUTPUT_ROOT.mkdir(parents=True, exist_ok=True)
    MACHINE_REPORT_ROOT.mkdir(parents=True, exist_ok=True)

    compile_results: list[dict[str, object]] = []

    for entry in selected_examples:
        example_id = entry.get("id")
        source = entry.get("source")
        if not isinstance(example_id, str) or not isinstance(source, str):
            return fail("example entry missing id/source")
        source_path = ROOT / source
        if not source_path.is_file():
            return fail(f"missing showcase source: {source}")
        module_name = read_module_name(source_path)
        if module_name is None:
            return fail(f"missing module declaration in showcase source: {source}")
        workspace_payload = entry.get("_workspace_payload")
        if not isinstance(workspace_payload, dict):
            return fail(f"missing selected workspace payload for {example_id}")
        if workspace_payload.get("module_name") != module_name:
            return fail(f"workspace manifest module_name drifted for {example_id}")

        out_dir = MACHINE_OUTPUT_ROOT / example_id
        command = public_workflow_command(
            "compile-objc3c",
            source,
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        )
        if run(command) != 0:
            return fail(f"compile failed for showcase example {example_id}")

        required_artifacts = required_showcase_artifacts(entry, out_dir)
        for artifact_label, artifact_path in required_artifacts.items():
            if not artifact_path.is_file():
                return fail(
                    f"missing required showcase artifact {artifact_label} for {example_id}: "
                    f"{repo_relative(artifact_path)}"
                )

        compile_results.append(
            build_compile_result(
                entry=entry,
                module_name=module_name,
                source=source,
                workspace_payload=workspace_payload,
                out_dir=out_dir,
                required_artifacts=required_artifacts,
            )
        )
        print(f"out_dir: {repo_relative(out_dir)}")

    summary_payload = build_summary_payload(
        portfolio_payload=payload,
        selected_examples=selected_examples,
        compile_results=compile_results,
    )
    write_summary_json(SUMMARY_PATH, summary_payload)

    selected_ids = ", ".join(
        entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
    )
    print(f"summary_path: {repo_relative(SUMMARY_PATH)}")
    print(f"showcase-surface: OK ({selected_ids})")
    return 0
