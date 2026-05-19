"""Publication orchestration for the release/runtime claim matrix."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file

from .load import load_dependency_cases
from .model import build_matrix
from .probes import ensure_native_build, run_matrix_probes
from .render import render_markdown


def publish_matrix(json_out: Path, md_out: Path) -> None:
    json_out.parent.mkdir(parents=True, exist_ok=True)
    md_out.parent.mkdir(parents=True, exist_ok=True)

    ensure_native_build()
    dependency_cases = load_dependency_cases()
    probes = run_matrix_probes()

    native_report = load_json(probes.native_report_path)
    native_publication = load_json(probes.native_publication_path)
    validation_payload = load_json(probes.validation_path)
    runner_report = load_json(probes.runner_report_path)
    runner_publication = load_json(probes.runner_publication_path)

    matrix = build_matrix(
        dependency_cases=dependency_cases,
        probes=probes,
        native_report=native_report,
        native_publication=native_publication,
        validation_payload=validation_payload,
        runner_report=runner_report,
        runner_publication=runner_publication,
    )
    write_json_file(json_out, matrix)
    md_out.write_text(render_markdown(probes), encoding="utf-8")
