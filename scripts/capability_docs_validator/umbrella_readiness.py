from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT, repo_rel

from capability_docs_validator.errors import CapabilityDocsError

READINESS_REQUIREMENT_FIELDS = (
    "required_source_anchors",
    "required_public_commands",
    "required_positive_fixtures",
    "required_negative_fixtures",
    "required_runtime_probes",
    "required_abi_governance_rows",
    "required_docs",
)

GENERATED_OUTPUT_REJECTION_TOKENS = (
    "tmp/",
    "temp/",
    "generated markdown projections",
    "issue comments",
    "PR bodies",
)
FORBIDDEN_SOURCE_PATH_PREFIXES = (
    "tmp/",
    "tmp\\",
    "temp/",
    "temp\\",
    "generated/",
    "generated\\",
    "build/",
    "build\\",
    "dist/",
    "dist\\",
)
PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "


def _row_by_id(rows: list[dict[str, Any]]) -> dict[str, dict[str, Any]]:
    return {str(row["id"]): row for row in rows}


def _evidence_capability_ids(evidence_map: dict[str, Any]) -> set[str]:
    ids: set[str] = set()
    raw_rows = evidence_map.get("rows", [])
    if not isinstance(raw_rows, list):
        return ids
    for row in raw_rows:
        if isinstance(row, dict) and isinstance(row.get("capability_id"), str):
            ids.add(str(row["capability_id"]))
    return ids


def _require_existing_path(entry_id: str, requirement: dict[str, Any]) -> None:
    raw_path = requirement.get("path")
    if not isinstance(raw_path, str) or not raw_path:
        return
    raw_repo_path = Path(raw_path)
    if (
        raw_path.startswith(FORBIDDEN_SOURCE_PATH_PREFIXES)
        or raw_repo_path.is_absolute()
        or ".." in raw_repo_path.parts
    ):
        raise CapabilityDocsError(
            f"{entry_id} readiness requirement {requirement['id']} "
            f"uses non-source path as readiness evidence: {raw_path}"
        )
    path = ROOT / raw_path
    if not path.exists():
        raise CapabilityDocsError(
            f"{entry_id} readiness requirement {requirement['id']} "
            f"references missing path: {raw_path}"
        )


def _public_action_names() -> set[str]:
    from scripts.objc3c_workflow.action_catalog import ACTION_SPECS

    return set(ACTION_SPECS)


def _validate_public_command(entry_id: str, requirement: dict[str, Any]) -> None:
    command = requirement.get("command")
    if not isinstance(command, str) or not command:
        return
    if not command.startswith(PUBLIC_COMMAND_PREFIX):
        raise CapabilityDocsError(
            f"{entry_id} readiness requirement {requirement['id']} "
            f"uses unsupported public command surface: {command}"
        )
    action = command[len(PUBLIC_COMMAND_PREFIX) :].split(maxsplit=1)[0]
    if action not in _public_action_names():
        raise CapabilityDocsError(
            f"{entry_id} readiness requirement {requirement['id']} "
            f"references unregistered public workflow action: {action}"
        )


def _blocked_requirement_ids(entry: dict[str, Any]) -> set[str]:
    blockers = entry.get("promotion_blockers", [])
    if not isinstance(blockers, list):
        return set()
    return {
        str(blocker["blocker_id"])
        for blocker in blockers
        if isinstance(blocker, dict) and isinstance(blocker.get("blocker_id"), str)
    }


def _validate_requirement(
    entry_id: str,
    requirement: dict[str, Any],
    *,
    blocker_ids: set[str],
    rows_by_id: dict[str, dict[str, Any]],
) -> None:
    status = requirement.get("status")
    requirement_id = requirement.get("id")
    if status == "satisfied":
        if not any(key in requirement for key in ("path", "capability_id", "command")):
            raise CapabilityDocsError(
                f"{entry_id} readiness requirement {requirement_id} is satisfied "
                "but has no path, capability_id, or command evidence"
            )
        _require_existing_path(entry_id, requirement)
        _validate_public_command(entry_id, requirement)
        capability_id = requirement.get("capability_id")
        if isinstance(capability_id, str) and capability_id not in rows_by_id:
            raise CapabilityDocsError(
                f"{entry_id} readiness requirement {requirement_id} "
                f"references unknown capability row: {capability_id}"
            )
        return

    blocker_id = requirement.get("blocker_id")
    if not isinstance(blocker_id, str) or blocker_id not in blocker_ids:
        raise CapabilityDocsError(
            f"{entry_id} readiness requirement {requirement_id} is {status} "
            "but does not reference a declared promotion blocker"
        )


