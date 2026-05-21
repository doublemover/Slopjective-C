"""Source-derived runtime debug trace contract extraction."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT, display_path

RUNTIME_TRACE_SOURCE_CONTRACT_ID = "objc3.runtime.debug.trace.source-contracts.v1"
RUNTIME_TRACE_SOURCE_CONTRACT_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "runtime" / "debug" / "runtime_debug_trace_contracts.h"
)
RUNTIME_TRACE_SOURCE_CONTRACT_IMPLEMENTATION = (
    ROOT / "native" / "objc3c" / "src" / "runtime" / "debug" / "runtime_debug_trace_contracts.cpp"
)
RUNTIME_TRACE_CONTRACT_MACRO = "OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACTS"
_EXPECTED_CONTRACT_FIELD_COUNT = 11


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _extract_macro_body(source: str) -> tuple[int, str]:
    lines = source.splitlines()
    for index, line in enumerate(lines):
        if line.startswith(f"#define {RUNTIME_TRACE_CONTRACT_MACRO}(X)"):
            body: list[str] = []
            for macro_line in lines[index + 1 :]:
                continued = macro_line.rstrip().endswith("\\")
                body.append(macro_line.rstrip().removesuffix("\\"))
                if not continued:
                    break
            return index + 2, "\n".join(body)
    raise ValueError(f"{RUNTIME_TRACE_CONTRACT_MACRO} macro missing")


def _iter_invocations(body: str) -> list[tuple[int, str]]:
    invocations: list[tuple[int, str]] = []
    cursor = 0
    while True:
        start = body.find("X(", cursor)
        if start < 0:
            return invocations
        depth = 0
        in_string = False
        escaped = False
        for offset in range(start + 1, len(body)):
            char = body[offset]
            if in_string:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == '"':
                    in_string = False
                continue
            if char == '"':
                in_string = True
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
                if depth == 0:
                    line_number = body.count("\n", 0, start) + 1
                    invocations.append((line_number, body[start + 2 : offset]))
                    cursor = offset + 1
                    break
        else:
            raise ValueError("unterminated runtime debug trace contract row")


def _split_arguments(invocation: str) -> list[str]:
    arguments: list[str] = []
    start = 0
    in_string = False
    escaped = False
    for index, char in enumerate(invocation):
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == ",":
            arguments.append(invocation[start:index].strip())
            start = index + 1
    arguments.append(invocation[start:].strip())
    return arguments


def _string_argument(argument: str) -> str:
    tokens = []
    in_string = False
    escaped = False
    start = 0
    for index, char in enumerate(argument):
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                tokens.append(argument[start : index + 1])
                in_string = False
            continue
        if char == '"':
            in_string = True
            start = index
    if not tokens:
        raise ValueError(f"expected string literal argument, got: {argument}")
    return "".join(json.loads(token) for token in tokens)


def _int_argument(argument: str) -> int:
    try:
        return int(argument)
    except ValueError as exc:
        raise ValueError(f"expected integer argument, got: {argument}") from exc


def _contract_row(arguments: list[str], *, source_line: int) -> dict[str, Any]:
    if len(arguments) != _EXPECTED_CONTRACT_FIELD_COUNT:
        raise ValueError(
            "runtime debug trace contract row has "
            f"{len(arguments)} fields; expected {_EXPECTED_CONTRACT_FIELD_COUNT}"
        )
    required_fields = [
        field.strip()
        for field in _string_argument(arguments[6]).split(",")
        if field.strip()
    ]
    return {
        "lane_id": _string_argument(arguments[0]),
        "trace_domain": _string_argument(arguments[1]),
        "event_kind": _string_argument(arguments[2]),
        "snapshot_header": _string_argument(arguments[3]),
        "snapshot_type": _string_argument(arguments[4]),
        "snapshot_symbol": _string_argument(arguments[5]),
        "required_fields": required_fields,
        "status": _string_argument(arguments[7]),
        "source_anchor": _string_argument(arguments[8]),
        "deterministic": _int_argument(arguments[9]) == 1,
        "public_abi": _int_argument(arguments[10]) == 1,
        "source_path": display_path(RUNTIME_TRACE_SOURCE_CONTRACT_HEADER),
        "source_line": source_line,
    }


def load_runtime_trace_source_contracts() -> dict[str, Any]:
    header_text = RUNTIME_TRACE_SOURCE_CONTRACT_HEADER.read_text(encoding="utf-8")
    first_body_line, body = _extract_macro_body(header_text)
    lanes = [
        _contract_row(
            _split_arguments(invocation),
            source_line=first_body_line + line_number - 1,
        )
        for line_number, invocation in _iter_invocations(body)
    ]
    return {
        "contract_id": RUNTIME_TRACE_SOURCE_CONTRACT_ID,
        "macro": RUNTIME_TRACE_CONTRACT_MACRO,
        "source_path": display_path(RUNTIME_TRACE_SOURCE_CONTRACT_HEADER),
        "implementation_path": display_path(RUNTIME_TRACE_SOURCE_CONTRACT_IMPLEMENTATION),
        "source_sha256": _sha256(RUNTIME_TRACE_SOURCE_CONTRACT_HEADER),
        "implementation_sha256": _sha256(RUNTIME_TRACE_SOURCE_CONTRACT_IMPLEMENTATION),
        "lane_count": len(lanes),
        "lanes": lanes,
    }
