"""Deterministic native validation inventory and exact node selection.

The inventory stays complete when a run selects only a subset, allowing the
existing cross-lane executor to reuse independently qualified nodes.
"""
from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

FAMILIES = ("carriers", "typed-errors", "native-package", "editor")
DRIVERS = ("clang", "clang-cl", "objc3c-clang")
FIXTURE_SUITES = ("expression", "abi", "cpp-lifetime", "arc-lifetime", "match", "transfer", "typed-errors")

_RUNTIME = "scripts/objc3c_toolchain_runtime"
_VALIDATION = "scripts/objc3c_native_validation"
_COMMON_INPUTS = (
    f"{_VALIDATION}/plan.py",
    "scripts/objc3c_cross_lane_execution.py",
    "scripts/objc3c_cross_lane_selection.py",
    "scripts/objc3c_toolchain/__init__.py",
    "scripts/objc3c_toolchain/__main__.py",
    "scripts/objc3c_toolchain/cli.py",
    "scripts/objc3c_toolchain/core.py",
    "scripts/objc3c_toolchain/host.py",
    "scripts/objc3c_toolchain/facade.py",
    "toolchains/llvm/manifest.json",
    "toolchains/llvm/patches/series",
)
_COMMON_SIBLINGS = ("__init__.py", "cli.py", "worker.py", "artifacts.py", "identity.py", "identity_linux.py")
_RUNTIME_INPUTS = (f"{_RUNTIME}/runtime-profile.json", f"{_RUNTIME}/verify_runtime_profile.py")
_SUITE_RUNNERS = {
    "expression": f"{_RUNTIME}/run_carrier_expression_fixtures_windows.ps1",
    "abi": f"{_RUNTIME}/run_carrier_abi_fixtures_windows.ps1",
    "cpp-lifetime": f"{_RUNTIME}/run_carrier_cpp_lifetime_fixtures_windows.ps1",
    "arc-lifetime": f"{_RUNTIME}/run_carrier_arc_lifetime_fixtures_windows.ps1",
    "match": f"{_RUNTIME}/run_native_match_fixtures_windows.ps1",
    "transfer": f"{_RUNTIME}/run_carrier_transfer_fixtures_windows.ps1",
    "typed-errors": f"{_RUNTIME}/run_native_typed_error_fixtures_windows.ps1",
}
_WITNESSES = {
    "match": ("tests/clang_foundation/match-native-runtime.mm",
              "tests/clang_foundation/match-guard-cleanup-runtime.mm"),
    "transfer": ("tests/clang_foundation/carrier-transfer-runtime.cpp",
                 "tests/clang_foundation/carrier-transfer-runtime-v2.cpp",
                 "tests/clang_foundation/carrier-transfer-local-runtime.mm"),
}
_PACKAGE_SOURCES = {
    "provider": "tests/clang_foundation/native_package/provider.m",
    "managed_support": "tests/clang_foundation/native_package/managed-value-support.h",
    "eager": "tests/clang_foundation/native_package/consumer-eager.m",
    "source": "tests/clang_foundation/native_package/consumer-source.m",
}
_EDITOR_SOURCE = "scripts/objc3c_editor_tooling/qualification.py"
_EDITOR_INPUTS = (
    _EDITOR_SOURCE,
    "scripts/objc3c_editor_tooling/lsp_session.py",
    "tests/tooling/fixtures/objc3_editor/qualification-cases.json",
    "tests/tooling/fixtures/objc3_editor/reserved_intrinsics.m",
    "tests/tooling/fixtures/objc3_editor/reserved_intrinsics.mm",
    "tests/tooling/fixtures/objc3_editor/reserved_intrinsics.formatted",
)
_GENERATED_ROOTS = {"out", "temp", "tmp", "artifacts", "__pycache__"}


def _file(repo: Path, relative: str) -> str:
    candidate = repo / relative
    resolved = candidate.resolve()
    try:
        real_relative = resolved.relative_to(repo)
    except ValueError as error:
        raise ValueError(f"validation input escapes repository: {relative}") from error
    if real_relative.parts and real_relative.parts[0] in _GENERATED_ROOTS:
        raise ValueError(f"validation input is generated output: {relative}")
    if not resolved.is_file():
        raise ValueError(f"validation input is missing: {relative}")
    return real_relative.as_posix()


def _source_files(repo: Path, relative: str, suffixes: tuple[str, ...]) -> list[str]:
    directory = repo / relative
    if not directory.exists() and not directory.is_symlink():
        raise ValueError(f"validation source directory is missing: {relative}")
    try:
        directory.resolve().relative_to(repo)
    except ValueError as error:
        raise ValueError(f"validation source directory escapes repository: {relative}") from error
    if not directory.is_dir():
        raise ValueError(f"validation source directory is not a directory: {relative}")
    files = sorted(_file(repo, path.relative_to(repo).as_posix())
                   for path in directory.rglob("*")
                   if path.suffix in suffixes and (path.is_file() or path.is_symlink())
                   and "__pycache__" not in path.parts)
    if not files:
        raise ValueError(f"validation source directory has no source inputs: {relative}")
    return files


def _selectors(values: Sequence[str], accepted: Sequence[str], kind: str) -> set[str]:
    if isinstance(values, (str, bytes)):
        raise ValueError(f"{kind} selectors must be a sequence of exact names")
    try:
        requested = set(values)
    except TypeError as error:
        raise ValueError(f"{kind} selectors must be a sequence of exact names") from error
    if any(not isinstance(value, str) or not value for value in requested):
        raise ValueError(f"{kind} selectors must be nonempty exact names")
    unknown = requested - set(accepted)
    if unknown:
        raise ValueError(f"unknown {kind} selector: " + ", ".join(sorted(unknown)))
    return requested


