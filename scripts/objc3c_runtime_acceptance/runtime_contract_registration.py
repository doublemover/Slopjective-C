"""Runtime registration and installation contract ownership."""

from __future__ import annotations


RUNTIME_MULTI_IMAGE_STARTUP_ORDERING_SOURCE_SURFACE_CONTRACT_ID = (
    "objc3c.runtime.multi.image.startup.ordering.source.surface.v1"
)
RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID = "objc3c.runtime.acceptance.suite.surface.v1"
RUNTIME_INSTALLATION_ABI_SURFACE_CONTRACT_ID = "objc3c.runtime.installation.abi.surface.v1"
RUNTIME_LOADER_LIFECYCLE_SURFACE_CONTRACT_ID = "objc3c.runtime.loader.lifecycle.surface.v1"
RUNTIME_DUPLICATE_INSTALL_DIAGNOSTIC_MODEL = (
    "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)
RUNTIME_OUT_OF_ORDER_INSTALL_DIAGNOSTIC_MODEL = (
    "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state"
)

INSTALLATION_LIFECYCLE_FIXTURE = (
    "tests/tooling/fixtures/native/runtime_canonical_runnable_object_runtime_library.objc3"
)
INSTALLATION_LIFECYCLE_PROBE = (
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp"
)
MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE = (
    "tests/tooling/runtime/multi_image_registration_reset_replay_probe.cpp"
)


__all__ = [name for name in globals() if name.isupper()]
