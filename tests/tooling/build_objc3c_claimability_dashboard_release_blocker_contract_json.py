from __future__ import annotations

import json
from pathlib import Path


def read_dashboard_release_blocker_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload
