"""Configuration for live metaprogramming host-cache compilation cases."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.progress_format import repo_display_path


CASE_DIR_NAME = "live-metaprogramming-cache-runtime-integration"
HOST_CACHE_PROBE_PATH = "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
HOST_CACHE_PROBE_LABEL = "live metaprogramming host-cache runtime integration probe"
MATERIALIZATION_ATTEMPT_LIMIT = 16

PROVIDER_FIXTURE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "macro_host_process_provider.objc3"
)
CONSUMER_FIXTURE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "macro_host_process_consumer.objc3"
)

PROVIDER_MODULE_DECLARATION = "module MetaprogrammingHostProcessProvider;"
PROVIDER_MODULE_NAME_PREFIX = "MetaprogrammingHostProcessProvider"
PROVIDER_MATERIALIZED_FIXTURE_NAME = "metaprogramming_cache_provider_materialize.objc3"

CACHE_ROOT_DIR_NAME = "metaprogramming-cache-root"
CACHE_ROOT_FLAG = "--objc3-metaprogramming-cache-root"
CACHE_ROOT_ENVIRONMENT_VARIABLE = "OBJC3C_METAPROGRAMMING_CACHE_ROOT"

REGISTRATION_ORDINAL_FLAG = "--objc3-bootstrap-registration-order-ordinal"
IMPORT_RUNTIME_SURFACE_FLAG = "--objc3-import-runtime-surface"

HOST_CACHE_ARTIFACT_FILE = "module.metaprogramming-macro-host-cache.json"
RUNTIME_IMPORT_SURFACE_FILE = "module.runtime-import-surface.json"
CONSUMER_LINK_PLAN_FILE = "module.cross-module-runtime-link-plan.json"
HOST_CACHE_IMPORT_SURFACE_KEY = (
    "objc_metaprogramming_macro_host_process_and_cache_runtime_integration"
)


def case_dir_for_run(run_dir: Path) -> Path:
    return run_dir / CASE_DIR_NAME


def cache_root_for_case(case_dir: Path) -> Path:
    return case_dir / CACHE_ROOT_DIR_NAME


def cache_root_override_for_case(case_dir: Path) -> str:
    return repo_display_path(cache_root_for_case(case_dir))


def cache_root_args(cache_root_override: str) -> tuple[str, str]:
    return (CACHE_ROOT_FLAG, cache_root_override)


__all__ = [
    "CACHE_ROOT_ENVIRONMENT_VARIABLE",
    "CACHE_ROOT_FLAG",
    "CASE_DIR_NAME",
    "CONSUMER_FIXTURE_PATH",
    "CONSUMER_LINK_PLAN_FILE",
    "HOST_CACHE_ARTIFACT_FILE",
    "HOST_CACHE_IMPORT_SURFACE_KEY",
    "HOST_CACHE_PROBE_LABEL",
    "HOST_CACHE_PROBE_PATH",
    "IMPORT_RUNTIME_SURFACE_FLAG",
    "MATERIALIZATION_ATTEMPT_LIMIT",
    "PROVIDER_FIXTURE_PATH",
    "PROVIDER_MATERIALIZED_FIXTURE_NAME",
    "PROVIDER_MODULE_DECLARATION",
    "PROVIDER_MODULE_NAME_PREFIX",
    "REGISTRATION_ORDINAL_FLAG",
    "RUNTIME_IMPORT_SURFACE_FILE",
    "cache_root_args",
    "cache_root_for_case",
    "cache_root_override_for_case",
    "case_dir_for_run",
]
