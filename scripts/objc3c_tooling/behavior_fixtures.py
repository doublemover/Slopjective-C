from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterable

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import ROOT, repo_rel

NATIVE_ROOT = ROOT / "tests" / "native"
FIXTURE_ROOT = ROOT / "tests" / "fixtures"

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
EXPECTED_STAGES = {"parse", "compile", "link", "run"}


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
    def runtime_dispatch_symbol(self) -> str:
        return str(self.execution.get("runtime_dispatch_symbol", "objc3_runtime_dispatch_i32"))


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
    if fixture.fixture_kind in STRICT_KINDS:
        if not fixture.expected_diagnostic_code:
            raise RuntimeError(f"strict fixture must declare diagnostic code: {fixture.relative_metadata}")
        if not fixture.required_tokens:
            raise RuntimeError(f"strict fixture must declare diagnostic tokens: {fixture.relative_metadata}")


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


def load_manifest_fixture_paths(manifest_path: Path) -> set[str]:
    manifest = load_json_object(manifest_path)
    raw_fixtures = manifest.get("fixtures", [])
    if not isinstance(raw_fixtures, list):
        raise RuntimeError(f"fixtures must be a list in {repo_rel(manifest_path)}")
    paths: set[str] = set()
    for entry in raw_fixtures:
        if not isinstance(entry, dict):
            raise RuntimeError(f"manifest fixture entries must be objects in {repo_rel(manifest_path)}")
        path = entry.get("path")
        if not isinstance(path, str) or not path:
            raise RuntimeError(f"manifest fixture entry missing path in {repo_rel(manifest_path)}")
        paths.add(path)
    return paths
