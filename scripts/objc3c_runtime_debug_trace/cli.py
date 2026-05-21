"""CLI entrypoint for deterministic runtime debug trace generation."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import ROOT, display_path

from .contracts import DEFAULT_TRACE_SOURCE, RUNTIME_DEBUG_TRACE_SUMMARY_PATH
from .orchestration import build_runtime_debug_trace_from_live_actions


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "source",
        nargs="?",
        default=DEFAULT_TRACE_SOURCE.relative_to(ROOT).as_posix(),
    )
    parser.add_argument("--trace-out", type=Path, default=RUNTIME_DEBUG_TRACE_SUMMARY_PATH)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    rc, payload = build_runtime_debug_trace_from_live_actions(
        source_path=str(args.source).replace("\\", "/"),
        trace_out=args.trace_out,
    )
    print(f"summary_path: {display_path(args.trace_out)}")
    print(f"trace_path: {display_path(args.trace_out)}")
    if payload.get("ok") is True:
        print("runtime-debug-trace: PASS")
    else:
        print("runtime-debug-trace: FAIL", file=sys.stderr)
        for failure in payload.get("failures", []):
            print(f"- {failure}", file=sys.stderr)
    return rc
