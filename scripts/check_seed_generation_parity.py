#!/usr/bin/env python3
"""Check deterministic parity for v0.13 seed graph and payload artifacts."""

from __future__ import annotations

import argparse
import difflib
import importlib.util
import io
import json
import sys
import tempfile
from contextlib import redirect_stderr, redirect_stdout
from dataclasses import dataclass
from pathlib import Path
from types import ModuleType
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
GENERATE_BATCHES_SCRIPT = ROOT / "scripts/generate_seed_batches.py"
GENERATE_PAYLOADS_SCRIPT = ROOT / "scripts/generate_seed_issue_payloads.py"
MAX_DIFF_LINES = 80


class ParityError(ValueError):
    """Raised when parity checking cannot complete deterministically."""


@dataclass(frozen=True)
class ScriptRunResult:
    code: int
    stdout: str
    stderr: str


@dataclass(frozen=True)
class ArtifactMismatch:
    artifact_name: str
    artifact_path: Path
    classification: str
    first_mismatch_path: str | None
    diff_preview: str


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_seed_generation_parity.py",
        description=(
            "Regenerate v0.13 seed artifacts deterministically and verify parity "
            "for graph and payload JSON files."
        ),
    )
    parser.add_argument(
        "--matrix",
        type=Path,
        required=True,
        help="Path to v0.13 seed matrix markdown source.",
    )
    parser.add_argument(
        "--owner-map-json",
        type=Path,
        required=True,
        help="Path to deterministic seed owner registry JSON.",
    )
    parser.add_argument(
        "--graph-path",
        type=Path,
        required=True,
        help="Path to v0.13 seed dependency graph JSON artifact.",
    )
    parser.add_argument(
        "--payload-path",
        type=Path,
        required=True,
        help="Path to v0.13 seed issue payload JSON artifact.",
    )
    return parser


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def shell_quote(token: str) -> str:
    if any(char.isspace() for char in token):
        return f'"{token}"'
    return token


def render_command(args: list[str]) -> str:
    return " ".join(shell_quote(arg) for arg in args)


def ensure_file_exists(path: Path, *, context: str) -> Path:
    resolved = path.resolve()
    if not resolved.is_file():
        raise ParityError(f"{context} file not found: {display_path(resolved)}")
    return resolved


