from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Iterator

from objc3c_tooling.paths import repo_rel

from .constants import STRICT_KINDS


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
        return tuple(
            fixture for fixture in self.fixtures if fixture.owner_phase == owner_phase
        )

    def family(
        self,
        owner_phase: str,
        behavior_family: str,
    ) -> tuple[BehaviorFixture, ...]:
        return tuple(
            fixture
            for fixture in self.fixtures
            if fixture.owner_phase == owner_phase
            and fixture.behavior_family == behavior_family
        )

    def retired_surface_fixtures(self) -> tuple[BehaviorFixture, ...]:
        return tuple(fixture for fixture in self.fixtures if fixture.retired_surface_tags)

    def compiler_driver_fixtures(self) -> tuple[BehaviorFixture, ...]:
        return tuple(
            fixture
            for fixture in self.fixtures
            if fixture.expected_stage in {"parse", "compile"}
            and not fixture.requires_live_runtime_dispatch
        )
