#!/usr/bin/env python3
"""Validate the bounded getting-started tutorial surface against the live docs and showcase examples."""

from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
import shlex
import sys
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.json_io import write_report_json
from objc3c_tooling.subprocesses import python_script_command, run_capture
from objc3c_migration_analyzer import analyze_migration_input, apply_rewrite_plan
try:
    from scripts.objc3c_workflow.public_command_api import public_workflow_action_names
except ModuleNotFoundError:
    from objc3c_workflow.public_command_api import public_workflow_action_names

DOCUMENTATION_SURFACE_PY = ROOT / "scripts" / "check_documentation_surface.py"
SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
WALKTHROUGH_PATH = ROOT / "showcase" / "tutorial_walkthrough.json"
PROGRAM_SURFACE_PATH = ROOT / "stdlib" / "program_surface.json"
PACKAGE_JSON_PATH = ROOT / "package.json"
FIRST_RUN_WORKFLOW_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "first_run_workflow_contract.json"
)
DEVELOPER_EXPERIENCE_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "developer_experience_completion_contract.json"
)
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
FIRST_RUN_WORKFLOW_CONTRACT_ID = "objc3c.developer.first-run.workflow.v1"
DEVELOPER_EXPERIENCE_CONTRACT_ID = "objc3c.developer.experience.completion.v1"


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


def _parse_public_command_with_context(command: str, context: str) -> tuple[str, list[str]]:
    try:
        action, tokens = _parse_public_command(command)
    except ValueError as exc:
        raise RuntimeError(f"{context} has an invalid objc3c command: {exc}") from exc
    _reject_extra_action_separator(action=action, tokens=tokens, context=context)
    return action, tokens


def _reject_extra_action_separator(*, action: str, tokens: list[str], context: str) -> None:
    separator_index = len(PUBLIC_COMMAND_PREFIX) + 1
    if len(tokens) > separator_index and tokens[separator_index] == "--":
        raise RuntimeError(
            f"{context} has an extra separator after the workflow action; "
            f"use `npm run objc3c -- {action} ...`"
        )


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
            action, _tokens = _parse_public_command_with_context(
                command,
                f"{repo_rel(path)}:{line_number}",
            )
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
        action, tokens = _parse_public_command_with_context(
            command,
            f"{repo_rel(WALKTHROUGH_PATH)} step {index}",
        )
        expect(action != "<action>", f"tutorial walkthrough step {index} used the placeholder public action")
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


def _require_string_list(value: Any, label: str) -> list[str]:
    expect(isinstance(value, list), f"{label} must be a list")
    strings = [entry for entry in value if isinstance(entry, str)]
    expect(len(strings) == len(value), f"{label} must contain only strings")
    return strings


def _require_string_map(value: Any, label: str) -> dict[str, str]:
    expect(isinstance(value, dict), f"{label} must be an object")
    result = {str(key): entry for key, entry in value.items() if isinstance(entry, str)}
    expect(len(result) == len(value), f"{label} must contain only string values")
    return result


def _record_lookup(records: Sequence[PublicCommandRecord]) -> set[tuple[str, str]]:
    return {(record.source_path, record.command) for record in records}


def _record_line_lookup(records: Sequence[PublicCommandRecord]) -> dict[tuple[str, str], int]:
    return {
        (record.source_path, record.command): int(record.line)
        for record in records
        if isinstance(record.line, int)
    }


def _repo_file(path_text: str, *, root: Path, label: str) -> Path:
    path = root / path_text
    expect(path.is_file(), f"{label} is missing: {path_text}")
    return path


def _validate_required_doc_commands(
    *,
    required_doc_commands: Any,
    public_command_records: Sequence[PublicCommandRecord],
    label: str,
) -> int:
    expect(isinstance(required_doc_commands, list), f"{label} must be a list")
    record_lookup = _record_lookup(public_command_records)
    missing_doc_commands: list[dict[str, str]] = []
    for index, entry in enumerate(required_doc_commands):
        expect(isinstance(entry, dict), f"{label}[{index}] must be an object")
        command = entry.get("command")
        source_path = entry.get("source_path")
        expect(isinstance(command, str), f"{label}[{index}].command must be a string")
        expect(isinstance(source_path, str), f"{label}[{index}].source_path must be a string")
        _parse_public_command_with_context(command, f"{label} required command {index}")
        if (source_path, command) not in record_lookup:
            missing_doc_commands.append({"source_path": source_path, "command": command})
    expect(not missing_doc_commands, f"{label} required doc commands are missing: {missing_doc_commands}")
    return len(required_doc_commands)


