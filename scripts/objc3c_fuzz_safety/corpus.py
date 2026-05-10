"""Corpus construction and manifest loading for the objc3c fuzz-safety runner."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Callable

from objc3c_tooling.paths import display_path

from .config import DEFAULT_MANIFEST, ROOT, InputError


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
            "fn main() -> i32 {\n"
            "  return [extern ping];\n"
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


def normalize_text(value: str) -> str:
    return value.replace("\r\n", "\n")


def load_manifest_cases(manifest_path: Path) -> list[CorpusCase]:
    payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise InputError(f"manifest did not contain an object: {display_path(manifest_path)}")
    if payload.get("contract_id") != "objc3c.stress.parser-sema-fuzz.manifest.v1":
        raise InputError(f"manifest contract_id drifted: {display_path(manifest_path)}")
    cases_payload = payload.get("cases")
    if not isinstance(cases_payload, list) or not cases_payload:
        raise InputError(f"manifest missing non-empty cases list: {display_path(manifest_path)}")

    cases: list[CorpusCase] = []
    for entry in cases_payload:
        if not isinstance(entry, dict):
            raise InputError(f"manifest contains a non-object case: {display_path(manifest_path)}")
        case_id = entry.get("case_id")
        subsystem = entry.get("subsystem")
        source_path = entry.get("source_path")
        if not isinstance(case_id, str) or not case_id:
            raise InputError(f"manifest case missing case_id: {display_path(manifest_path)}")
        if subsystem not in {"parser", "semantic"}:
            raise InputError(f"manifest case {case_id} has invalid subsystem")
        if not isinstance(source_path, str) or not source_path:
            raise InputError(f"manifest case {case_id} missing source_path")
        resolved_source = (ROOT / source_path).resolve()
        if not resolved_source.is_file():
            raise InputError(f"manifest case {case_id} references missing file {source_path}")
        cases.append(
            CorpusCase(
                case_id=case_id,
                subsystem=subsystem,
                source=normalize_text(resolved_source.read_text(encoding="utf-8")),
            )
        )
    return cases


def build_corpus(max_cases: int | None, *, manifest_path: Path = DEFAULT_MANIFEST) -> list[CorpusCase]:
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
    for manifest_case in load_manifest_cases(manifest_path):
        cases[manifest_case.case_id] = manifest_case
    ordered = [cases[key] for key in sorted(cases)]
    if max_cases is not None:
        return ordered[:max_cases]
    return ordered
