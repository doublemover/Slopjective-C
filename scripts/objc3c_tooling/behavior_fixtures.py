from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable, Iterator

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import ROOT, repo_rel

NATIVE_ROOT = ROOT / "tests" / "native"
FIXTURE_ROOT = ROOT / "tests" / "fixtures"

PHASE_ORDER: tuple[str, ...] = ("parser", "sema", "lowering", "ir", "runtime", "e2e")
REQUIRED_TREE: dict[str, tuple[str, ...]] = {
    "parser": ("positive", "negative", "snapshots"),
    "sema": (
        "types",
        "ownership",
        "objc",
        "control_flow",
        "errors",
        "concurrency",
        "negative",
    ),
    "lowering": ("expressions", "statements", "objc_runtime", "ownership", "errors"),
    "ir": ("module", "function", "metadata", "runtime_calls"),
    "runtime": (
        "dispatch",
        "object_model",
        "storage",
        "arc",
        "blocks",
        "errors",
        "concurrency",
    ),
    "e2e": ("smoke", "feature_matrix", "negative_execution"),
}

STRICT_KINDS = {"negative", "strict-error", "rejection"}
FIXTURE_KINDS = {"positive", *STRICT_KINDS}
EXPECTED_STAGES = {"parse", "compile", "link", "run"}
RETIRED_SURFACE_TAGS = frozenset({"old-mode", "runtime-adapter", "runtime-dispatch"})


@dataclass(frozen=True)
class BehaviorFixture:
    source_path: Path
    metadata_path: Path
    metadata: dict[str, Any]

    @property
    def owner_phase(self) -> str:
        return str(self.metadata["owner_phase"])

    @property
    def behavior_family(self) -> str:
        return str(self.metadata["behavior_family"])

    @property
    def fixture_kind(self) -> str:
        return str(self.metadata["fixture_kind"])

    @property
    def expected(self) -> dict[str, Any]:
        expected = self.metadata.get("expected")
        if not isinstance(expected, dict):
            raise RuntimeError(f"missing expected object in {repo_rel(self.metadata_path)}")
        return expected

    @property
    def execution(self) -> dict[str, Any]:
        execution = self.metadata.get("execution", {})
        if not isinstance(execution, dict):
            raise RuntimeError(f"execution must be an object in {repo_rel(self.metadata_path)}")
        return execution

    @property
    def expected_stage(self) -> str:
        return str(self.expected["stage"])

    @property
    def expected_diagnostic_code(self) -> str:
        return str(self.expected.get("diagnostic_code", ""))

    @property
    def required_tokens(self) -> list[str]:
        raw_tokens = self.expected.get("required_tokens", [])
        if not isinstance(raw_tokens, list):
            raise RuntimeError(f"required_tokens must be a list in {repo_rel(self.metadata_path)}")
        return [str(token) for token in raw_tokens if str(token)]

    @property
    def retired_surface_tags(self) -> tuple[str, ...]:
        raw_tags = self.metadata.get("retired_surface_tags", [])
        if not isinstance(raw_tags, list):
            raise RuntimeError(f"retired_surface_tags must be a list in {self.relative_metadata}")
        return tuple(str(tag) for tag in raw_tags)

    @property
    def is_strict(self) -> bool:
        return self.fixture_kind in STRICT_KINDS

    @property
    def phase_family(self) -> tuple[str, str]:
        return (self.owner_phase, self.behavior_family)

    @property
    def relative_source(self) -> str:
        return repo_rel(self.source_path)

    @property
    def relative_metadata(self) -> str:
        return repo_rel(self.metadata_path)

    @property
    def expected_exit_code(self) -> int:
        if "expected_exit_code" in self.execution:
            return int(self.execution["expected_exit_code"])
        exitcode_path = self.source_path.with_suffix(".exitcode.txt")
        if exitcode_path.is_file():
            return int(exitcode_path.read_text(encoding="utf-8").strip())
        return 0

    @property
    def native_compile_args(self) -> list[str]:
        raw_args = self.execution.get("native_compile_args", [])
        if not isinstance(raw_args, list):
            raise RuntimeError(f"native_compile_args must be a list in {repo_rel(self.metadata_path)}")
        return [str(arg) for arg in raw_args]

    @property
    def requires_live_runtime_dispatch(self) -> bool:
        return bool(self.execution.get("requires_live_runtime_dispatch", False))

    @property
    def runtime_dispatch_symbol(self) -> str:
        return str(self.execution.get("runtime_dispatch_symbol", "objc3_runtime_dispatch_i32"))

    def canonical_manifest_entry(self) -> dict[str, Any]:
        entry: dict[str, Any] = {
            "path": self.relative_source,
            "origin": str(self.metadata["origin"]),
            "owner_phase": self.owner_phase,
            "behavior_family": self.behavior_family,
            "fixture_kind": self.fixture_kind,
        }
        if self.retired_surface_tags:
            entry["retired_surface_tags"] = list(self.retired_surface_tags)
        entry["expected_diagnostic_code"] = self.expected_diagnostic_code
        return entry