def _walkthrough_step_by_id(walkthrough: dict[str, Any]) -> dict[str, dict[str, Any]]:
    steps = walkthrough.get("steps")
    expect(isinstance(steps, list), "tutorial walkthrough did not publish steps")
    result: dict[str, dict[str, Any]] = {}
    for step in steps:
        expect(isinstance(step, dict), "tutorial walkthrough published a malformed step")
        step_id = step.get("id")
        expect(isinstance(step_id, str), "tutorial walkthrough step did not publish id")
        result[step_id] = step
    return result


def validate_first_run_workflow_contract(
    *,
    contract: dict[str, Any],
    package_payload: dict[str, Any],
    public_command_records: Sequence[PublicCommandRecord],
    walkthrough: dict[str, Any],
    root: Path = ROOT,
) -> dict[str, Any]:
    expect(
        contract.get("contract_id") == FIRST_RUN_WORKFLOW_CONTRACT_ID,
        "first-run workflow contract_id drifted",
    )
    expect(contract.get("schema_version") == 1, "first-run workflow schema_version drifted")

    package_bridge = contract.get("package_bridge")
    expect(isinstance(package_bridge, dict), "first-run workflow package_bridge must be an object")
    expected_script = package_bridge.get("expected_script")
    expect(isinstance(expected_script, str), "first-run workflow expected_script must be a string")
    public_prefix = tuple(
        _require_string_list(
            package_bridge.get("public_command_prefix"),
            "package_bridge.public_command_prefix",
        )
    )
    expect(public_prefix == PUBLIC_COMMAND_PREFIX, "first-run workflow public command prefix drifted")

    scripts = package_payload.get("scripts")
    expect(isinstance(scripts, dict), "package.json did not publish scripts")
    expect(
        scripts.get("objc3c") == expected_script,
        "package.json scripts.objc3c drifted from the first-run workflow contract",
    )

    registered_actions = set(public_workflow_action_names())
    required_actions = _require_string_list(
        contract.get("required_public_actions"),
        "required_public_actions",
    )
    missing_actions = sorted(action for action in required_actions if action not in registered_actions)
    expect(not missing_actions, f"first-run workflow references unregistered actions: {missing_actions}")

    required_doc_command_count = _validate_required_doc_commands(
        required_doc_commands=contract.get("required_fenced_doc_commands"),
        public_command_records=public_command_records,
        label="first-run workflow",
    )

    first_compile = _require_string_map(contract.get("first_compile"), "first_compile")
    source = first_compile["source"]
    expect((root / source).is_file(), f"first-run compile source is missing: {source}")
    expect(
        first_compile["workflow_action"] == "compile-objc3c",
        "first-run compile workflow_action drifted",
    )

    steps_by_id = _walkthrough_step_by_id(walkthrough)
    required_walkthrough_steps = _require_string_list(
        contract.get("required_walkthrough_steps"),
        "required_walkthrough_steps",
    )
    missing_steps = [step_id for step_id in required_walkthrough_steps if step_id not in steps_by_id]
    expect(not missing_steps, f"first-run workflow walkthrough steps are missing: {missing_steps}")

    compile_step = steps_by_id[first_compile["walkthrough_step_id"]]
    command = compile_step.get("public_command")
    expect(isinstance(command, str), "first-run walkthrough compile step did not publish public_command")
    action, tokens = _parse_public_command_with_context(
        command,
        f"{repo_rel(WALKTHROUGH_PATH)} step {first_compile['walkthrough_step_id']}",
    )
    expect(action == first_compile["workflow_action"], "first-run walkthrough compile action drifted")
    expect(source in tokens, "first-run walkthrough compile command did not include its source")
    expect(
        compile_step.get("artifact_root") == first_compile["artifact_root"],
        "first-run walkthrough artifact root drifted",
    )

    return {
        "status": "PASS",
        "contract_path": repo_rel(FIRST_RUN_WORKFLOW_CONTRACT_PATH, root=root),
        "package_script": expected_script,
        "required_public_actions": required_actions,
        "required_fenced_doc_command_count": required_doc_command_count,
        "required_walkthrough_steps": required_walkthrough_steps,
        "first_compile": first_compile,
    }


