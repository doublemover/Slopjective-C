from __future__ import annotations

from scripts.objc3c_workflow.npm_surface import describe_package_script_payload
from scripts.objc3c_workflow.npm_surface_policy import (
    NPM_SURFACE_POLICY_OWNER,
    PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID,
    PACKAGE_SCRIPT_LOOKUP_OWNER,
    package_script_lookup_decision,
    package_script_is_registered,
    package_script_payload_name,
)
from scripts.objc3c_workflow.public_bridge import PACKAGE_BRIDGES


def test_npm_surface_policy_owns_package_script_lookup_and_payload_name() -> None:
    assert NPM_SURFACE_POLICY_OWNER == "objc3c-workflow-npm-surface-policy"
    assert PACKAGE_SCRIPT_LOOKUP_OWNER == "objc3c-workflow-package-script-lookup"
    assert PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID == (
        "objc3c-workflow-package-script-lookup-v1"
    )
    assert package_script_is_registered(PACKAGE_BRIDGES, "objc3c") is True
    assert package_script_is_registered(PACKAGE_BRIDGES, "retired-script") is False
    assert package_script_payload_name("objc3c") == "objc3c"
    lookup = package_script_lookup_decision(PACKAGE_BRIDGES, "objc3c")
    assert lookup.registered is True
    assert lookup.validation_errors == ()
    payload = describe_package_script_payload("objc3c")
    assert payload["script_name"] == "objc3c"
    assert payload["package_script_lookup"]["contract_id"] == (
        "objc3c-workflow-package-script-lookup-v1"
    )
    assert payload["package_script_lookup"]["retired_metadata_allowed"] is False
