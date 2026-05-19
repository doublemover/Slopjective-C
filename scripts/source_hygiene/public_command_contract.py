from __future__ import annotations

from collections.abc import Mapping, Sequence
from pathlib import Path
from typing import Any


CANONICAL_PACKAGE_BRIDGE = "objc3c"
CANONICAL_PUBLIC_ENTRYPOINT = "single-npm-bridge"
CANONICAL_PUBLIC_COMMAND_PREFIX = f"npm run {CANONICAL_PACKAGE_BRIDGE} --"
CANONICAL_PACKAGE_BRIDGE_ACTION = "<action>"
CANONICAL_PACKAGE_BRIDGE_BACKEND = (
    f"{CANONICAL_PUBLIC_COMMAND_PREFIX} {CANONICAL_PACKAGE_BRIDGE_ACTION}"
)
MAX_PACKAGE_BRIDGES = 1
RETIRED_PUBLIC_ACTIONS = frozenset({"lint-default"})


def canonical_public_command(action: str) -> str:
    return f"{CANONICAL_PUBLIC_COMMAND_PREFIX} {action}"


def _sequence(payload: Mapping[str, Any], key: str) -> Sequence[Any]:
    value = payload.get(key, ())
    return value if isinstance(value, Sequence) and not isinstance(value, (str, bytes)) else ()


def _action_name(entry: Mapping[str, Any], index: int) -> str:
    action = entry.get("action")
    return action if isinstance(action, str) and action else f"<action[{index}]>"


def package_bridge_names(contract: Mapping[str, Any]) -> list[str]:
    names: list[str] = []
    for entry in _sequence(contract, "package_bridges"):
        if isinstance(entry, Mapping):
            value = entry.get("package_bridge")
            if isinstance(value, str):
                names.append(value)
        elif isinstance(entry, str):
            names.append(entry)
    return names


def validate_package_bridge_budget(contract: Mapping[str, Any]) -> list[str]:
    package_bridges = package_bridge_names(contract)
    failures: list[str] = []
    missing = contract.get("missing_package_bridge")
    unexpected = contract.get("unexpected_package_bridges")
    package_bridge_count = contract.get("package_bridge_count")

    if missing:
        failures.append(f"missing package bridge: {missing}")
    if unexpected:
        failures.append(f"unexpected package bridges present: {unexpected}")
    if package_bridges != [CANONICAL_PACKAGE_BRIDGE]:
        failures.append(
            f"package bridge drifted: expected {[CANONICAL_PACKAGE_BRIDGE]} got {package_bridges}"
        )
    if package_bridge_count != MAX_PACKAGE_BRIDGES:
        failures.append(
            f"package bridge budget must be exactly {MAX_PACKAGE_BRIDGES}: got {package_bridge_count}"
        )
    if len(package_bridges) != MAX_PACKAGE_BRIDGES:
        failures.append(
            f"package bridge payload budget must be exactly {MAX_PACKAGE_BRIDGES}: got {len(package_bridges)}"
        )
    return failures


def validate_package_bridge_payloads(contract: Mapping[str, Any]) -> list[str]:
    failures: list[str] = []
    for index, raw_entry in enumerate(_sequence(contract, "package_bridges")):
        if not isinstance(raw_entry, Mapping):
            failures.append(f"package bridge entry {index} is not an object")
            continue
        bridge = raw_entry.get("package_bridge")
        action = raw_entry.get("action")
        backend = raw_entry.get("backend")
        public_entrypoint = raw_entry.get("public_entrypoint")
        if bridge != CANONICAL_PACKAGE_BRIDGE:
            failures.append(f"package bridge entry {index} bridge drifted: {bridge!r}")
        if action != CANONICAL_PACKAGE_BRIDGE_ACTION:
            failures.append(f"package bridge entry {index} action drifted: {action!r}")
        if backend != CANONICAL_PACKAGE_BRIDGE_BACKEND:
            failures.append(f"package bridge entry {index} backend drifted: {backend!r}")
        if public_entrypoint != CANONICAL_PUBLIC_ENTRYPOINT:
            failures.append(
                f"package bridge entry {index} public_entrypoint drifted: {public_entrypoint!r}"
            )
    return failures


