"""Block/ARC capture legality runtime acceptance case orchestration."""

from __future__ import annotations

from pathlib import Path

from ...case_result import CaseResult
from .assertions import expect_capture_fixture_set
from .catalog import (
    CASE_ID,
    CLAIM_CLASS,
    FIXTURE,
    POST_DIAGNOSTIC_FIXTURES,
    PRE_DIAGNOSTIC_FIXTURES,
    PROBE,
    SUMMARY_FIXTURES,
)
from .data import CaptureFixtureFacts
from .payloads import (
    build_capture_legality_summary,
    capture_fixture_facts,
    compile_capture_negative_batch,
)


def check_escaping_block_capture_legality_case(run_dir: Path) -> CaseResult:
    case_dir = run_dir / CASE_ID
    facts_by_key: dict[str, CaptureFixtureFacts] = {}

    for spec in PRE_DIAGNOSTIC_FIXTURES:
        facts_by_key[spec.key] = capture_fixture_facts(spec, case_dir)

    negative_batch = compile_capture_negative_batch(case_dir)

    for spec in POST_DIAGNOSTIC_FIXTURES:
        facts_by_key[spec.key] = capture_fixture_facts(spec, case_dir)

    expect_capture_fixture_set(facts_by_key, SUMMARY_FIXTURES)

    return CaseResult(
        case_id=CASE_ID,
        probe=PROBE,
        fixture=FIXTURE,
        claim_class=CLAIM_CLASS,
        passed=True,
        summary=build_capture_legality_summary(facts_by_key, negative_batch),
    )


__all__ = ["check_escaping_block_capture_legality_case"]
