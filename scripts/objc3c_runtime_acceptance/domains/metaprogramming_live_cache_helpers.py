"""Helpers for live metaprogramming host-cache acceptance cases."""

from __future__ import annotations

import json
import shutil
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT


def remove_metaprogramming_cache_entry_from_artifact(artifact: dict[str, Any]) -> bool:
    relative_entry = artifact.get("cache_entry_relative_path")
    if not isinstance(relative_entry, str) or relative_entry == "":
        return False
    cache_entry = ROOT / Path(relative_entry)
    allowed_root = ROOT / "tmp" / "artifacts" / "objc3c-native" / "cache" / "metaprogramming"
    try:
        cache_entry.resolve().relative_to(allowed_root.resolve())
    except ValueError:
        return False
    if cache_entry.is_dir():
        shutil.rmtree(cache_entry)
        return True
    if cache_entry.is_file():
        cache_entry.unlink()
        return True
    return False


def build_metaprogramming_cache_seed_macros(
    materialization_attempt: int,
    unique_suffix: str,
) -> str:
    return "".join(
        [
            "\n"
            f"pure fn cacheSeed{materialization_attempt}_{index}() -> i32 "
            '__attribute__((objc_macro(named("Trace")), '
            f'objc_macro_package(named("std.metaprogramming.trace.{materialization_attempt}")), '
            f'objc_macro_provenance(named("sha256:{unique_suffix}{index:02d}")), '
            f'objc_macro_cache_key(named("Trace:v1:{unique_suffix}:{materialization_attempt}:{index}")), '
            'objc_macro_sandbox(named("deterministic")))) {\n'
            f"  return {17 + index};\n"
            "}\n"
            for index in range(materialization_attempt)
        ]
    )


def compile_live_metaprogramming_cache_candidate(
    temp_provider_fixture: Path,
    compile_dir: Path,
    cache_root_args: list[str],
) -> tuple[Path, Path, dict[str, Any], dict[str, Any], dict[str, Any]]:
    compile_fixture_with_args(
        temp_provider_fixture,
        compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "1",
            *cache_root_args,
        ],
    )
    host_cache_artifact_path = compile_dir / "module.metaprogramming-macro-host-cache.json"
    runtime_import_path = compile_dir / "module.runtime-import-surface.json"
    host_cache_artifact = json.loads(host_cache_artifact_path.read_text(encoding="utf-8"))
    runtime_import_surface = json.loads(runtime_import_path.read_text(encoding="utf-8"))
    host_cache_import_surface = runtime_import_surface.get(
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration", {}
    )
    return (
        host_cache_artifact_path,
        runtime_import_path,
        host_cache_artifact,
        runtime_import_surface,
        host_cache_import_surface,
    )
