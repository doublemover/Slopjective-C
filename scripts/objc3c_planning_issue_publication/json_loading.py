"""JSON input loading for planning issue publication."""

from __future__ import annotations

import json
import os
from pathlib import Path
from typing import Any

from .contracts import (
    PublicationError,
    require_dict,
    validate_existing_report,
    validate_payload,
)
from .models import PublicationInputs
from .paths import assert_not_tmp_source, repo_path


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise PublicationError(f"missing JSON file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise PublicationError(f"invalid JSON in {path}: {exc}") from exc


def _canonical_json(
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


def write_json(
    path: Path | str,
    payload: Any,
    *,
    sort_keys: bool = False,
    ensure_ascii: bool = True,
    atomic: bool = True,
) -> None:
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    rendered = _canonical_json(payload, sort_keys=sort_keys, ensure_ascii=ensure_ascii)
    if not atomic:
        destination.write_text(rendered, encoding="utf-8", newline="\n")
        return

    tmp_path = destination.with_name(f".{destination.name}.tmp.{os.getpid()}")
    try:
        tmp_path.write_text(rendered, encoding="utf-8", newline="\n")
        os.replace(tmp_path, destination)
    finally:
        try:
            tmp_path.unlink()
        except FileNotFoundError:
            pass


def load_publication_inputs(payload: Path, report: Path, repo_override: str) -> PublicationInputs:
    payload_path = repo_path(payload)
    report_path = repo_path(report)
    assert_not_tmp_source(payload_path)

    payload_obj = require_dict(load_json(payload_path), "payload")
    validate_payload(payload_obj)

    existing_report_obj = load_json(report_path) if report_path.is_file() else {}
    existing_report = require_dict(existing_report_obj, "publication report")
    validate_existing_report(existing_report, payload_obj)

    repo = repo_override or payload_obj["repository"]
    payload_obj["repository"] = repo
    return PublicationInputs(
        payload_path=payload_path,
        report_path=report_path,
        payload=payload_obj,
        existing_report=existing_report,
        repo=repo,
    )


__all__ = ["load_json", "load_publication_inputs", "write_json"]
