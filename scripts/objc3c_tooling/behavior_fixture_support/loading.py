from __future__ import annotations

from pathlib import Path
from typing import Iterable

from objc3c_tooling.json_io import load_json_object
from objc3c_tooling.paths import repo_rel

from .constants import NATIVE_ROOT, REQUIRED_TREE
from .models import BehaviorFixture, BehaviorFixtureCatalog
from .validation import validate_behavior_fixture


def fixture_path_for_metadata(meta_path: Path) -> Path:
    return meta_path.with_name(meta_path.name.removesuffix(".meta.json") + ".objc3")


def iter_required_behavior_directories(*, native_root: Path = NATIVE_ROOT) -> Iterable[Path]:
    for phase, families in REQUIRED_TREE.items():
        yield native_root / phase
        for family in families:
            yield native_root / phase / family


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


def load_behavior_fixture_catalog(
    *,
    native_root: Path = NATIVE_ROOT,
) -> BehaviorFixtureCatalog:
    return BehaviorFixtureCatalog(tuple(load_behavior_fixtures(native_root=native_root)))
