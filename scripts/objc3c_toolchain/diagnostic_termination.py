"""Require an ordinary diagnostic exit from an authored compiler gate."""
from __future__ import annotations

import re

ABNORMAL_DIAGNOSTIC = re.compile(
    r'^PLEASE (?:submit a bug report|ATTACH THE FOLLOWING FILES)\b'
    r'|^LLVM ERROR:'
    r'|^Stack dump:'
    r'|\bAssertion [^\n]*failed[.!]?\s*$'
    r'|\berror: (?:[\w+-]+ )?frontend command failed (?:due to signal|with exit code)\b'
    r'|\berror: unable to execute command:'
    r'|^(?:==\d+==\s*)?(?:(?:ERROR|WARNING|SUMMARY):\s*)?'
    r'(?:AddressSanitizer|LeakSanitizer|MemorySanitizer|ThreadSanitizer|UndefinedBehaviorSanitizer):'
    r'|^.*:\d+:\d+: runtime error:',
    re.MULTILINE,
)


def require_diagnostic_exit(code: int, transcript: str, *markers: str) -> None:
    if type(code) is not int or code != 1:
        raise ValueError(f'expected diagnostic exit 1, got {code}')
    abnormal = ABNORMAL_DIAGNOSTIC.search(transcript)
    if abnormal:
        raise ValueError(f'abnormal termination in diagnostic exit 1: {abnormal.group().strip()}')
    for marker in markers:
        if marker not in transcript:
            raise ValueError(f'ordinary diagnostic exit lost expected text: {marker}')
