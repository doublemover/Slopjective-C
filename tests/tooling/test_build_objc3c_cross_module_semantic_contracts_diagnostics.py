from build_objc3c_cross_module_semantic_contracts_diagnostics_report import (
    load_cross_module_semantic_contracts_diagnostics_report,
)
from build_objc3c_cross_module_semantic_contracts_diagnostics_runner import (
    run_cross_module_semantic_contracts_diagnostics_check,
)
from build_objc3c_cross_module_semantic_contracts_diagnostics_summary import (
    assert_cross_module_semantic_contracts_diagnostics_summary,
)


def cross_module_semantic_contracts_diagnostics_summary_is_fresh_and_passing() -> None:
    completed = run_cross_module_semantic_contracts_diagnostics_check()
    assert completed.returncode == 0, completed.stdout + completed.stderr

    summary = load_cross_module_semantic_contracts_diagnostics_report()
    assert_cross_module_semantic_contracts_diagnostics_summary(summary)


test_cross_module_semantic_contracts_diagnostics_summary_is_fresh_and_passing = (
    cross_module_semantic_contracts_diagnostics_summary_is_fresh_and_passing
)
