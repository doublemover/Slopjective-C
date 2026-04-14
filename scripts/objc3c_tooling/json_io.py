"""JSON and text file helpers for repository tooling scripts."""

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def load_json_any(path: Path | str) -> Any:
    return json.loads(Path(path).read_text(encoding="utf-8"))


def load_json_object(path: Path | str) -> dict[str, Any]:
    payload = load_json_any(path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {display_path(Path(path))}")
    return payload


def load_json_array(path: Path | str) -> list[Any]:
    payload = load_json_any(path)
    if not isinstance(payload, list):
        raise RuntimeError(f"expected JSON array at {display_path(Path(path))}")
    return payload


def canonical_json(payload: Any, *, sort_keys: bool = False) -> str:
    return json.dumps(payload, indent=2, sort_keys=sort_keys) + "\n"


def _atomic_write_text(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp_path = path.with_name(f".{path.name}.tmp.{os.getpid()}")
    try:
        tmp_path.write_text(content, encoding="utf-8", newline="\n")
        os.replace(tmp_path, path)
    finally:
        try:
            tmp_path.unlink()
        except FileNotFoundError:
            pass


def write_text_file(path: Path | str, content: str, *, atomic: bool = True) -> None:
    destination = Path(path)
    if atomic:
        _atomic_write_text(destination, content)
        return
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(content, encoding="utf-8", newline="\n")


def write_json_file(
    path: Path | str,
    payload: Any,
    *,
    sort_keys: bool = False,
    atomic: bool = True,
) -> None:
    write_text_file(path, canonical_json(payload, sort_keys=sort_keys), atomic=atomic)


def render_json(payload: Any, *, sort_keys: bool = False) -> str:
    return canonical_json(payload, sort_keys=sort_keys)

