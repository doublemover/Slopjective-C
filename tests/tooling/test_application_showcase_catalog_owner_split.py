from __future__ import annotations

from scripts.objc3c_workflow.action_catalog_application_architecture import (
    APPLICATION_ARCHITECTURE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_developer_tooling import (
    APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_playground import (
    APPLICATION_PLAYGROUND_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib import (
    APPLICATION_STDLIB_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_integrations import (
    APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_runnable import (
    APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_stdlib_workspace import (
    APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_application_workspaces import (
    APPLICATION_WORKSPACE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_getting_started import (
    CORE_GETTING_STARTED_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_showcase import (
    CORE_SHOWCASE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_showcase_examples import (
    CORE_SHOWCASE_EXAMPLE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_core_stdlib_surface import (
    CORE_STDLIB_SURFACE_ACTION_SPECS,
)


def test_application_workspace_catalog_aggregates_owner_catalogs() -> None:
    assert tuple(APPLICATION_WORKSPACE_ACTION_SPECS) == (
        "materialize-playground-workspace",
        "materialize-canonical-application-workspace",
        "validate-application-architecture",
        "validate-runnable-application-architecture",
        "validate-runnable-developer-tooling",
    )
    assert (
        APPLICATION_WORKSPACE_ACTION_SPECS["materialize-playground-workspace"]
        is APPLICATION_PLAYGROUND_ACTION_SPECS["materialize-playground-workspace"]
    )
    for action, spec in APPLICATION_ARCHITECTURE_ACTION_SPECS.items():
        assert APPLICATION_WORKSPACE_ACTION_SPECS[action] is spec
    assert (
        APPLICATION_WORKSPACE_ACTION_SPECS["validate-runnable-developer-tooling"]
        is APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS[
            "validate-runnable-developer-tooling"
        ]
    )


def test_application_stdlib_catalog_aggregates_owner_catalogs() -> None:
    assert tuple(APPLICATION_STDLIB_ACTION_SPECS) == (
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-stdlib-advanced",
        "validate-stdlib-program",
        "validate-runnable-stdlib-advanced",
        "validate-runnable-stdlib-foundation",
        "validate-runnable-stdlib-program",
    )

    owner_catalogs = (
        APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS,
        APPLICATION_STDLIB_INTEGRATION_ACTION_SPECS,
        APPLICATION_STDLIB_RUNNABLE_ACTION_SPECS,
    )
    for catalog in owner_catalogs:
        for action, spec in catalog.items():
            assert APPLICATION_STDLIB_ACTION_SPECS[action] is spec


def test_core_showcase_catalog_preserves_public_order_and_owner_identity() -> None:
    assert tuple(CORE_SHOWCASE_ACTION_SPECS) == (
        "check-showcase-surface",
        "check-stdlib-surface",
        "validate-showcase-runtime",
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-getting-started",
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["check-showcase-surface"]
        is CORE_SHOWCASE_EXAMPLE_ACTION_SPECS["check-showcase-surface"]
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["check-stdlib-surface"]
        is CORE_STDLIB_SURFACE_ACTION_SPECS["check-stdlib-surface"]
    )
    assert (
        CORE_SHOWCASE_ACTION_SPECS["validate-getting-started"]
        is CORE_GETTING_STARTED_ACTION_SPECS["validate-getting-started"]
    )
