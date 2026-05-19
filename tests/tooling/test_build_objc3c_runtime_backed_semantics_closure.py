from build_objc3c_runtime_backed_semantics_closure_negative_compile import (
    assert_runtime_backed_semantics_token_and_negative_compile_expectations,
)
from build_objc3c_runtime_backed_semantics_closure_summary_behavior import (
    assert_runtime_backed_semantics_summary_behavior,
)
from build_objc3c_runtime_backed_semantics_closure_support import (
    load_runtime_backed_semantics_closure_summary,
    run_runtime_backed_semantics_closure_check,
)


def runtime_backed_semantics_closure_report_is_current() -> None:
    result = run_runtime_backed_semantics_closure_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout

    summary = load_runtime_backed_semantics_closure_summary()
    assert_runtime_backed_semantics_summary_behavior(summary)
    assert_runtime_backed_semantics_token_and_negative_compile_expectations(summary)


test_runtime_backed_semantics_closure_report_is_current = (
    runtime_backed_semantics_closure_report_is_current
)