def _validate_generated_output_boundary(entry: dict[str, Any]) -> None:
    entry_id = str(entry["umbrella_capability_id"])
    boundary = entry["generated_output_boundary"]
    if boundary.get("source_truth_allowed") is not False:
        raise CapabilityDocsError(f"{entry_id} generated output must not be source truth")
    unsupported_sources = set(str(source) for source in boundary.get("unsupported_sources", []))
    missing = [token for token in GENERATED_OUTPUT_REJECTION_TOKENS if token not in unsupported_sources]
    if missing:
        raise CapabilityDocsError(
            f"{entry_id} generated output boundary is missing unsupported sources: "
            + ", ".join(missing)
        )


def validate_umbrella_readiness(
    readiness: dict[str, Any],
    *,
    rows: list[dict[str, Any]],
    evidence_map: dict[str, Any],
) -> None:
    rows_by_id = _row_by_id(rows)
    evidence_ids = _evidence_capability_ids(evidence_map)
    seen_entries: set[str] = set()

    for entry in readiness.get("entries", []):
        if not isinstance(entry, dict):
            raise CapabilityDocsError("umbrella readiness entries must be objects")
        entry_id = str(entry["umbrella_capability_id"])
        if entry_id in seen_entries:
            raise CapabilityDocsError(f"duplicate umbrella readiness entry: {entry_id}")
        seen_entries.add(entry_id)

        matrix_row = rows_by_id.get(entry_id)
        if matrix_row is None:
            raise CapabilityDocsError(f"umbrella readiness references unknown row: {entry_id}")
        if matrix_row["state"] != entry["current_state"]:
            raise CapabilityDocsError(
                f"{entry_id} readiness current_state {entry['current_state']} "
                f"does not match matrix state {matrix_row['state']}"
            )
        if entry["target_state"] != "implemented":
            raise CapabilityDocsError(f"{entry_id} target_state must be implemented")

        blockers = entry.get("promotion_blockers", [])
        if entry["readiness_state"] == "blocked" and not blockers:
            raise CapabilityDocsError(f"{entry_id} is blocked but has no promotion blockers")
        if entry["readiness_state"] == "ready" and blockers:
            raise CapabilityDocsError(f"{entry_id} is ready but still has promotion blockers")

        blocker_ids = _blocked_requirement_ids(entry)
        if len(blocker_ids) != len(blockers):
            raise CapabilityDocsError(f"{entry_id} has duplicate promotion blocker ids")

        _validate_generated_output_boundary(entry)

        for prerequisite in entry["required_prerequisite_rows"]:
            capability_id = str(prerequisite["capability_id"])
            required_state = str(prerequisite["required_state"])
            row = rows_by_id.get(capability_id)
            if row is None:
                raise CapabilityDocsError(
                    f"{entry_id} prerequisite references unknown row: {capability_id}"
                )
            if row["state"] != required_state:
                raise CapabilityDocsError(
                    f"{entry_id} prerequisite {capability_id} requires {required_state} "
                    f"but matrix has {row['state']}"
                )
            if required_state == "implemented" and capability_id not in evidence_ids:
                raise CapabilityDocsError(
                    f"{entry_id} prerequisite {capability_id} has no evidence-map rows"
                )

        for field in READINESS_REQUIREMENT_FIELDS:
            for requirement in entry[field]:
                if not isinstance(requirement, dict):
                    raise CapabilityDocsError(f"{entry_id} {field} entries must be objects")
                _validate_requirement(
                    entry_id,
                    requirement,
                    blocker_ids=blocker_ids,
                    rows_by_id=rows_by_id,
                )


def _code(value: object) -> str:
    text = str(value)
    return f"`{text}`" if text else ""


def _bullet_list(items: list[str], *, indent: str = "- ") -> list[str]:
    if not items:
        return [f"{indent}None"]
    return [f"{indent}{item}" for item in items]


