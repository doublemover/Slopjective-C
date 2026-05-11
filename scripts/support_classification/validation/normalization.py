"""Input normalization for support classification validation."""

from __future__ import annotations

from typing import Any

from .contracts import ContractError
from .contracts import PathExists
from .rules import uses_tmp_source_truth


def require_list(contract: dict[str, Any], key: str) -> list[Any]:
    value = contract.get(key)
    if not isinstance(value, list):
        raise ContractError(f"contract field `{key}` must be a list")
    return value


def require_string(value: Any, label: str) -> str:
    if not isinstance(value, str) or not value:
        raise ContractError(f"{label} must be a non-empty string")
    return value


def normalize_checked_in_path(
    path_text: str,
    *,
    label: str,
    path_exists: PathExists,
) -> str:
    if uses_tmp_source_truth(path_text):
        raise ContractError(f"{label} must not use tmp as source truth")
    if not path_exists(path_text):
        raise ContractError(f"missing checked-in surface path `{path_text}`")
    return normalize_path_separators(path_text)


def normalize_path_separators(path_text: str) -> str:
    return path_text.replace("\\", "/")


def require_existing_contract_path(
    contract: dict[str, Any],
    *,
    key: str,
    path_exists: PathExists,
) -> str:
    path_text = require_string(contract.get(key), key)
    if not path_exists(path_text):
        raise ContractError(f"{key} path is missing: `{path_text}`")
    return normalize_path_separators(path_text)


__all__ = [
    "normalize_checked_in_path",
    "normalize_path_separators",
    "require_existing_contract_path",
    "require_list",
    "require_string",
]