@dataclass(frozen=True)
class BehaviorFixtureCatalog:
    fixtures: tuple[BehaviorFixture, ...]

    def __iter__(self) -> Iterator[BehaviorFixture]:
        return iter(self.fixtures)

    @property
    def covered_phases(self) -> set[str]:
        return {fixture.owner_phase for fixture in self.fixtures}

    def by_relative_source(self) -> dict[str, BehaviorFixture]:
        return {fixture.relative_source: fixture for fixture in self.fixtures}

    def phase(self, owner_phase: str) -> tuple[BehaviorFixture, ...]:
        return tuple(fixture for fixture in self.fixtures if fixture.owner_phase == owner_phase)

    def family(self, owner_phase: str, behavior_family: str) -> tuple[BehaviorFixture, ...]:
        return tuple(
            fixture
            for fixture in self.fixtures
            if fixture.owner_phase == owner_phase and fixture.behavior_family == behavior_family
        )

    def retired_surface_fixtures(self) -> tuple[BehaviorFixture, ...]:
        return tuple(fixture for fixture in self.fixtures if fixture.retired_surface_tags)

    def compiler_driver_fixtures(self) -> tuple[BehaviorFixture, ...]:
        return tuple(
            fixture
            for fixture in self.fixtures
            if fixture.expected_stage in {"parse", "compile"} and not fixture.requires_live_runtime_dispatch
        )


def fixture_path_for_metadata(meta_path: Path) -> Path:
    return meta_path.with_name(meta_path.name.removesuffix(".meta.json") + ".objc3")


def iter_required_behavior_directories(*, native_root: Path = NATIVE_ROOT) -> Iterable[Path]:
    for phase, families in REQUIRED_TREE.items():
        yield native_root / phase
        for family in families:
            yield native_root / phase / family


