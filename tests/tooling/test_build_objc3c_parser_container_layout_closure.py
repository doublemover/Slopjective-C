from build_objc3c_parser_container_layout_closure_artifact import (
    assert_parser_container_layout_summary_artifact_exists,
)
from build_objc3c_parser_container_layout_closure_runner import (
    run_parser_container_layout_closure_check,
)


def parser_container_layout_closure_report_is_current() -> None:
    result = run_parser_container_layout_closure_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout
    assert_parser_container_layout_summary_artifact_exists()


test_parser_container_layout_closure_report_is_current = (
    parser_container_layout_closure_report_is_current
)
