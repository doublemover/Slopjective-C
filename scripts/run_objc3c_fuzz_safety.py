#!/usr/bin/env python3
"""Deterministic stress/fuzz safety gate for objc3c parser and semantic passes."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
DEFAULT_OUT_ROOT = ROOT / "tmp" / "artifacts" / "objc3c-native" / "fuzz-safety"
MODE = "objc3c-fuzz-safety-v1"
SCHEMA_VERSION = "objc3c-fuzz-safety-report.v1"

EXIT_OK = 0
EXIT_GUARD_FAIL = 1
EXIT_INPUT_ERROR = 2

DIAGNOSTIC_SIGNAL_PATTERN = re.compile(r"(error|O3[A-Z]\d{3})", re.IGNORECASE)


class InputError(ValueError):
    """Raised when CLI input cannot be normalized safely."""


@dataclass(frozen=True)
class CorpusCase:
    case_id: str
    subsystem: str
    source: str


@dataclass(frozen=True)
class MutationRule:
    name: str
    transform: Callable[[str], str]


BASE_CORPUS: tuple[CorpusCase, ...] = (
    CorpusCase(
        case_id="parser_missing_rbrace",
        subsystem="parser",
        source=(
            "module FuzzParserMissingRBrace;\n"
            "fn main() -> int {\n"
            "  return 1;\n"
        ),
    ),
    CorpusCase(
        case_id="parser_missing_semicolon",
        subsystem="parser",
        source=(
            "module FuzzParserMissingSemicolon;\n"
            "fn main() -> int {\n"
            "  return 1\n"
            "}\n"
        ),
    ),
    CorpusCase(
        case_id="parser_unterminated_message_send",
        subsystem="parser",
        source=(
            "module FuzzParserUnterminatedMessage;\n"
            "fn main() -> int {\n"
            "  return [obj value:\n"
            "}\n"
        ),
    ),
    CorpusCase(
        case_id="parser_missing_while_rparen",
        subsystem="parser",
        source=(
            "module FuzzParserMissingWhileRParen;\n"
            "fn main() -> int {\n"
            "  while (true {\n"
            "    return 0;\n"
            "  }\n"
            "}\n"
        ),
    ),
    CorpusCase(
        case_id="sema_duplicate_symbol",
        subsystem="semantic",
        source=(
            "module FuzzSemaDuplicateSymbol;\n"
            "fn value() -> int { return 1; }\n"
            "fn value() -> int { return 2; }\n"
            "fn main() -> int { return value(); }\n"
        ),
    ),
    CorpusCase(
        case_id="sema_undefined_reference",
        subsystem="semantic",
        source=(
            "module FuzzSemaUndefinedReference;\n"
            "fn main() -> int {\n"
            "  return unknown_symbol;\n"
            "}\n"
        ),
    ),
    CorpusCase(
        case_id="sema_invalid_message_receiver",
        subsystem="semantic",
        source=(
            "module FuzzSemaInvalidMessageReceiver;\n"
            "fn main() -> int {\n"
            "  return [42 length];\n"
            "}\n"
        ),
    ),
    CorpusCase(
        case_id="sema_bad_return_contract",
        subsystem="semantic",
        source=(
            "module FuzzSemaBadReturnContract;\n"
            "fn main() -> int {\n"
            "  return;\n"
            "}\n"
        ),
    ),
)


def mutate_drop_last_char(source: str) -> str:
    if not source:
        return source
    return source[:-1]


def mutate_append_garbage_tail(source: str) -> str:
    return source + "\n@@@ fuzz_token ?? !!\n"


MUTATION_RULES: tuple[MutationRule, ...] = (
    MutationRule("drop_last_char", mutate_drop_last_char),
    MutationRule("append_garbage_tail", mutate_append_garbage_tail),
)


def canonical_json_text(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def normalize_text(value: str) -> str:
    return value.replace("\r\n", "\n")


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def parse_generated_at_utc(value: str | None) -> str:
    if value is None:
        return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    candidate = value.strip()
    try:
        datetime.strptime(candidate, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise InputError(
            "--generated-at-utc must be UTC format YYYY-MM-DDTHH:MM:SSZ"
        ) from exc
    return candidate


def build_corpus(max_cases: int | None) -> list[CorpusCase]:
    cases: dict[str, CorpusCase] = {}
    for base in BASE_CORPUS:
        cases[base.case_id] = base
        for rule in MUTATION_RULES:
            mutated = rule.transform(base.source)
            if mutated == base.source:
                continue
            case_id = f"{base.case_id}__{rule.name}"
            cases[case_id] = CorpusCase(
                case_id=case_id,
                subsystem=base.subsystem,
                source=mutated,
            )
    ordered = [cases[key] for key in sorted(cases)]
    if max_cases is not None:
        return ordered[:max_cases]
    return ordered


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="run_objc3c_fuzz_safety.py",
        description=(
            "Deterministic malformed-input stress/fuzz gate for objc3c parser "
            "and semantic passes."
        ),
    )
    parser.add_argument(
        "--compiler",
        type=Path,
        default=DEFAULT_COMPILER,
        help=f"Path to compiler executable (default: {display_path(DEFAULT_COMPILER)}).",
    )
    parser.add_argument(
        "--python-launcher",
        type=Path,
        help="Optional launcher used to run --compiler (useful for test harnesses).",
    )
    parser.add_argument(
        "--out-root",
        type=Path,
        default=DEFAULT_OUT_ROOT,
        help=f"Output root for corpus/log artifacts (default: {display_path(DEFAULT_OUT_ROOT)}).",
    )
    parser.add_argument(
        "--timeout-sec",
        type=float,
        default=5.0,
        help="Per-invocation timeout in seconds (default: 5.0).",
    )
    parser.add_argument(
        "--max-cases",
        type=int,
        help="Optional deterministic cap for corpus size.",
    )
    parser.add_argument(
        "--generated-at-utc",
        help="Optional deterministic timestamp in UTC format YYYY-MM-DDTHH:MM:SSZ.",
    )
    parser.add_argument(
        "--contract-mode",
        action="store_true",
        help="Emit deterministic JSON summary to stdout.",
    )
    return parser


def validate_inputs(args: argparse.Namespace) -> None:
    if args.timeout_sec <= 0:
        raise InputError("--timeout-sec must be > 0")
    if args.max_cases is not None and args.max_cases <= 0:
        raise InputError("--max-cases must be > 0")

    compiler = args.compiler.resolve()
    if not compiler.exists() or not compiler.is_file():
        raise InputError(f"compiler not found: {display_path(compiler)}")

    if args.python_launcher is not None:
        launcher = args.python_launcher.resolve()
        if not launcher.exists() or not launcher.is_file():
            raise InputError(f"python launcher not found: {display_path(launcher)}")


def hash_output(stdout_text: str, stderr_text: str) -> str:
    payload = normalize_text(stdout_text) + "\n---stderr---\n" + normalize_text(stderr_text)
    return hashlib.sha256(payload.encode("utf-8")).hexdigest()


def run_case(
    *,
    case: CorpusCase,
    compiler_command: list[str],
    out_root: Path,
    timeout_sec: float,
) -> dict[str, object]:
    corpus_dir = out_root / "corpus"
    logs_dir = out_root / "logs"
    case_root = out_root / "cases" / case.case_id
    corpus_dir.mkdir(parents=True, exist_ok=True)
    logs_dir.mkdir(parents=True, exist_ok=True)
    case_root.mkdir(parents=True, exist_ok=True)

    source_path = corpus_dir / f"{case.case_id}.objc3"
    source_path.write_text(case.source, encoding="utf-8")

    outputs: list[dict[str, object]] = []
    unsafe_errors: list[dict[str, str]] = []

    for iteration in (1, 2):
        run_out_dir = case_root / f"run{iteration}"
        run_out_dir.mkdir(parents=True, exist_ok=True)
        command = [
            *compiler_command,
            str(source_path),
            "--out-dir",
            str(run_out_dir),
            "--emit-prefix",
            f"{case.case_id}_r{iteration}",
        ]

        try:
            completed = subprocess.run(
                command,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                timeout=timeout_sec,
                check=False,
            )
            stdout_text = normalize_text(completed.stdout)
            stderr_text = normalize_text(completed.stderr)
            log_path = logs_dir / f"{case.case_id}.run{iteration}.log"
            log_path.write_text(
                f"command: {' '.join(command)}\n"
                f"exit_code: {completed.returncode}\n"
                "---stdout---\n"
                f"{stdout_text}\n"
                "---stderr---\n"
                f"{stderr_text}\n",
                encoding="utf-8",
            )
            outputs.append(
                {
                    "iteration": iteration,
                    "exit_code": completed.returncode,
                    "stdout": stdout_text,
                    "stderr": stderr_text,
                    "output_sha256": hash_output(stdout_text, stderr_text),
                    "timed_out": False,
                    "log_path": display_path(log_path),
                }
            )
        except subprocess.TimeoutExpired:
            timeout_log = logs_dir / f"{case.case_id}.run{iteration}.log"
            timeout_log.write_text(
                f"command: {' '.join(command)}\n"
                f"timeout_sec: {timeout_sec}\n"
                "status: TIMEOUT\n",
                encoding="utf-8",
            )
            outputs.append(
                {
                    "iteration": iteration,
                    "exit_code": None,
                    "stdout": "",
                    "stderr": "",
                    "output_sha256": "",
                    "timed_out": True,
                    "log_path": display_path(timeout_log),
                }
            )
            unsafe_errors.append(
                {
                    "check_id": "SAFE-003",
                    "detail": f"timeout at run{iteration} (> {timeout_sec:.3f}s)",
                }
            )

    run1 = outputs[0]
    run2 = outputs[1]

    run1_exit = run1["exit_code"]
    run2_exit = run2["exit_code"]
    run1_output = f"{run1['stdout']}\n{run1['stderr']}"
    run2_output = f"{run2['stdout']}\n{run2['stderr']}"

    if run1_exit is not None and run1_exit >= 0 and run1_exit == 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-001",
                "detail": "run1 returned exit code 0 for malformed corpus case",
            }
        )
    if run2_exit is not None and run2_exit >= 0 and run2_exit == 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-001",
                "detail": "run2 returned exit code 0 for malformed corpus case",
            }
        )
    if run1_exit is not None and run1_exit < 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-004",
                "detail": f"run1 terminated by signal ({run1_exit})",
            }
        )
    if run2_exit is not None and run2_exit < 0:
        unsafe_errors.append(
            {
                "check_id": "SAFE-004",
                "detail": f"run2 terminated by signal ({run2_exit})",
            }
        )

    if (
        run1_exit is not None
        and run2_exit is not None
        and run1_exit != run2_exit
    ):
        unsafe_errors.append(
            {
                "check_id": "DET-001",
                "detail": f"exit code drift run1={run1_exit} run2={run2_exit}",
            }
        )

    if (
        isinstance(run1["output_sha256"], str)
        and isinstance(run2["output_sha256"], str)
        and run1["output_sha256"]
        and run2["output_sha256"]
        and run1["output_sha256"] != run2["output_sha256"]
    ):
        unsafe_errors.append(
            {
                "check_id": "DET-002",
                "detail": "diagnostic output hash drift between run1 and run2",
            }
        )

    if (
        not run1["timed_out"]
        and not DIAGNOSTIC_SIGNAL_PATTERN.search(run1_output)
    ):
        unsafe_errors.append(
            {
                "check_id": "SAFE-002",
                "detail": "run1 missing diagnostic signal token (error/O3*)",
            }
        )
    if (
        not run2["timed_out"]
        and not DIAGNOSTIC_SIGNAL_PATTERN.search(run2_output)
    ):
        unsafe_errors.append(
            {
                "check_id": "SAFE-002",
                "detail": "run2 missing diagnostic signal token (error/O3*)",
            }
        )

    checks = {
        "nonzero_exit": not any(item["check_id"] == "SAFE-001" for item in unsafe_errors),
        "diagnostic_signal_present": not any(
            item["check_id"] == "SAFE-002" for item in unsafe_errors
        ),
        "timeout_free": not any(item["check_id"] == "SAFE-003" for item in unsafe_errors),
        "no_crash_signal": not any(item["check_id"] == "SAFE-004" for item in unsafe_errors),
        "deterministic_exit": not any(item["check_id"] == "DET-001" for item in unsafe_errors),
        "deterministic_output": not any(
            item["check_id"] == "DET-002" for item in unsafe_errors
        ),
    }

    return {
        "case_id": case.case_id,
        "subsystem": case.subsystem,
        "source_path": display_path(source_path),
        "run1_exit_code": run1_exit,
        "run2_exit_code": run2_exit,
        "run1_output_sha256": run1["output_sha256"],
        "run2_output_sha256": run2["output_sha256"],
        "checks": checks,
        "errors": sorted(
            unsafe_errors,
            key=lambda item: (item["check_id"], item["detail"]),
        ),
    }


def evaluate(
    *,
    compiler_command: list[str],
    out_root: Path,
    timeout_sec: float,
    max_cases: int | None,
    generated_at_utc: str,
) -> dict[str, object]:
    out_root.mkdir(parents=True, exist_ok=True)
    corpus = build_corpus(max_cases=max_cases)
    results = [
        run_case(
            case=case,
            compiler_command=compiler_command,
            out_root=out_root,
            timeout_sec=timeout_sec,
        )
        for case in corpus
    ]

    violations: list[dict[str, str]] = []
    for result in results:
        case_id = str(result["case_id"])
        errors = result["errors"]
        assert isinstance(errors, list)
        for error in errors:
            assert isinstance(error, dict)
            violations.append(
                {
                    "check_id": str(error["check_id"]),
                    "case_id": case_id,
                    "detail": str(error["detail"]),
                }
            )

    violations = sorted(
        violations,
        key=lambda item: (item["check_id"], item["case_id"], item["detail"]),
    )
    status = "PASS" if not violations else "FAIL"

    return {
        "mode": MODE,
        "schema_version": SCHEMA_VERSION,
        "generated_at_utc": generated_at_utc,
        "config": {
            "timeout_sec": timeout_sec,
            "iteration_count": 2,
            "case_count": len(corpus),
            "out_root": display_path(out_root),
        },
        "corpus_strategy": {
            "strategy_id": "objc3c-malformed-corpus-v1",
            "base_case_count": len(BASE_CORPUS),
            "mutation_rules": [rule.name for rule in MUTATION_RULES],
            "deterministic_ordering": "case_id-lexicographic",
        },
        "status": status,
        "violation_count": len(violations),
        "violations": violations,
        "results": results,
    }


def render_human_summary(summary: dict[str, object]) -> str:
    status = summary["status"]
    config = summary["config"]
    assert isinstance(config, dict)
    lines = [
        f"status: {status}",
        f"mode: {summary['mode']}",
        f"case_count: {config['case_count']}",
        f"violation_count: {summary['violation_count']}",
    ]
    if status == "FAIL":
        lines.append("violations:")
        violations = summary["violations"]
        assert isinstance(violations, list)
        for item in violations:
            assert isinstance(item, dict)
            lines.append(
                f"- [{item['check_id']}] {item['case_id']}: {item['detail']}"
            )
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        validate_inputs(args)
        generated_at_utc = parse_generated_at_utc(args.generated_at_utc)
    except InputError as exc:
        print(f"objc3c-fuzz-safety: error: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    compiler_command: list[str]
    if args.python_launcher is not None:
        compiler_command = [str(args.python_launcher.resolve()), str(args.compiler.resolve())]
    else:
        compiler_command = [str(args.compiler.resolve())]

    try:
        summary = evaluate(
            compiler_command=compiler_command,
            out_root=args.out_root.resolve(),
            timeout_sec=float(args.timeout_sec),
            max_cases=args.max_cases,
            generated_at_utc=generated_at_utc,
        )
    except OSError as exc:
        print(f"objc3c-fuzz-safety: error: failed to execute compiler: {exc}", file=sys.stderr)
        return EXIT_INPUT_ERROR

    if args.contract_mode:
        print(canonical_json_text(summary), end="")
    elif summary["status"] == "FAIL":
        print(render_human_summary(summary), file=sys.stderr, end="")
    else:
        print(render_human_summary(summary), end="")

    return EXIT_OK if summary["status"] == "PASS" else EXIT_GUARD_FAIL


if __name__ == "__main__":
    raise SystemExit(main())
