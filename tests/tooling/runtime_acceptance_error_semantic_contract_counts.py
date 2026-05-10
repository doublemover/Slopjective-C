from runtime_acceptance_error_semantic_contract_support import cleanup_expected_counts


def error_propagation_cleanup_contract_preserves_strict_counts() -> None:
    counts = cleanup_expected_counts()

    assert counts["throws_declaration_sites"] == 1
    assert counts["result_like_sites"] == 7
    assert counts["ns_error_bridging_sites"] == 3
    assert counts["placeholder_throws_propagation_sites"] == 0
    assert counts["placeholder_unwind_cleanup_sites"] == 0
