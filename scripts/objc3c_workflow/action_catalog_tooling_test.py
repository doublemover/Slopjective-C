"""Tooling, runtime, and test action specs."""

from __future__ import annotations

from .action_catalog_native_package import NATIVE_PACKAGE_ACTION_SPECS
from .action_catalog_public_tests import PUBLIC_TEST_ACTION_SPECS
from .action_catalog_runtime_validation import RUNTIME_VALIDATION_ACTION_SPECS
from .action_catalog_sections import merge_action_catalog_sections
from .action_catalog_tooling_developer import TOOLING_DEVELOPER_ACTION_SPECS
from .action_spec import ActionSpec

TOOLING_AND_TEST_ACTION_SPECS: dict[str, ActionSpec] = merge_action_catalog_sections(
    TOOLING_DEVELOPER_ACTION_SPECS,
    PUBLIC_TEST_ACTION_SPECS,
    RUNTIME_VALIDATION_ACTION_SPECS,
    NATIVE_PACKAGE_ACTION_SPECS,
)
