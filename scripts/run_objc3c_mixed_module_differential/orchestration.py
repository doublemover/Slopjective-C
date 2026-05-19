"""Main orchestration for mixed-module differential runs."""

from __future__ import annotations

import argparse
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence

from .cli import parse_args
from .commands import CASE_RUNNERS
from .config import RunnerConfig
from .fixtures import load_manifest, prepare_run_root
from .models import CaseResult
from .native import NativeToolResolver
from .reporting import build_summary_payload, build_surfaces, emit_result, write_json


def run_cases(
    case_ids: list[str],
    run_root: Path,
    native_tools: NativeToolResolver | None = None,
) -> list[CaseResult]:
    tools = native_tools or NativeToolResolver()
    results: list[CaseResult] = []
    for case_id in case_ids:
        command = CASE_RUNNERS[case_id]
        clangxx = tools.clangxx() if command.requires_clangxx else None
        result = command(run_root, clangxx)
        if result.case_id != command.expected_result_case_id:
            raise RuntimeError(f"mixed-module differential runner drifted for case {case_id}")
        if result.passed is not True:
            raise RuntimeError(f"mixed-module differential case failed: {case_id}")
        results.append(result)
    return results


def run_mixed_module_differential(args: argparse.Namespace) -> int:
    config = RunnerConfig.from_args(args)
    manifest = load_manifest(config.manifest_path)
    run_root = prepare_run_root()
    results = run_cases(manifest["case_ids"], run_root)
    surfaces = build_surfaces(manifest["surface_contracts"], results)

    payload = build_summary_payload(
        generated_at_utc=datetime.now(timezone.utc).isoformat(),
        manifest_path=config.manifest_path,
        run_root=run_root,
        manifest=manifest,
        results=results,
        surfaces=surfaces,
    )
    config.summary_out.parent.mkdir(parents=True, exist_ok=True)
    write_json(config.summary_out, payload)
    emit_result(payload, config.summary_out, config.contract_mode)
    return 0


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    return run_mixed_module_differential(args)


__all__ = ["main", "run_cases", "run_mixed_module_differential"]
