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


def _load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def _strict_error_sidecars() -> list[Path]:
    return [
        path
        for path in sorted(NATIVE_ROOT.glob("**/*.meta.json"))
        if _load_json(path).get("fixture_kind") == "strict-error"
    ]


def test_strict_error_fixture_sidecars_match_existing_sources() -> None:
    strict_error_sidecars = _strict_error_sidecars()
    assert strict_error_sidecars

    for sidecar_path in strict_error_sidecars:
        metadata = _load_json(sidecar_path)
        source_path = sidecar_path.with_name(metadata["fixture"])
        assert source_path.is_file(), source_path.relative_to(ROOT).as_posix()
        assert metadata["boundary"]["behavior_contract"] == "canonical-strict-error"
        assert metadata["expected"]["diagnostic_code"]
        assert metadata["expected"]["required_tokens"]


def test_strict_error_fixture_names_are_explicit_or_tracked_debt() -> None:
    nonconforming_names = {
        sidecar_path.with_name(_load_json(sidecar_path)["fixture"]).relative_to(ROOT).as_posix()
        for sidecar_path in _strict_error_sidecars()
        if not _load_json(sidecar_path)["fixture"].endswith(STRICT_ERROR_SUFFIXES)
    }

    assert nonconforming_names == CURRENT_STRICT_ERROR_NAME_DEBT
