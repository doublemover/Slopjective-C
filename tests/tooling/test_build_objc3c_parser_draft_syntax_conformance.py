from __future__ import annotations

from build_objc3c_parser_draft_syntax_conformance_artifacts import (
    assert_parser_draft_syntax_conformance_artifacts_are_current,
)
from build_objc3c_parser_draft_syntax_conformance_runner import (
    run_parser_draft_syntax_conformance_check,
)


def parser_draft_syntax_conformance_report_is_current() -> None:
    completed = run_parser_draft_syntax_conformance_check()
    assert_parser_draft_syntax_conformance_artifacts_are_current(completed)


test_parser_draft_syntax_conformance_report_is_current = (
    parser_draft_syntax_conformance_report_is_current
)