def build_plan(
    repo: Path,
    families: Sequence[str] = (),
    node_ids: Sequence[str] = (),
    drivers: Sequence[str] = (),
) -> dict[str, Any]:
    """Build the complete inventory and a dependency-closed filtered selection."""
    repo = Path(repo).resolve()
    if not repo.is_dir():
        raise ValueError(f"repository root is missing or not a directory: {repo}")
    requested_families = _selectors(families, FAMILIES, "family")
    requested_drivers = _selectors(drivers, DRIVERS, "driver")
    common = [_file(repo, path) for path in _COMMON_INPUTS]
    common += [_file(repo, f"{_VALIDATION}/{name}") for name in _COMMON_SIBLINGS]
    # Tracked patches own the generated LLVM fixtures and implementation. Never
    # hash generated development checkouts or fixture outputs as source inputs.
    common += _source_files(repo, "toolchains/llvm/patches", (".patch",))
    runtime = [_file(repo, path) for path in _RUNTIME_INPUTS]
    native = _source_files(repo, "native/objc3c/src/clang_foundation", (".cpp", ".h"))
    fixture_impl = [_file(repo, f"{_VALIDATION}/fixture_nodes.py"), _file(repo, f"{_VALIDATION}/fixture_linux.py")]
    package_impl = [_file(repo, f"{_VALIDATION}/package_nodes.py")]
    nodes: list[dict[str, Any]] = []

    def add(node_id: str, family: str, phase: str, operation: str, driver: str,
            source: str, inputs: Sequence[str], dependencies: Sequence[str] = (),
            **details: str) -> None:
        nodes.append({"node_id": node_id, "family_id": family, "phase": phase,
                      "operation": operation, "driver": driver,
                      "source": _file(repo, source),
                      "identity_inputs": sorted(set(common + [_file(repo, source)] + list(inputs))),
                      "depends_on": list(dependencies), **details})

    for suite in FIXTURE_SUITES:
        suite_inputs = runtime + fixture_impl
        if suite == "typed-errors":
            suite_inputs += [_file(repo, f"{_RUNTIME}/run_expected_trap_windows.py")]
        if suite in _WITNESSES:
            suite_inputs += [_file(repo, path) for path in _WITNESSES[suite]]
            suite_inputs += [_file(repo, f"{_RUNTIME}/run_native_match_transfer_fixtures_windows.ps1")]
        supported = DRIVERS if suite in ("match", "transfer") else DRIVERS[:2]
        for driver in supported:
            family = "typed-errors" if suite == "typed-errors" else "carriers"
            node_id = f"typed-errors:{driver}" if suite == "typed-errors" else f"carriers:{suite}:{driver}"
            add(node_id, family, "fixture", "fixture", driver,
                _SUITE_RUNNERS[suite], suite_inputs + (native if driver == "objc3c-clang" else []),
                suite=suite)

    package_inputs = runtime + native + package_impl + [_file(repo, _PACKAGE_SOURCES["managed_support"])]
    add("native-package:provider", "native-package", "provider", "package-provider",
        "objc3c-clang", _PACKAGE_SOURCES["provider"], package_inputs)
    for name, mode in (("eager", "eager"), ("source", "source-directed")):
        add(f"native-package:consumer-{name}", "native-package", "consumer-compile",
            "package-consumer", "objc3c-clang", _PACKAGE_SOURCES[name], package_inputs,
            ["native-package:provider"], mode=mode)
    add("editor:qualification", "editor", "editor", "editor", "clang", _EDITOR_SOURCE,
        [_file(repo, path) for path in _EDITOR_INPUTS])

    by_id = {node["node_id"]: node for node in nodes}
    requested_nodes = _selectors(node_ids, tuple(by_id), "node")
    allowed_families = requested_families or set(FAMILIES)
    allowed_drivers = requested_drivers or set(DRIVERS)
    if requested_nodes:
        incompatible = [node_id for node_id in sorted(requested_nodes)
                        if by_id[node_id]["family_id"] not in allowed_families
                        or by_id[node_id]["driver"] not in allowed_drivers]
        if incompatible:
            raise ValueError("node selector conflicts with family/driver filter: " +
                             ", ".join(incompatible))
        selected = set(requested_nodes)
    else:
        selected = {node["node_id"] for node in nodes
                    if node["family_id"] in allowed_families and node["driver"] in allowed_drivers}
    if not selected:
        raise ValueError("selectors do not match any native validation node")
    pending = list(selected)
    while pending:
        for dependency in by_id[pending.pop()]["depends_on"]:
            if dependency not in selected:
                selected.add(dependency)
                pending.append(dependency)
    ordered = [node for node in nodes if node["node_id"] in selected]
    selected_families = sorted({node["family_id"] for node in ordered})
    return {
        "nodes": nodes,
        "selected_nodes": ordered,
        "selected_node_ids": [node["node_id"] for node in ordered],
        "unselected_node_ids": [node["node_id"] for node in nodes if node["node_id"] not in selected],
        "selected_families": selected_families,
        "unselected_families": sorted(set(FAMILIES) - set(selected_families)),
        "selection_complete": True,
    }


__all__ = ["FAMILIES", "DRIVERS", "FIXTURE_SUITES", "build_plan"]
