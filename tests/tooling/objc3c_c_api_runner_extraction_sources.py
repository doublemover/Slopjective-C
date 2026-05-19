from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RUNNER_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "tools"
RUNNER_CPP = RUNNER_SOURCE_ROOT / "objc3c_frontend_c_api_runner.cpp"


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def runner_sources() -> list[Path]:
    return sorted(
        path
        for path in RUNNER_SOURCE_ROOT.glob("objc3c_frontend_c_api_runner*.*")
        if path.suffix in {".cpp", ".h"}
    )


def runner_source_text() -> str:
    sources = runner_sources()
    assert RUNNER_CPP in sources
    assert sources
    return "\n".join(read_text(source) for source in sources)
