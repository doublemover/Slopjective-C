from build_objc3c_object_model_ir_lowering_closure_layout import (
    assert_object_model_ir_lowering_layout_offsets,
)
from build_objc3c_object_model_ir_lowering_closure_runner import (
    run_object_model_ir_lowering_closure_check,
)
from build_objc3c_object_model_ir_lowering_closure_summary import (
    load_object_model_ir_lowering_closure_summary,
)


def object_model_ir_lowering_closure_report_is_current() -> None:
    result = run_object_model_ir_lowering_closure_check()

    assert result.returncode == 0, result.stdout + result.stderr
    assert "status: PASS" in result.stdout

    summary = load_object_model_ir_lowering_closure_summary()
    assert_object_model_ir_lowering_layout_offsets(summary)


test_object_model_ir_lowering_closure_report_is_current = (
    object_model_ir_lowering_closure_report_is_current
)