def validate_behavior_fixture(fixture: BehaviorFixture) -> None:
    metadata = fixture.metadata
    if metadata.get("schema_version") != 1:
        raise RuntimeError(f"schema_version must be 1 in {fixture.relative_metadata}")
    if metadata.get("fixture") != fixture.source_path.name:
        raise RuntimeError(f"fixture field must match source filename in {fixture.relative_metadata}")
    if metadata.get("origin") != "hand-authored":
        raise RuntimeError(f"native behavior fixture must be hand-authored in {fixture.relative_metadata}")

    owner_phase = fixture.owner_phase
    behavior_family = fixture.behavior_family
    if owner_phase not in REQUIRED_TREE:
        raise RuntimeError(f"unknown owner_phase '{owner_phase}' in {fixture.relative_metadata}")
    if behavior_family not in REQUIRED_TREE[owner_phase]:
        raise RuntimeError(
            f"unknown behavior_family '{behavior_family}' for phase '{owner_phase}' "
            f"in {fixture.relative_metadata}"
        )

    relative_parts = fixture.source_path.relative_to(NATIVE_ROOT).parts
    if len(relative_parts) < 3:
        raise RuntimeError(f"fixture is not under a behavior family: {fixture.relative_source}")
    if relative_parts[0] != owner_phase or relative_parts[1] != behavior_family:
        raise RuntimeError(
            f"fixture path and metadata owner disagree: {fixture.relative_source} "
            f"vs {owner_phase}/{behavior_family}"
        )

    expected_stage = fixture.expected_stage
    if expected_stage not in EXPECTED_STAGES:
        raise RuntimeError(f"unknown expected stage '{expected_stage}' in {fixture.relative_metadata}")
    if fixture.fixture_kind not in FIXTURE_KINDS:
        raise RuntimeError(f"unknown fixture_kind '{fixture.fixture_kind}' in {fixture.relative_metadata}")
    if fixture.is_strict:
        if not fixture.expected_diagnostic_code:
            raise RuntimeError(f"strict fixture must declare diagnostic code: {fixture.relative_metadata}")
        if not fixture.required_tokens:
            raise RuntimeError(f"strict fixture must declare diagnostic tokens: {fixture.relative_metadata}")
    else:
        if fixture.expected_diagnostic_code:
            raise RuntimeError(f"positive fixture must not declare diagnostic code: {fixture.relative_metadata}")
        if fixture.required_tokens:
            raise RuntimeError(f"positive fixture must not declare diagnostic tokens: {fixture.relative_metadata}")

    retired_tags = set(fixture.retired_surface_tags)
    if len(retired_tags) != len(fixture.retired_surface_tags):
        raise RuntimeError(f"retired_surface_tags must not contain duplicates in {fixture.relative_metadata}")
    unknown_tags = retired_tags - RETIRED_SURFACE_TAGS
    if unknown_tags:
        raise RuntimeError(
            f"unknown retired_surface_tags {sorted(unknown_tags)!r} in {fixture.relative_metadata}"
        )
    if retired_tags and not fixture.is_strict:
        raise RuntimeError(f"retired surfaces must be strict fixtures: {fixture.relative_metadata}")

    execution = fixture.execution
    requires_live_runtime_dispatch = execution.get("requires_live_runtime_dispatch")
    if requires_live_runtime_dispatch is not None and not isinstance(requires_live_runtime_dispatch, bool):
        raise RuntimeError(
            f"requires_live_runtime_dispatch must be a boolean in {fixture.relative_metadata}"
        )
    expected_exit_code = execution.get("expected_exit_code")
    if expected_exit_code is not None and (
        not isinstance(expected_exit_code, int) or isinstance(expected_exit_code, bool)
    ):
        raise RuntimeError(f"expected_exit_code must be an integer in {fixture.relative_metadata}")


def load_behavior_fixtures(*, native_root: Path = NATIVE_ROOT) -> list[BehaviorFixture]:
    fixtures: list[BehaviorFixture] = []
    for meta_path in sorted(native_root.rglob("*.meta.json")):
        source_path = fixture_path_for_metadata(meta_path)
        metadata = load_json_object(meta_path)
        fixture = BehaviorFixture(
            source_path=source_path,
            metadata_path=meta_path,
            metadata=metadata,
        )
        if not source_path.is_file():
            raise RuntimeError(f"missing source fixture for metadata: {fixture.relative_metadata}")
        validate_behavior_fixture(fixture)
        fixtures.append(fixture)
    return fixtures


def load_behavior_fixture_catalog(*, native_root: Path = NATIVE_ROOT) -> BehaviorFixtureCatalog:
    return BehaviorFixtureCatalog(tuple(load_behavior_fixtures(native_root=native_root)))


def load_manifest_fixture_entries(manifest_path: Path) -> list[dict[str, Any]]:
    manifest = load_json_object(manifest_path)
    raw_fixtures = manifest.get("fixtures", [])
    if not isinstance(raw_fixtures, list):
        raise RuntimeError(f"fixtures must be a list in {repo_rel(manifest_path)}")
    entries: list[dict[str, Any]] = []
    seen_paths: set[str] = set()
    for entry in raw_fixtures:
        if not isinstance(entry, dict):
            raise RuntimeError(f"manifest fixture entries must be objects in {repo_rel(manifest_path)}")
        path = entry.get("path")
        if not isinstance(path, str) or not path:
            raise RuntimeError(f"manifest fixture entry missing path in {repo_rel(manifest_path)}")
        if path in seen_paths:
            raise RuntimeError(f"manifest fixture entry duplicates path {path!r} in {repo_rel(manifest_path)}")
        seen_paths.add(path)
        entries.append(entry)
    return entries


def load_manifest_fixture_paths(manifest_path: Path) -> set[str]:
    return {entry["path"] for entry in load_manifest_fixture_entries(manifest_path)}
