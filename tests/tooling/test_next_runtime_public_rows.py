from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECK_PATH = ROOT / "scripts" / "check_objc3c_next_runtime_public_rows.py"
SRC_ROOT = ROOT / "native" / "objc3c" / "src"
RUNTIME_UMBRELLA_PATH = (
    SRC_ROOT / "runtime" / "public" / "objc3_runtime_api.h"
)
STDLIB_FOUNDATION_NEXT_PROBE_PATH = (
    ROOT / "tests" / "tooling" / "runtime" / "stdlib_foundation_next_runtime_probe.cpp"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_check_module():
    spec = importlib.util.spec_from_file_location(
        "check_objc3c_next_runtime_public_rows",
        CHECK_PATH,
    )
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_next_runtime_public_rows_are_issue_backed_and_replayable() -> None:
    module = _load_check_module()

    report = module.validate_next_runtime_public_rows()

    assert report["status"] == "PASS"
    assert report["issues"] == [8154, 8155]
    assert "runtime.object-model.full-realization" in report["reserved_umbrella_rows"]
    assert "language.advanced-runtime-closure" in report["reserved_umbrella_rows"]


def test_foundation_next_stdlib_runtime_contracts_are_public_umbrella_exports() -> None:
    umbrella = _read(RUNTIME_UMBRELLA_PATH)
    probe = _read(STDLIB_FOUNDATION_NEXT_PROBE_PATH)
    contracts = {
        "runtime/stdlib/core_runtime_contract.h": (
            "objc3_runtime_stdlib_core_language_revision_i32",
        ),
        "runtime/stdlib/text_runtime_contract.h": (
            "objc3_runtime_stdlib_text_utf8_literal_i32",
            "objc3_runtime_stdlib_text_concat_i32",
        ),
        "runtime/stdlib/collections_runtime_contract.h": (
            "objc3_runtime_stdlib_collections_array3_i32",
            "objc3_runtime_stdlib_collections_map_lookup_or_i32",
        ),
    }

    assert '#include "runtime/public/objc3_runtime_api.h"' in probe
    assert '#include "runtime/stdlib/' not in probe

    for include_path, symbols in contracts.items():
        assert f'#include "{include_path}"' in umbrella
        contract_header = _read(SRC_ROOT / Path(*include_path.split("/")))
        for symbol in symbols:
            assert f"{symbol}(" in contract_header
