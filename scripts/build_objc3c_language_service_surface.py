#!/usr/bin/env python3
"""Replay Objective-C 3 language-service requests over the editor source graph."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT / "scripts") not in sys.path:
    sys.path.insert(0, str(ROOT / "scripts"))

from objc3c_editor_tooling.input_loading import (  # noqa: E402
    load_editor_tooling_inputs,
    publish_diagnostics_only_summary,
    run_frontend_compile,
)
from objc3c_editor_tooling.model import build_editor_tooling_model  # noqa: E402
from objc3c_editor_tooling.paths import (  # noqa: E402
    default_source_argument,
    paths_for_source,
    resolve_source,
)
from objc3c_editor_tooling.validation import compile_summary_exit_code  # noqa: E402
from objc3c_language_service import ObjectiveC3LanguageService, replay_requests  # noqa: E402
from objc3c_tooling.json_io import load_json_object, write_json_file  # noqa: E402
from objc3c_tooling.paths import display_path  # noqa: E402


DEFAULT_REQUESTS = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "language_service"
    / "request_replay.json"
)
REPORT_ROOT = ROOT / "tmp" / "reports" / "developer-tooling" / "language-service"


def _requests_from_payload(payload: dict[str, Any]) -> list[dict[str, Any]]:
    requests = payload.get("requests", [])
    if not isinstance(requests, list):
        raise ValueError("language-service replay fixture must contain a requests list")
    return [request for request in requests if isinstance(request, dict)]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", default=default_source_argument())
    parser.add_argument("--requests", default=str(DEFAULT_REQUESTS))
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    source = resolve_source(args.source)
    paths = paths_for_source(source)
    compile_result = run_frontend_compile(paths)
    if compile_result.stdout:
        sys.stdout.write(compile_result.stdout)
    if compile_result.stderr:
        sys.stderr.write(compile_result.stderr)

    summary_exit_code = compile_summary_exit_code(compile_result)
    if summary_exit_code is not None:
        if not publish_diagnostics_only_summary(paths, compile_result):
            return summary_exit_code

    inputs = load_editor_tooling_inputs(paths)
    editor_model = build_editor_tooling_model(paths, inputs)
    request_payload = load_json_object(Path(args.requests))
    service = ObjectiveC3LanguageService(
        source_graph=editor_model.source_graph,
        source_texts={paths.source.display_path: inputs.source_text},
    )
    replay = replay_requests(service, _requests_from_payload(request_payload))
    replay = {
        **replay,
        "source_path": paths.source.display_path,
        "source_graph_digest": editor_model.source_graph.get("source_graph_digest", ""),
        "request_fixture": display_path(Path(args.requests).resolve()),
    }
    report_dir = REPORT_ROOT / paths.source.slug
    report_dir.mkdir(parents=True, exist_ok=True)
    report_path = report_dir / "language-service-replay.json"
    write_json_file(report_path, replay)
    print(f"language_service_path: {display_path(report_path)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
