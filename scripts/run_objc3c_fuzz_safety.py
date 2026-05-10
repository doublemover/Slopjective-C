#!/usr/bin/env python3
"""Deterministic stress/fuzz safety gate for objc3c parser and semantic passes."""

from __future__ import annotations

from objc3c_fuzz_safety.aggregation import evaluate
from objc3c_fuzz_safety.cli import build_parser, main
from objc3c_fuzz_safety.config import (
    DEFAULT_COMPILER,
    DEFAULT_MANIFEST,
    DEFAULT_OUT_ROOT,
    EXIT_GUARD_FAIL,
    EXIT_INPUT_ERROR,
    EXIT_OK,
    MODE,
    ROOT,
    SCHEMA_VERSION,
    InputError,
    parse_generated_at_utc,
    validate_inputs,
)
from objc3c_fuzz_safety.corpus import (
    BASE_CORPUS,
    MUTATION_RULES,
    CorpusCase,
    MutationRule,
    build_corpus,
    load_manifest_cases,
    mutate_append_garbage_tail,
    mutate_drop_last_char,
    normalize_text,
)
from objc3c_fuzz_safety.execution import (
    DIAGNOSTIC_SIGNAL_PATTERN,
    hash_output,
    run_case,
)
from objc3c_fuzz_safety.reporting import canonical_json_text, render_human_summary

__all__ = [
    "BASE_CORPUS",
    "DEFAULT_COMPILER",
    "DEFAULT_MANIFEST",
    "DEFAULT_OUT_ROOT",
    "DIAGNOSTIC_SIGNAL_PATTERN",
    "EXIT_GUARD_FAIL",
    "EXIT_INPUT_ERROR",
    "EXIT_OK",
    "MODE",
    "MUTATION_RULES",
    "ROOT",
    "SCHEMA_VERSION",
    "CorpusCase",
    "InputError",
    "MutationRule",
    "build_corpus",
    "build_parser",
    "canonical_json_text",
    "evaluate",
    "hash_output",
    "load_manifest_cases",
    "main",
    "mutate_append_garbage_tail",
    "mutate_drop_last_char",
    "normalize_text",
    "parse_generated_at_utc",
    "render_human_summary",
    "run_case",
    "validate_inputs",
]


if __name__ == "__main__":
    raise SystemExit(main())
