from scripts.objc3c_workflow.actions.runtime_acceptance_route_catalog import (
    RUNTIME_ACCEPTANCE_ROUTES,
)
from scripts.objc3c_workflow.actions.runtime_acceptance_route_model import (
    RuntimeAcceptanceRoute,
)

from runtime_acceptance_route_owner_split_support import (
    PUBLIC_RUNTIME_ACCEPTANCE_ACTIONS,
)


def runtime_acceptance_route_catalog_preserves_public_suites() -> None:
    assert set(RUNTIME_ACCEPTANCE_ROUTES) == PUBLIC_RUNTIME_ACCEPTANCE_ACTIONS
    for action, route in RUNTIME_ACCEPTANCE_ROUTES.items():
        assert isinstance(route, RuntimeAcceptanceRoute)
        assert route.action == action
        assert route.target.endswith(f"--suite {route.suite}")