def _validate_template_compile_contract(
    template_compile: Any,
    *,
    registered_actions: set[str],
    root: Path,
) -> dict[str, Any]:
    expect(isinstance(template_compile, dict), "template_compile_contract must be an object")
    materializer_action = template_compile.get("materializer_action")
    compile_action = template_compile.get("compile_action")
    expect(materializer_action in registered_actions, "template materializer action is not public")
    expect(compile_action in registered_actions, "template compile action is not public")
    source_origin = str(template_compile.get("source_origin", ""))
    _repo_file(source_origin, root=root, label="template compile source_origin")
    materialized_source = str(template_compile.get("materialized_source", ""))
    artifact_root = str(template_compile.get("artifact_root", ""))
    expect(
        materialized_source == "tmp/artifacts/project-template/auroraBoard/src/main.objc3",
        "template materialized source drifted from the canonical first template",
    )
    expect(
        artifact_root == "tmp/artifacts/project-template/auroraBoard/build",
        "template compile artifact root drifted",
    )
    command = template_compile.get("public_compile_command")
    expect(isinstance(command, str), "template compile command must be a string")
    action, tokens = _parse_public_command_with_context(command, "template compile contract")
    expect(action == compile_action, "template compile command action drifted")
    expect(materialized_source in tokens, "template compile command did not include materialized source")
    expect("--out-dir" in tokens and artifact_root in tokens, "template compile command did not publish out-dir")
    expect("--emit-prefix" in tokens and "module" in tokens, "template compile command did not publish emit-prefix")
    return {
        "materializer_action": materializer_action,
        "compile_action": compile_action,
        "source_origin": source_origin,
        "materialized_source": materialized_source,
        "artifact_root": artifact_root,
    }


def _validate_migration_examples(
    migration_examples: Any,
    *,
    public_command_records: Sequence[PublicCommandRecord],
    registered_actions: set[str],
    root: Path,
) -> dict[str, Any]:
    expect(isinstance(migration_examples, dict), "migration_examples must be an object")
    for action in _require_string_list(
        migration_examples.get("required_public_actions"),
        "migration_examples.required_public_actions",
    ):
        expect(action in registered_actions, f"migration example action is not public: {action}")
    doc_command_count = _validate_required_doc_commands(
        required_doc_commands=migration_examples.get("required_fenced_doc_commands"),
        public_command_records=public_command_records,
        label="migration examples",
    )

    positive_input = _repo_file(
        str(migration_examples.get("positive_input", "")),
        root=root,
        label="positive migration input",
    )
    expected_rewrite_output = _repo_file(
        str(migration_examples.get("expected_rewrite_output", "")),
        root=root,
        label="expected migration rewrite output",
    )
    positive = analyze_migration_input(positive_input)
    expect(positive.ok, "positive migration example did not pass")
    rewritten = apply_rewrite_plan(positive.source_text, positive.payload["rewrite_plan"])
    expected_rewritten = expected_rewrite_output.read_text(encoding="utf-8")
    expect(rewritten == expected_rewritten, "positive migration expected rewrite output drifted")

    negative_input = _repo_file(
        str(migration_examples.get("negative_input", "")),
        root=root,
        label="negative migration input",
    )
    expected_negative_path = _repo_file(
        str(migration_examples.get("expected_negative_diagnostics", "")),
        root=root,
        label="expected negative migration diagnostics",
    )
    expected_negative = load_json(expected_negative_path)
    expect(
        expected_negative.get("contract_id") == "objc3c.migration_analyzer.expected_diagnostics.v1",
        "expected migration diagnostics contract_id drifted",
    )
    negative = analyze_migration_input(negative_input)
    expect(not negative.ok, "negative migration example did not fail closed")
    diagnostics = [item for item in negative.payload.get("diagnostics", []) if isinstance(item, dict)]
    for index, requirement in enumerate(expected_negative.get("required_diagnostics", [])):
        expect(isinstance(requirement, dict), f"required negative diagnostic {index} is malformed")
        code = requirement.get("code")
        matches = [
            item
            for item in diagnostics
            if item.get("code") == code
            and item.get("category") == requirement.get("category")
            and item.get("surface") == requirement.get("surface")
        ]
        minimum_count = int(requirement.get("minimum_count", 1))
        expect(len(matches) >= minimum_count, f"missing required migration diagnostic {code}")
        for field in _require_string_list(
            requirement.get("metadata_fields"),
            f"required_diagnostics[{index}].metadata_fields",
        ):
            expect(
                all(field in item and item[field] not in ("", None) for item in matches),
                f"{code} missing metadata field {field}",
            )

    return {
        "positive_input": repo_rel(positive_input, root=root),
        "expected_rewrite_output": repo_rel(expected_rewrite_output, root=root),
        "negative_input": repo_rel(negative_input, root=root),
        "expected_negative_diagnostics": repo_rel(expected_negative_path, root=root),
        "doc_command_count": doc_command_count,
        "automatic_edit_count": positive.payload["rewrite_plan"]["automatic_edit_count"],
        "negative_diagnostic_count": len(diagnostics),
    }


