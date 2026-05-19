from build_objc3c_parser_draft_syntax_surface_artifact import (
    assert_parser_draft_syntax_surface_summary_artifact_exists,
)
from build_objc3c_parser_draft_syntax_surface_runner import (
    run_parser_draft_syntax_surface_check,
)


def parser_draft_syntax_surface_report_is_current() -> None:
    result = run_parser_draft_syntax_surface_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout
    assert_parser_draft_syntax_surface_summary_artifact_exists()


test_parser_draft_syntax_surface_report_is_current = (
    parser_draft_syntax_surface_report_is_current
)
