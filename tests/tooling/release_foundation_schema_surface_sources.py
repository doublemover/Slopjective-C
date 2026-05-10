from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from types import ModuleType
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file
from objc3c_shared.schema_registry import schema_path

SCRIPT_PATH = ROOT / "scripts" / "check_release_foundation_schema_surface.py"


def load_checker() -> ModuleType:
    spec = importlib.util.spec_from_file_location(
        "check_release_foundation_schema_surface",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError(
            "Unable to load scripts/check_release_foundation_schema_surface.py"
        )
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_json(path: Path) -> dict[str, Any]:
    return load_json_object(path)


def write_json(path: Path, payload: dict[str, Any]) -> None:
    write_json_file(path, payload, sort_keys=True)


def relative_schema_path(schema_id: str) -> str:
    return schema_path(schema_id).relative_to(ROOT).as_posix()


def temporary_summary_path() -> Path:
    return ROOT / "tmp" / "tests" / "release-foundation-schema-surface-summary.json"