def _valid_position(position: object) -> bool:
    return (
        isinstance(position, dict)
        and isinstance(position.get("line"), int)
        and isinstance(position.get("column"), int)
        and int(position["line"]) > 0
        and int(position["column"]) > 0
    )


def _valid_range(range_payload: object) -> bool:
    if not isinstance(range_payload, dict):
        return False
    start = range_payload.get("start")
    end = range_payload.get("end")
    return _valid_position(start) and _valid_position(end)


def _validate_diagnostic_fixit_metadata(diagnostic_metadata: Any, *, root: Path) -> dict[str, Any]:
    expect(isinstance(diagnostic_metadata, dict), "diagnostic_fixit_metadata must be an object")
    bridge_contract_path = _repo_file(
        str(diagnostic_metadata.get("bridge_contract", "")),
        root=root,
        label="diagnostic bridge contract",
    )
    bridge_contract = load_json(bridge_contract_path)
    case_paths = _require_string_list(bridge_contract.get("diagnostic_case_paths"), "diagnostic_case_paths")
    expected_code_actions = set(
        _require_string_list(diagnostic_metadata.get("required_code_action_codes"), "required_code_action_codes")
    )
    required_fields = _require_string_list(
        diagnostic_metadata.get("required_diagnostic_fields"),
        "required_diagnostic_fields",
    )
    observed_code_actions: set[str] = set()
    diagnostic_count = 0
    machine_fixit_count = 0

    for case_path_text in case_paths:
        case_path = _repo_file(case_path_text, root=root, label="diagnostic metadata case")
        case_payload = load_json(case_path)
        expect(isinstance(case_payload.get("expect"), dict), f"{case_path_text} did not publish expect")
        diagnostics = case_payload["expect"].get("diagnostics")
        expect(isinstance(diagnostics, list), f"{case_path_text} did not publish diagnostics")
        for diagnostic in diagnostics:
            expect(isinstance(diagnostic, dict), f"{case_path_text} published a malformed diagnostic")
            diagnostic_count += 1
            for field in required_fields:
                expect(field in diagnostic, f"{case_path_text} diagnostic missing {field}")
                if field not in {"fixits"}:
                    expect(diagnostic[field] not in ("", None), f"{case_path_text} diagnostic has empty {field}")
            expect(_valid_range(diagnostic.get("span")), f"{case_path_text} diagnostic span is malformed")
            fixits = diagnostic.get("fixits")
            expect(isinstance(fixits, list), f"{case_path_text} diagnostic fixits must be a list")
            for fixit in fixits:
                expect(isinstance(fixit, dict), f"{case_path_text} fixit is malformed")
                expect(_valid_range(fixit.get("range")), f"{case_path_text} fixit range is malformed")
                expect(isinstance(fixit.get("replacement"), str), f"{case_path_text} fixit replacement is malformed")
                machine_fixit_count += 1
                code = diagnostic.get("code")
                if isinstance(code, str):
                    observed_code_actions.add(code)
            recovery = diagnostic.get("recovery")
            expect(isinstance(recovery, dict), f"{case_path_text} diagnostic missing recovery metadata")
            expect(recovery.get("deterministic") is True, f"{case_path_text} recovery must be deterministic")
            expect(recovery.get("accepts_invalid_program") is False, f"{case_path_text} recovery must fail closed")

    expect(expected_code_actions.issubset(observed_code_actions), "required diagnostic code-action codes are missing")
    expected_fixit_count = int(diagnostic_metadata.get("minimum_machine_applicable_fixit_count", 0))
    expect(
        machine_fixit_count >= expected_fixit_count,
        "diagnostic fix-it count is below the developer experience floor",
    )
    return {
        "bridge_contract": repo_rel(bridge_contract_path, root=root),
        "diagnostic_case_count": len(case_paths),
        "diagnostic_count": diagnostic_count,
        "machine_applicable_fixit_count": machine_fixit_count,
        "required_code_action_codes": sorted(expected_code_actions),
    }