def validate_public_action_surface(contract: Mapping[str, Any]) -> list[str]:
    failures: list[str] = []
    for index, raw_entry in enumerate(_sequence(contract, "actions")):
        if not isinstance(raw_entry, Mapping):
            failures.append(f"action entry {index} is not an object")
            continue
        action = _action_name(raw_entry, index)
        expected_command = canonical_public_command(action)
        if action in RETIRED_PUBLIC_ACTIONS:
            failures.append(f"retired public action present: {action}")
        if ":" in action:
            failures.append(f"public action uses retired colon-style name: {action}")
        if raw_entry.get("package_bridge") != CANONICAL_PACKAGE_BRIDGE:
            failures.append(
                f"{action}: package_bridge drifted: {raw_entry.get('package_bridge')!r}"
            )
        if raw_entry.get("public_entrypoint") != CANONICAL_PUBLIC_ENTRYPOINT:
            failures.append(
                f"{action}: public_entrypoint drifted: {raw_entry.get('public_entrypoint')!r}"
            )
        for field in ("public_command", "public_invocation"):
            if raw_entry.get(field) != expected_command:
                failures.append(
                    f"{action}: {field} must be {expected_command!r}, got {raw_entry.get(field)!r}"
                )
    return failures


def validate_public_command_contract(contract: Mapping[str, Any]) -> list[str]:
    failures: list[str] = []
    failures.extend(validate_package_bridge_budget(contract))
    failures.extend(validate_package_bridge_payloads(contract))
    failures.extend(validate_public_action_surface(contract))
    if contract.get("internal_action_count") not in (0, None):
        failures.append(
            f"internal action budget must remain zero on the public command surface: {contract.get('internal_action_count')}"
        )
    return failures


def build_public_command_budget_summary(
    contract: Mapping[str, Any],
    *,
    contract_path: Path | str,
) -> dict[str, object]:
    failures = validate_public_command_contract(contract)
    return {
        "status": "PASS" if not failures else "FAIL",
        "package_bridge_count": contract.get("package_bridge_count"),
        "workflow_action_count": contract.get("workflow_action_count"),
        "internal_action_count": contract.get("internal_action_count"),
        "operator_action_count": contract.get("operator_action_count"),
        "maintainer_action_count": contract.get("maintainer_action_count"),
        "package_bridges": package_bridge_names(contract),
        "max_package_bridges": MAX_PACKAGE_BRIDGES,
        "canonical_package_bridge": CANONICAL_PACKAGE_BRIDGE,
        "canonical_public_command_prefix": CANONICAL_PUBLIC_COMMAND_PREFIX,
        "canonical_public_entrypoint": CANONICAL_PUBLIC_ENTRYPOINT,
        "retired_public_actions": sorted(RETIRED_PUBLIC_ACTIONS),
        "failures": failures,
        "contract_path": contract_path.as_posix()
        if isinstance(contract_path, Path)
        else contract_path,
    }


def render_public_command_budget_markdown(summary: Mapping[str, Any]) -> str:
    lines = [
        "# Public Command Budget Report",
        "",
        f"- status: `{summary['status']}`",
        f"- package_bridge_count: `{summary['package_bridge_count']}`",
        f"- workflow_action_count: `{summary['workflow_action_count']}`",
        f"- internal_action_count: `{summary['internal_action_count']}`",
        f"- operator_action_count: `{summary['operator_action_count']}`",
        f"- maintainer_action_count: `{summary['maintainer_action_count']}`",
        f"- canonical_package_bridge: `{summary['canonical_package_bridge']}`",
        f"- canonical_public_command_prefix: `{summary['canonical_public_command_prefix']}`",
        f"- canonical_public_entrypoint: `{summary['canonical_public_entrypoint']}`",
        "",
        "## Package bridges",
    ]
    for package_bridge in summary["package_bridges"]:
        lines.append(f"- `{package_bridge}`")
    lines.extend(["", "## Retired public actions"])
    for action in summary["retired_public_actions"]:
        lines.append(f"- `{action}`")
    lines.extend(["", "## Failures"])
    failures = summary["failures"]
    if failures:
        for failure in failures:
            lines.append(f"- {failure}")
    else:
        lines.append("- none")
    lines.append("")
    return "\n".join(lines)
