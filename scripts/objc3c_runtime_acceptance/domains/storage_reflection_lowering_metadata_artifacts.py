"""Compile artifact loading for storage/reflection lowering metadata cases."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs

from ..paths import ROOT


def load_storage_lowering_compile_artifacts(
    case_dir: Path,
    fixture_name: str,
    output_name: str,
) -> tuple[dict[str, Any], str]:
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / fixture_name
    _, ll_path, manifest_path = compile_fixture_outputs(fixture, case_dir / output_name)
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    return manifest, ll_path.read_text(encoding="utf-8")


__all__ = ["load_storage_lowering_compile_artifacts"]