def _validate_clean_room_project_usability(
    clean_room: Any,
    *,
    public_command_records: Sequence[PublicCommandRecord],
    registered_actions: set[str],
    root: Path,
) -> dict[str, Any]:
    expect(isinstance(clean_room, dict), "clean_room_project_usability must be an object")
    expect(
        clean_room.get("support_claim") == "objc3c.behavior.tooling.first-run-product-path",
        "clean-room project usability support claim drifted",
    )
    source_truth_paths = _require_string_list(
        clean_room.get("source_truth_paths"),
        "clean_room_project_usability.source_truth_paths",
    )
    generated_output_paths = _require_string_list(
        clean_room.get("generated_output_paths"),
        "clean_room_project_usability.generated_output_paths",
    )
    source_truth_set = set(source_truth_paths)
    generated_output_set = set(generated_output_paths)
    expect(
        source_truth_set.isdisjoint(generated_output_set),
        "clean-room source truth must not overlap generated outputs",
    )
    for path_text in source_truth_paths:
        expect(
            not path_text.startswith("tmp/"),
            f"clean-room source truth must not depend on tmp output: {path_text}",
        )
        _repo_file(path_text, root=root, label="clean-room source truth")
    for path_text in generated_output_paths:
        expect(
            path_text.startswith("tmp/"),
            f"clean-room generated output path must stay under tmp/: {path_text}",
        )

    template_workspace = str(clean_room.get("template_workspace_manifest", ""))
    expect(
        template_workspace in generated_output_set,
        "clean-room template workspace manifest must be a generated output",
    )
    expect(
        template_workspace.endswith("/workspace.json"),
        "clean-room template workspace manifest path must end in workspace.json",
    )

    required_manifest_fields = _require_string_list(
        clean_room.get("required_template_manifest_fields"),
        "clean_room_project_usability.required_template_manifest_fields",
    )
    expect(
        {"template_source", "template_workspace_manifest", "clean_room_usability"}.issubset(
            set(required_manifest_fields)
        ),
        "clean-room required template manifest fields no longer prove source, workspace, and policy",
    )

    command_specs = clean_room.get("required_public_commands")
    expect(isinstance(command_specs, list), "clean_room_project_usability.required_public_commands must be a list")
    observed_actions: list[str] = []
    record_lookup = _record_lookup(public_command_records)
    for index, entry in enumerate(command_specs):
        expect(isinstance(entry, dict), f"clean-room public command {index} must be an object")
        command = str(entry.get("command", ""))
        action, tokens = _parse_public_command_with_context(
            command,
            f"clean-room public command {index}",
        )
        expect(action in registered_actions, f"clean-room command action is not public: {action}")
        observed_actions.append(action)
        doc_surface = entry.get("source_path")
        if isinstance(doc_surface, str) and doc_surface:
            expect(
                (doc_surface, command) in record_lookup,
                f"clean-room required command is missing from docs: {command}",
            )
        if action == "compile-objc3c":
            expect("--out-dir" in tokens and "--emit-prefix" in tokens, "clean-room compile command is incomplete")

    policy = str(clean_room.get("generated_output_policy", ""))
    expect("tmp" in policy and "authoritative" in policy, "clean-room generated output policy must reject tmp as authority")
    expect(
        {"materialize-project-template", "compile-objc3c", "inspect-compile-observability"}.issubset(
            set(observed_actions)
        ),
        "clean-room project usability must cover materialize, compile, and inspect actions",
    )
    return {
        "support_claim": clean_room["support_claim"],
        "source_truth_paths": source_truth_paths,
        "generated_output_paths": generated_output_paths,
        "template_workspace_manifest": template_workspace,
        "actions": observed_actions,
    }


