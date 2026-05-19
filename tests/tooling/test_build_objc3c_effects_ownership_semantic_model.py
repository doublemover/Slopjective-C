from build_objc3c_effects_ownership_semantic_model_artifact import (
    assert_effects_ownership_semantic_model_summary_artifact_exists,
)
from build_objc3c_effects_ownership_semantic_model_runner import (
    run_effects_ownership_semantic_model_check,
)


def effects_ownership_semantic_model_report_is_current() -> None:
    result = run_effects_ownership_semantic_model_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout
    assert_effects_ownership_semantic_model_summary_artifact_exists()


test_effects_ownership_semantic_model_report_is_current = (
    effects_ownership_semantic_model_report_is_current
)
