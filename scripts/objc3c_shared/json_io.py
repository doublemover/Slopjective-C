"""Shared deterministic JSON I/O and schema validation helpers."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import (
    canonical_json,
    load_json_any,
    load_json_array,
    load_json_object,
    load_optional_json_object,
    render_json,
    require_json_object,
    write_json_file,
    write_text_file,
)


class JsonSchemaValidationError(RuntimeError):
    pass


def validate_json_schema(payload: Any, schema: dict[str, Any], *, label: str = "payload") -> None:
    try:
        import jsonschema
    except ModuleNotFoundError as exc:
        raise JsonSchemaValidationError("jsonschema is required for schema validation") from exc

    try:
        jsonschema.Draft202012Validator.check_schema(schema)
        jsonschema.Draft202012Validator(schema).validate(payload)
    except jsonschema.ValidationError as exc:
        path = ".".join(str(part) for part in exc.absolute_path) or "$"
        raise JsonSchemaValidationError(f"{label} schema validation failed at {path}: {exc.message}") from exc
    except jsonschema.SchemaError as exc:
        raise JsonSchemaValidationError(f"{label} schema is invalid: {exc.message}") from exc


def load_json_with_schema(path: Path | str, schema_path: Path | str) -> Any:
    payload = load_json_any(path)
    schema = load_json_object(schema_path)
    validate_json_schema(payload, schema, label=str(path))
    return payload


def write_report_json(path: Path | str, payload: Any, *, sort_keys: bool = False) -> None:
    write_json_file(path, payload, sort_keys=sort_keys)