def _validate_onboarding_command_map(
    onboarding_map: Any,
    *,
    public_command_records: Sequence[PublicCommandRecord],
    registered_actions: set[str],
    root: Path,
) -> dict[str, Any]:
    expect(isinstance(onboarding_map, dict), "onboarding_command_map must be an object")
    expect(
        onboarding_map.get("support_claim") == "objc3c.behavior.tooling.first-run-product-path",
        "onboarding command map support claim drifted",
    )
    doc_surface = str(onboarding_map.get("doc_surface", ""))
    _repo_file(doc_surface, root=root, label="onboarding command map doc_surface")
    source_truth_rule = str(onboarding_map.get("source_truth_rule", ""))
    expect("tmp" in source_truth_rule and "source-truth" in source_truth_rule, "onboarding command map must reject tmp source truth")

    forbidden_primary_actions = set(
        _require_string_list(
            onboarding_map.get("forbidden_primary_actions"),
            "onboarding_command_map.forbidden_primary_actions",
        )
    )
    required_stage_ids = _require_string_list(
        onboarding_map.get("required_stage_ids"),
        "onboarding_command_map.required_stage_ids",
    )
    stages = onboarding_map.get("stages")
    expect(isinstance(stages, list), "onboarding_command_map.stages must be a list")
    expect(len(stages) == len(required_stage_ids), "onboarding command map stage count drifted")

    record_lookup = _record_lookup(public_command_records)
    line_lookup = _record_line_lookup(public_command_records)
    observed_stage_ids: list[str] = []
    observed_actions: list[str] = []
    observed_lines: list[int] = []
    output_paths: list[str] = []

    for index, stage in enumerate(stages):
        expect(isinstance(stage, dict), f"onboarding command stage {index} must be an object")
        stage_id = str(stage.get("id", ""))
        observed_stage_ids.append(stage_id)
        intent = str(stage.get("intent", ""))
        command = str(stage.get("command", ""))
        public_doc = str(stage.get("public_doc", ""))
        failure_policy = str(stage.get("failure_policy", ""))
        expect(intent != "", f"onboarding command stage {stage_id} is missing intent")
        expect(public_doc == doc_surface, f"onboarding command stage {stage_id} drifted from the doc surface")
        expect(failure_policy.startswith("stop"), f"onboarding command stage {stage_id} must fail closed")

        action, _tokens = _parse_public_command_with_context(
            command,
            f"onboarding command stage {stage_id}",
        )
        expect(action in registered_actions, f"onboarding command stage {stage_id} action is not public: {action}")
        expect(action not in forbidden_primary_actions, f"onboarding command stage {stage_id} uses a forbidden primary action")
        expect((public_doc, command) in record_lookup, f"onboarding command stage {stage_id} is missing from docs")
        line = line_lookup.get((public_doc, command))
        expect(line is not None, f"onboarding command stage {stage_id} must be in a fenced doc block")
        observed_lines.append(line)
        observed_actions.append(action)

        source_path = stage.get("source_path")
        if isinstance(source_path, str) and source_path:
            _repo_file(source_path, root=root, label=f"onboarding command stage {stage_id} source_path")

        stage_output_paths = _require_string_list(
            stage.get("output_paths", []),
            f"onboarding_command_map.stages[{index}].output_paths",
        )
        for output_path in stage_output_paths:
            expect(
                output_path.startswith("tmp/"),
                f"onboarding command stage {stage_id} output path must stay under tmp/: {output_path}",
            )
            output_paths.append(output_path)

        prior_stage = stage.get("requires_prior_stage")
        if isinstance(prior_stage, str) and prior_stage:
            expect(prior_stage in observed_stage_ids[:-1], f"onboarding command stage {stage_id} has an unknown prior stage")

    expect(observed_stage_ids == required_stage_ids, "onboarding command map stage order drifted")
    expect(observed_lines == sorted(observed_lines), "onboarding command map docs are not in contract order")
    expect(
        {"build-native-binaries", "compile-objc3c", "inspect-compile-observability", "materialize-project-template", "validate-getting-started"}.issubset(set(observed_actions)),
        "onboarding command map no longer covers build, compile, inspect, template, and validation",
    )

    return {
        "support_claim": onboarding_map["support_claim"],
        "doc_surface": doc_surface,
        "stage_ids": observed_stage_ids,
        "actions": observed_actions,
        "output_paths": output_paths,
    }


