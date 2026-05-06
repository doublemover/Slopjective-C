"""Shared deterministic JSON I/O and schema validation helpers."""

from __future__ import annotations

import json
import os
from collections.abc import Mapping
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]


class JsonSchemaValidationError(RuntimeError):
    pass


def _display_path(path: Path | str) -> str:
    candidate = Path(path)
    absolute = candidate.resolve()
    root = ROOT.resolve()
    try:
        return absolute.relative_to(root).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_json_any(path: Path | str) -> Any:
    return json.loads(Path(path).read_text(encoding="utf-8"))


def load_json_object(path: Path | str) -> dict[str, Any]:
    payload = load_json_any(path)
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {_display_path(path)}")
    return payload


def require_json_object(path: Path | str) -> dict[str, Any]:
    source = Path(path)
    if not source.is_file():
        raise RuntimeError(f"expected JSON artifact was not published: {_display_path(source)}")
    return load_json_object(source)


def load_optional_json_object(path: Path | str) -> dict[str, Any] | None:
    source = Path(path)
    if not source.is_file():
        return None
    return load_json_object(source)


def load_json_array(path: Path | str) -> list[Any]:
    payload = load_json_any(path)
    if not isinstance(payload, list):
        raise RuntimeError(f"expected JSON array at {_display_path(path)}")
    return payload


def canonical_json(
    payload: Any,
    *,
    sort_keys: bool = False,
    ensure_ascii: bool = True,
) -> str:
    return json.dumps(
        payload,
        indent=2,
        sort_keys=sort_keys,
        ensure_ascii=ensure_ascii,
        allow_nan=False,
    ) + "\n"


def render_json(payload: Any, *, sort_keys: bool = False, ensure_ascii: bool = True) -> str:
    return canonical_json(payload, sort_keys=sort_keys, ensure_ascii=ensure_ascii)


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
    ensure_ascii: bool = True,
    atomic: bool = True,
) -> None:
    write_text_file(
        path,
        canonical_json(payload, sort_keys=sort_keys, ensure_ascii=ensure_ascii),
        atomic=atomic,
    )


def validate_json_schema(payload: Any, schema: Mapping[str, Any], *, label: str = "payload") -> None:
    try:
        import jsonschema
    except ModuleNotFoundError as exc:
        raise JsonSchemaValidationError("jsonschema is required for schema validation") from exc

    try:
        jsonschema.Draft202012Validator.check_schema(schema)
        validator = jsonschema.Draft202012Validator(schema)
        errors = sorted(
            validator.iter_errors(payload),
            key=lambda error: (
                tuple(str(part) for part in error.absolute_path),
                tuple(str(part) for part in error.absolute_schema_path),
                error.message,
            ),
        )
    except jsonschema.SchemaError as exc:
        raise JsonSchemaValidationError(f"{label} schema is invalid: {exc.message}") from exc

    if not errors:
        return

    error = errors[0]
    path = ".".join(str(part) for part in error.absolute_path) or "$"
    raise JsonSchemaValidationError(f"{label} schema validation failed at {path}: {error.message}")


def load_json_with_schema(path: Path | str, schema_path: Path | str) -> Any:
    payload = load_json_any(path)
    schema = load_json_object(schema_path)
    validate_json_schema(payload, schema, label=_display_path(path))
    return payload


def write_report_json(
    path: Path | str,
    payload: Any,
    *,
    sort_keys: bool = True,
    ensure_ascii: bool = True,
    schema: Mapping[str, Any] | None = None,
    atomic: bool = True,
) -> None:
    if schema is not None:
        validate_json_schema(payload, schema, label=_display_path(path))
    write_json_file(path, payload, sort_keys=sort_keys, ensure_ascii=ensure_ascii, atomic=atomic)
