from runtime_closure_owner_contracts_forbidden_claims import (
    assert_runtime_closure_forbidden_claims_fail_closed,
)
from runtime_closure_owner_contracts_owner_metadata import (
    assert_runtime_closure_owner_metadata,
    runtime_closure_owner_summary_keeps_publication_policy_visible,
)
from runtime_closure_owner_contracts_support import (
    runtime_closure_owner_fixtures,
)


def runtime_closure_owner_contracts_are_source_owned_and_fail_closed() -> None:
    for (
        boundary_path,
        boundary,
        owner_contract,
        checks,
    ) in runtime_closure_owner_fixtures():
        assert_runtime_closure_owner_metadata(owner_contract)
        assert_runtime_closure_forbidden_claims_fail_closed(
            boundary_path,
            boundary,
            checks,
        )


test_runtime_closure_owner_contracts_are_source_owned_and_fail_closed = (
    runtime_closure_owner_contracts_are_source_owned_and_fail_closed
)
test_runtime_closure_owner_summary_keeps_publication_policy_visible = (
    runtime_closure_owner_summary_keeps_publication_policy_visible
)