def validate_developer_experience_completion_contract(
    *,
    contract: dict[str, Any],
    public_command_records: Sequence[PublicCommandRecord],
    root: Path = ROOT,
) -> dict[str, Any]:
    expect(
        contract.get("contract_id") == DEVELOPER_EXPERIENCE_CONTRACT_ID,
        "developer experience completion contract_id drifted",
    )
    expect(contract.get("schema_version") == 1, "developer experience completion schema_version drifted")
    issue_ids = [int(issue_id) for issue_id in contract.get("issue_ids", [])]
    expect(8157 in issue_ids, "developer experience completion contract does not cover #8157")
    registered_actions = set(public_workflow_action_names())
    return {
        "status": "PASS",
        "contract_path": repo_rel(DEVELOPER_EXPERIENCE_CONTRACT_PATH, root=root),
        "issue_ids": issue_ids,
        "template_compile_contract": _validate_template_compile_contract(
            contract.get("template_compile_contract"),
            registered_actions=registered_actions,
            root=root,
        ),
        "migration_examples": _validate_migration_examples(
            contract.get("migration_examples"),
            public_command_records=public_command_records,
            registered_actions=registered_actions,
            root=root,
        ),
        "diagnostic_fixit_metadata": _validate_diagnostic_fixit_metadata(
            contract.get("diagnostic_fixit_metadata"),
            root=root,
        ),
        "clean_room_project_usability": _validate_clean_room_project_usability(
            contract.get("clean_room_project_usability"),
            public_command_records=public_command_records,
            registered_actions=registered_actions,
            root=root,
        ),
        "onboarding_command_map": _validate_onboarding_command_map(
            contract.get("onboarding_command_map"),
            public_command_records=public_command_records,
            registered_actions=registered_actions,
            root=root,
        ),
    }


def main() -> int:
    walkthrough = load_json(WALKTHROUGH_PATH)
    program_surface = load_json(PROGRAM_SURFACE_PATH)
    package_payload = load_json(PACKAGE_JSON_PATH)
    first_run_contract = load_json(FIRST_RUN_WORKFLOW_CONTRACT_PATH)
    developer_experience_contract = load_json(DEVELOPER_EXPERIENCE_CONTRACT_PATH)
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
    first_run_workflow = validate_first_run_workflow_contract(
        contract=first_run_contract,
        package_payload=package_payload,
        public_command_records=public_command_records,
        walkthrough=walkthrough,
    )
    developer_experience_completion = validate_developer_experience_completion_contract(
        contract=developer_experience_contract,
        public_command_records=public_command_records,
    )
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
        str(entry.get("id")): entry
        for entry in program_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    expect(
        list(program_examples_by_id) == example_ids,
        "program surface example ids drifted from getting-started walkthrough",
    )

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
        "first_run_workflow": first_run_workflow,
        "developer_experience_completion": developer_experience_completion,
        "child_report_paths": [repo_rel(SHOWCASE_SUMMARY_PATH)],
        "showcase_surface_summary": showcase_summary,
    }
    write_report_json(REPORT_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