def _requirement_summary(requirements: list[dict[str, Any]]) -> list[str]:
    lines: list[str] = []
    for requirement in requirements:
        status = _code(requirement.get("status", ""))
        description = str(requirement.get("description", ""))
        suffix_parts: list[str] = []
        for key in ("path", "capability_id", "command", "blocker_id"):
            value = requirement.get(key)
            if isinstance(value, str) and value:
                suffix_parts.append(f"{key}: {_code(value)}")
        suffix = f" ({'; '.join(suffix_parts)})" if suffix_parts else ""
        lines.append(f"- {status} {description}{suffix}")
    return lines or ["- None"]


def render_umbrella_readiness_doc(readiness: dict[str, Any]) -> str:
    lines: list[str] = [
        "# Umbrella Capability Readiness",
        "",
        "<!-- Generated by scripts/build_capability_support_docs.py; edit docs/support/umbrella_readiness.json instead. -->",
        "",
        "This document projects the checked umbrella readiness source. It explains",
        "why broad capability rows remain reserved or when they are ready for",
        "promotion. It does not create public Objective-C 3.0 behavior claims.",
        "",
        "Authoritative inputs:",
    ]
    projection = readiness.get("projection_policy", {})
    for path in projection.get("authoritative_data", []):
        lines.append(f"- {_code(path)}")
    lines.extend(
        [
            f"- {_code(readiness.get('schema_path', ''))}",
            "",
            "Consumer rule: "
            + str(projection.get("consumer_rule", "")),
            "",
        ]
    )

    for entry in readiness.get("entries", []):
        entry_id = str(entry["umbrella_capability_id"])
        lines.extend(
            [
                f"## {entry_id}",
                "",
                f"- Current state: {_code(entry['current_state'])}",
                f"- Target state: {_code(entry['target_state'])}",
                f"- Readiness state: {_code(entry['readiness_state'])}",
                f"- Intended public meaning: {entry['intended_public_meaning']}",
                "",
                "### Forbidden Overclaims",
                "",
                *_bullet_list([str(item) for item in entry["forbidden_overclaims"]]),
                "",
                "### Prerequisite Rows",
                "",
            ]
        )
        for prerequisite in entry["required_prerequisite_rows"]:
            lines.append(
                "- "
                f"{_code(prerequisite['capability_id'])} must be "
                f"{_code(prerequisite['required_state'])}: {prerequisite['reason']}"
            )
        lines.extend(["", "### Readiness Requirements", ""])
        for field in READINESS_REQUIREMENT_FIELDS:
            title = field.removeprefix("required_").replace("_", " ").title()
            lines.extend([f"#### {title}", "", *_requirement_summary(entry[field]), ""])
        lines.extend(
            [
                "### Generated Output Boundary",
                "",
                f"- Source truth allowed: {_code(str(entry['generated_output_boundary']['source_truth_allowed']).lower())}",
                f"- Rule: {entry['generated_output_boundary']['rule']}",
                "- Unsupported sources:",
                *_bullet_list(
                    [str(item) for item in entry["generated_output_boundary"]["unsupported_sources"]],
                    indent="  - ",
                ),
                "",
                "### Promotion Blockers",
                "",
            ]
        )
        if entry["promotion_blockers"]:
            for blocker in entry["promotion_blockers"]:
                lines.extend(
                    [
                        f"- {_code(blocker['blocker_id'])}: {blocker['summary']}",
                        *_bullet_list(
                            [str(item) for item in blocker["missing_work"]],
                            indent="  - ",
                        ),
                    ]
                )
        else:
            lines.append("- None")
        lines.extend(
            [
                "",
                "### Final Promotion Criteria",
                "",
                *_bullet_list([str(item) for item in entry["final_promotion_criteria"]]),
                "",
            ]
        )

    return "\n".join(lines)


def validate_umbrella_readiness_doc(path: Path, expected: str) -> None:
    if not path.is_file() or path.read_text(encoding="utf-8") != expected:
        raise CapabilityDocsError(
            "umbrella readiness projection drifted; run "
            "`python scripts/build_capability_support_docs.py` to regenerate: "
            + repo_rel(path)
        )
