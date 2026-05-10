from build_objc3c_manifest_object_ir_truth_gate_counts import (
    assert_manifest_object_ir_truth_gate_counts_and_checks,
)
from build_objc3c_manifest_object_ir_truth_gate_positive_runs import (
    assert_manifest_object_ir_truth_gate_positive_run_tokens,
)
from build_objc3c_manifest_object_ir_truth_gate_runner import (
    run_manifest_object_ir_truth_gate_check,
)
from build_objc3c_manifest_object_ir_truth_gate_summary import (
    load_manifest_object_ir_truth_gate_summary,
)


def manifest_object_ir_truth_gate_report_is_current() -> None:
    result = run_manifest_object_ir_truth_gate_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout

    summary = load_manifest_object_ir_truth_gate_summary()
    assert_manifest_object_ir_truth_gate_counts_and_checks(summary)
    assert_manifest_object_ir_truth_gate_positive_run_tokens(summary)


test_manifest_object_ir_truth_gate_report_is_current = (
    manifest_object_ir_truth_gate_report_is_current
)
