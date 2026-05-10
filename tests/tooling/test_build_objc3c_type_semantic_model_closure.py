from build_objc3c_type_semantic_model_closure_artifact import (
    assert_type_semantic_model_summary_artifact_exists,
)
from build_objc3c_type_semantic_model_closure_runner import (
    run_type_semantic_model_closure_check,
)


def type_semantic_model_closure_report_is_current() -> None:
    result = run_type_semantic_model_closure_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout
    assert_type_semantic_model_summary_artifact_exists()


test_type_semantic_model_closure_report_is_current = (
    type_semantic_model_closure_report_is_current
)
