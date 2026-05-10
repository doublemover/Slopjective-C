from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel


def require_json(path: Path, *, kind: str) -> dict[str, Any]:
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {repo_rel(path)}")
    return load_json(path)


def contract_id(payload: dict[str, Any]) -> str:
    return str(payload.get("contract_id", ""))


def validate_contracts(
    observed: dict[str, str],
    expected: dict[str, str],
    failures: list[str],
    *,
    label: str,
) -> None:
    for key, expected_contract in expected.items():
        if observed.get(key) != expected_contract:
            failures.append(f"{label} contract drifted for {key}")
