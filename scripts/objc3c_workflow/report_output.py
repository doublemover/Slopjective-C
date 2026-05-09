"""JSON stdout emission for workflow reports."""

from __future__ import annotations

import sys

from scripts.objc3c_shared.json_io import render_json


def emit_json(payload: object) -> int:
    sys.stdout.write(render_json(payload))
    return 0


__all__ = ["emit_json"]
