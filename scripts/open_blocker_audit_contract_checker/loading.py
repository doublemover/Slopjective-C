from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import display_path


def load_json(path: Path, *, label: str) -> dict[str, Any]:
    try:
        payload = load_json_object(path)
    except FileNotFoundError:
        raise ValueError(f"{label} file does not exist: {display_path(path)}") from None
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"{label} is not valid JSON: {exc.msg} at {exc.lineno}:{exc.colno}"
        ) from None
    if not isinstance(payload, dict):
        raise ValueError(f"{label} root must be a JSON object")
    return payload
