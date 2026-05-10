from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"error: unable to read {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"error: invalid JSON in {display_path(path)}: "
            f"{exc.msg} at {exc.lineno}:{exc.colno}"
        ) from exc
