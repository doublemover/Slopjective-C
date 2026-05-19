"""Source ownership for Object Model metaclass and canonical sample cases."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..paths import ROOT


def _relative_to_root(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


@dataclass(frozen=True)
class MetaclassGraphRootClassSources:
    case_id: str
    case_dir: Path
    fixture: Path
    probe: Path
    exe_path: Path
    probe_label: str
    positive_execution_fixture: str
    negative_execution_fixture: str

    def fixture_summary_path(self) -> str:
        return _relative_to_root(self.fixture)

    def probe_summary_path(self) -> str:
        return _relative_to_root(self.probe)


@dataclass(frozen=True)
class CanonicalSampleSetSources:
    case_id: str
    case_dir: Path
    fixture: Path
    probe: Path
    exe_path: Path
    probe_label: str

    def fixture_summary_path(self) -> str:
        return _relative_to_root(self.fixture)

    def probe_summary_path(self) -> str:
        return _relative_to_root(self.probe)


def build_metaclass_graph_root_class_sources(
    run_dir: Path,
) -> MetaclassGraphRootClassSources:
    case_id = "metaclass-graph-root-class"
    case_dir = run_dir / case_id
    return MetaclassGraphRootClassSources(
        case_id=case_id,
        case_dir=case_dir,
        fixture=ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "metaclass_graph_root_class_library.objc3",
        probe=ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "metaclass_graph_root_class_probe.cpp",
        exe_path=case_dir / "metaclass_graph_root_class_probe.exe",
        probe_label="metaclass/root-class graph probe",
        positive_execution_fixture="tests/tooling/fixtures/native/execution/positive/class_metaclass_root_runtime_dispatch.objc3",
        negative_execution_fixture="tests/tooling/fixtures/native/execution/negative/class_metaclass_missing_superclass.objc3",
    )


def build_canonical_sample_set_sources(run_dir: Path) -> CanonicalSampleSetSources:
    case_id = "canonical-sample-set"
    case_dir = run_dir / case_id
    return CanonicalSampleSetSources(
        case_id=case_id,
        case_dir=case_dir,
        fixture=ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "canonical_runnable_sample_set.objc3",
        probe=ROOT
        / "tests"
        / "tooling"
        / "runtime"
        / "canonical_runnable_sample_set_probe.cpp",
        exe_path=case_dir / "canonical_runnable_sample_set_probe.exe",
        probe_label="canonical runnable sample set probe",
    )


__all__ = [
    "CanonicalSampleSetSources",
    "MetaclassGraphRootClassSources",
    "build_canonical_sample_set_sources",
    "build_metaclass_graph_root_class_sources",
]
