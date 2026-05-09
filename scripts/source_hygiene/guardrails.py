from __future__ import annotations


GUARDRAIL_IDENTIFIERS: tuple[str, ...] = (
    "no-milestone-local",
    "no milestone-local",
    "no-duplicate-milestone-local",
    "no-proof-only",
)

GUARDRAIL_PHRASES: tuple[str, ...] = (
    "absent support",
    "are not support claims",
    "as public command surface",
    "cannot create",
    "cannot itself create",
    "does not add",
    "do not advertise",
    "do not claim",
    "do not describe",
    "do not document",
    "does not publish",
    "fail closed",
    "forbidden",
    "instead of",
    "is not allowed",
    "is absent",
    "may not become",
    "must not",
    "never authorize",
    "not a public",
    "not a compatibility",
    "not a fallback",
    "not command support",
    "not migration",
    "not public",
    "not support claims",
    "not user-facing",
    "non-goal",
    "non-goals",
    "no compatibility",
    "no fallback",
    "no legacy",
    "no migration",
    "no shim",
    "no accepted public surface",
    "removed from",
    "reject",
    "rejects",
    "rejected",
    "rejection",
    "retired",
    "stay out",
    "stays out",
    "strict-error",
    "unsupported",
    "without adding",
    "without becoming public",
    "without introducing",
)


def is_negative_test_assertion(repo_path: str, line: str) -> bool:
    return repo_path.startswith("tests/") and "assert" in line and " not in " in line


def is_guardrail_identifier(line: str) -> bool:
    return any(marker in line for marker in GUARDRAIL_IDENTIFIERS)


def is_guardrail_statement(*lines: str) -> bool:
    text = " ".join(line.strip().lower() for line in lines if line.strip())
    return any(phrase in text for phrase in GUARDRAIL_PHRASES)


def is_canonical_guardrail_context(
    repo_path: str,
    previous_line: str,
    line: str,
    next_line: str,
) -> bool:
    if is_negative_test_assertion(repo_path, line):
        return True
    if is_guardrail_identifier(line):
        return True
    return (
        is_guardrail_statement(line)
        or is_guardrail_statement(previous_line, line)
        or is_guardrail_statement(line, next_line)
    )
