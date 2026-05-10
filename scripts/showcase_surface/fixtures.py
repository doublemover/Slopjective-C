from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any


MODULE_DECL_RE = re.compile(r"^\s*module\s+([A-Za-z_][A-Za-z0-9_]*)\s*;", re.MULTILINE)


def load_json_payload(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def read_module_name(source_path: Path) -> str | None:
    source_text = source_path.read_text(encoding="utf-8")
    module_match = MODULE_DECL_RE.search(source_text)
    return None if module_match is None else module_match.group(1)
