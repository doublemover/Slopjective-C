from __future__ import annotations

import json
from pathlib import Path

from .constants import CONFIG_PATH, ROOT
from .models import ContractConfig


def load_contract_config() -> tuple[ContractConfig | None, list[str]]:
    errors: list[str] = []
    if not CONFIG_PATH.is_file():
        return None, [f"missing config file: {CONFIG_PATH}"]

    try:
        payload = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        return None, [f"invalid JSON in {CONFIG_PATH}: {exc}"]

    if not isinstance(payload, dict):
        return None, [f"config root must be an object: {CONFIG_PATH}"]

    if payload.get("contract_id") != "site-index-generator/v2":
        errors.append(
            "config contract_id drift: expected 'site-index-generator/v2' "
            f"observed {payload.get('contract_id')!r}"
        )

    def get_path_field(key: str) -> Path | None:
        value = payload.get(key)
        if not isinstance(value, str) or not value.strip():
            errors.append(f"config field {key!r} must be a non-empty string")
            return None
        return ROOT / value

    output_path = get_path_field("output_path")
    body_path = get_path_field("body_path")

    front_matter_lines = payload.get("front_matter")
    front_matter = ""
    if not isinstance(front_matter_lines, list) or not front_matter_lines:
        errors.append("config field 'front_matter' must be a non-empty string array")
    else:
        parsed_lines: list[str] = []
        for index, value in enumerate(front_matter_lines):
            if not isinstance(value, str):
                errors.append(
                    f"config field 'front_matter[{index}]' must be a string"
                )
                continue
            parsed_lines.append(value)
        if parsed_lines:
            front_matter = "\n".join(parsed_lines)
            if not front_matter.endswith("\n"):
                front_matter += "\n"

    if errors or output_path is None or body_path is None:
        return None, errors

    return (
        ContractConfig(
            output_path=output_path,
            body_path=body_path,
            front_matter=front_matter,
        ),
        [],
    )
