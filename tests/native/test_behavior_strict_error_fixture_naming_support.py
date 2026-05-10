from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NATIVE_ROOT = ROOT / "tests" / "native"

STRICT_ERROR_SUFFIXES = (
    "_strict_error.objc3",
    "_rejected.objc3",
    "_contract.objc3",
)
CURRENT_STRICT_ERROR_NAME_DEBT = {
    "tests/native/lowering/objc_runtime/numeric_zero_receiver_requires_runtime_dispatch.objc3",
    "tests/native/runtime/errors/runtime_dispatch_unresolved_symbol.objc3",
}


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def strict_error_sidecars() -> list[Path]:
    return [
        path
        for path in sorted(NATIVE_ROOT.glob("**/*.meta.json"))
        if load_json(path).get("fixture_kind") == "strict-error"
    ]