def load_script_module(path: Path, module_name: str) -> ModuleType:
    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise ParityError(f"unable to load script module: {display_path(path)}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module


def run_script_main(module: ModuleType, *, args: list[str], context: str) -> ScriptRunResult:
    main_func = getattr(module, "main", None)
    if not callable(main_func):
        raise ParityError(f"{context} script does not expose callable main(argv)")

    stdout_buffer = io.StringIO()
    stderr_buffer = io.StringIO()
    try:
        with redirect_stdout(stdout_buffer), redirect_stderr(stderr_buffer):
            code = main_func(args)
    except Exception as exc:
        raise ParityError(f"{context} regeneration failed with exception: {exc}") from exc

    if not isinstance(code, int):
        raise ParityError(f"{context} script returned non-integer exit code: {code!r}")
    return ScriptRunResult(code=code, stdout=stdout_buffer.getvalue(), stderr=stderr_buffer.getvalue())


def format_script_failure(
    *,
    context: str,
    invocation_command: str,
    replay_command: str,
    parity_command: str,
    result: ScriptRunResult,
) -> str:
    stdout = result.stdout.strip() or "<empty>"
    stderr = result.stderr.strip() or "<empty>"
    return (
        f"{context} regeneration failed with exit code {result.code}\n"
        f"invocation: {invocation_command}\n"
        f"stdout: {stdout}\n"
        f"stderr: {stderr}\n"
        "remediation steps:\n"
        f"1. Resolve the {context} regeneration error reported above.\n"
        f"2. Re-run {context} regeneration:\n{replay_command}\n"
        f"3. Re-run parity checker:\n{parity_command}"
    )


def read_utf8_text(path: Path, *, context: str) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except FileNotFoundError as exc:
        raise ParityError(f"{context} file not found: {display_path(path)}") from exc
    except UnicodeDecodeError as exc:
        raise ParityError(f"{context} file is not valid UTF-8: {display_path(path)}") from exc


def normalize_newlines(text: str) -> str:
    return text.replace("\r\n", "\n").replace("\r", "\n")


def parse_json_document(text: str, *, context: str, path: Path) -> Any:
    try:
        return json.loads(text)
    except json.JSONDecodeError as exc:
        raise ParityError(
            f"{context} is not valid JSON ({display_path(path)}): "
            f"line {exc.lineno}, col {exc.colno}"
        ) from exc


def first_mismatch_path(expected: Any, actual: Any, *, path: str = "$") -> str:
    if type(expected) is not type(actual):
        return f"{path} (type {type(expected).__name__} != {type(actual).__name__})"

    if isinstance(expected, dict):
        expected_keys = sorted(expected.keys())
        actual_keys = sorted(actual.keys())
        if expected_keys != actual_keys:
            missing = sorted(set(expected_keys) - set(actual_keys))
            if missing:
                return f"{path}.{missing[0]} (missing from regenerated artifact)"
            extra = sorted(set(actual_keys) - set(expected_keys))
            if extra:
                return f"{path}.{extra[0]} (unexpected in regenerated artifact)"
            return f"{path} (object key mismatch)"

        for key in expected_keys:
            candidate = first_mismatch_path(expected[key], actual[key], path=f"{path}.{key}")
            if candidate:
                return candidate
        return ""

    if isinstance(expected, list):
        if len(expected) != len(actual):
            return f"{path} (length {len(expected)} != {len(actual)})"
        for index, (expected_item, actual_item) in enumerate(zip(expected, actual, strict=True)):
            candidate = first_mismatch_path(expected_item, actual_item, path=f"{path}[{index}]")
            if candidate:
                return candidate
        return ""

    if expected != actual:
        return path

    return ""


def truncate_diff_lines(lines: list[str]) -> list[str]:
    if len(lines) > MAX_DIFF_LINES:
        return lines[:MAX_DIFF_LINES] + [f"... diff truncated at {MAX_DIFF_LINES} lines"]
    return lines


def build_json_diff_preview(expected: Any, regenerated: Any) -> str:
    expected_text = json.dumps(expected, indent=2, sort_keys=True) + "\n"
    regenerated_text = json.dumps(regenerated, indent=2, sort_keys=True) + "\n"
    lines = list(
        difflib.unified_diff(
            expected_text.splitlines(),
            regenerated_text.splitlines(),
            fromfile="artifact",
            tofile="regenerated",
            lineterm="",
        )
    )
    lines = truncate_diff_lines(lines)
    if not lines:
        return "<no diff output>"
    return "\n".join(lines)


def build_text_diff_preview(expected_text: str, regenerated_text: str) -> str:
    lines = list(
        difflib.unified_diff(
            expected_text.splitlines(),
            regenerated_text.splitlines(),
            fromfile="artifact",
            tofile="regenerated",
            lineterm="",
        )
    )
    lines = truncate_diff_lines(lines)
    if not lines:
        return "<no diff output>"
    return "\n".join(lines)


def compare_artifact_parity(
    *,
    artifact_name: str,
    artifact_path: Path,
    regenerated_path: Path,
) -> ArtifactMismatch | None:
    artifact_text = normalize_newlines(
        read_utf8_text(artifact_path, context=f"{artifact_name} artifact")
    )
    regenerated_text = normalize_newlines(
        read_utf8_text(regenerated_path, context=f"{artifact_name} regenerated artifact")
    )

    if artifact_text == regenerated_text:
        return None

    artifact_payload = parse_json_document(
        artifact_text,
        context=f"{artifact_name} artifact",
        path=artifact_path,
    )
    regenerated_payload = parse_json_document(
        regenerated_text,
        context=f"{artifact_name} regenerated artifact",
        path=regenerated_path,
    )

    if artifact_payload == regenerated_payload:
        return ArtifactMismatch(
            artifact_name=artifact_name,
            artifact_path=artifact_path,
            classification="serialization-drift",
            first_mismatch_path=None,
            diff_preview=build_text_diff_preview(artifact_text, regenerated_text),
        )

    mismatch_path = first_mismatch_path(artifact_payload, regenerated_payload) or "$"
    return ArtifactMismatch(
        artifact_name=artifact_name,
        artifact_path=artifact_path,
        classification="semantic-drift",
        first_mismatch_path=mismatch_path,
        diff_preview=build_json_diff_preview(artifact_payload, regenerated_payload),
    )


def render_mismatch_diagnostic(mismatch: ArtifactMismatch) -> str:
    lines = [
        f"{mismatch.artifact_name} artifact mismatch for {display_path(mismatch.artifact_path)}",
        f"classification: {mismatch.classification}",
    ]
    if mismatch.first_mismatch_path is None:
        lines.append("first mismatch path: n/a (parsed JSON payloads are equal)")
        lines.append("diff (normalized text preview):")
    else:
        lines.append(f"first mismatch path: {mismatch.first_mismatch_path}")
        lines.append("diff (canonicalized JSON preview):")
    lines.append(mismatch.diff_preview)
    return "\n".join(lines)


def render_remediation_guidance(
    *,
    graph_fix_command: str,
    payload_fix_command: str,
    parity_command: str,
) -> str:
    return (
        "remediation steps:\n"
        "1. Regenerate graph artifact:\n"
        f"{graph_fix_command}\n"
        "2. Regenerate payload artifact:\n"
        f"{payload_fix_command}\n"
        "3. Re-run parity checker:\n"
        f"{parity_command}"
    )


def check_parity(
    *,
    matrix_path: Path,
    owner_map_path: Path,
    graph_path: Path,
    payload_path: Path,
) -> int:
    matrix = ensure_file_exists(matrix_path, context="matrix").resolve()
    owner_map = ensure_file_exists(owner_map_path, context="owner map").resolve()
    graph = ensure_file_exists(graph_path, context="graph artifact").resolve()
    payload = ensure_file_exists(payload_path, context="payload artifact").resolve()

    graph_fix_command = render_command(
        [
            "python",
            "scripts/generate_seed_batches.py",
            "--matrix",
            display_path(matrix),
            "--owner-map-json",
            display_path(owner_map),
            "--output",
            display_path(graph),
        ]
    )
    payload_fix_command = render_command(
        [
            "python",
            "scripts/generate_seed_issue_payloads.py",
            "--graph",
            display_path(graph),
            "--output",
            display_path(payload),
        ]
    )
    parity_command = render_command(
        [
            "python",
            "scripts/check_seed_generation_parity.py",
            "--matrix",
            display_path(matrix),
            "--owner-map-json",
            display_path(owner_map),
            "--graph-path",
            display_path(graph),
            "--payload-path",
            display_path(payload),
        ]
    )

    with tempfile.TemporaryDirectory(prefix="seed-generation-parity-") as temp_dir:
        temp_root = Path(temp_dir)
        regenerated_graph = temp_root / "regenerated_seed_dependency_graph.json"
        regenerated_payload = temp_root / "regenerated_seed_issue_payloads.json"

        batches_module = load_script_module(
            GENERATE_BATCHES_SCRIPT, module_name="_seed_parity_generate_seed_batches"
        )
        graph_regen_args = [
            "--matrix",
            str(matrix),
            "--owner-map-json",
            str(owner_map),
            "--output",
            str(regenerated_graph),
        ]
        graph_result = run_script_main(
            batches_module,
            args=graph_regen_args,
            context="graph",
        )
        if graph_result.code != 0:
            invocation_command = render_command(
                [
                    "python",
                    "scripts/generate_seed_batches.py",
                    "--matrix",
                    display_path(matrix),
                    "--owner-map-json",
                    display_path(owner_map),
                    "--output",
                    "<tmp>/regenerated_seed_dependency_graph.json",
                ]
            )
            raise ParityError(
                format_script_failure(
                    context="graph",
                    invocation_command=invocation_command,
                    replay_command=graph_fix_command,
                    parity_command=parity_command,
                    result=graph_result,
                )
            )

        payloads_module = load_script_module(
            GENERATE_PAYLOADS_SCRIPT, module_name="_seed_parity_generate_seed_issue_payloads"
        )
        payload_regen_args = [
            "--graph",
            str(graph),
            "--output",
            str(regenerated_payload),
        ]
        payload_result = run_script_main(
            payloads_module,
            args=payload_regen_args,
            context="payload",
        )
        if payload_result.code != 0:
            invocation_command = render_command(
                [
                    "python",
                    "scripts/generate_seed_issue_payloads.py",
                    "--graph",
                    display_path(graph),
                    "--output",
                    "<tmp>/regenerated_seed_issue_payloads.json",
                ]
            )
            raise ParityError(
                format_script_failure(
                    context="payload",
                    invocation_command=invocation_command,
                    replay_command=payload_fix_command,
                    parity_command=parity_command,
                    result=payload_result,
                )
            )

        mismatches: list[ArtifactMismatch] = []
        graph_mismatch = compare_artifact_parity(
            artifact_name="graph",
            artifact_path=graph,
            regenerated_path=regenerated_graph,
        )
        if graph_mismatch:
            mismatches.append(graph_mismatch)

        payload_mismatch = compare_artifact_parity(
            artifact_name="payload",
            artifact_path=payload,
            regenerated_path=regenerated_payload,
        )
        if payload_mismatch:
            mismatches.append(payload_mismatch)

        if mismatches:
            print(
                "seed-generation-parity: parity check failed "
                f"({len(mismatches)} artifact mismatch(es)).",
                file=sys.stderr,
            )
            print("drift classifications:", file=sys.stderr)
            for mismatch in mismatches:
                print(
                    f"- {mismatch.artifact_name}: {mismatch.classification}",
                    file=sys.stderr,
                )
            for mismatch in mismatches:
                print("", file=sys.stderr)
                print(render_mismatch_diagnostic(mismatch), file=sys.stderr)
            print("", file=sys.stderr)
            print("regeneration commands:", file=sys.stderr)
            print(graph_fix_command, file=sys.stderr)
            print(payload_fix_command, file=sys.stderr)
            print("", file=sys.stderr)
            print(
                render_remediation_guidance(
                    graph_fix_command=graph_fix_command,
                    payload_fix_command=payload_fix_command,
                    parity_command=parity_command,
                ),
                file=sys.stderr,
            )
            return 1

    print(
        "seed-generation-parity: OK "
        f"(matrix={display_path(matrix)}, owner_map={display_path(owner_map)}, "
        f"graph={display_path(graph)}, payload={display_path(payload)})"
    )
    return 0


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return check_parity(
            matrix_path=args.matrix,
            owner_map_path=args.owner_map_json,
            graph_path=args.graph_path,
            payload_path=args.payload_path,
        )
    except ParityError as exc:
        print(f"seed-generation-parity: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
