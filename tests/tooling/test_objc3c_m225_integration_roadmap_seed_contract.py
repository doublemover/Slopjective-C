from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m225_integration_roadmap_seeding_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M225 integration roadmap seeding",
        "### 1.1 ABI/version continuity planning intake",
        "### 1.2 Gate-evidence planning intake",
        "objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)",
        "objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()",
        "OBJC3C_FRONTEND_VERSION_STRING",
        "OBJC3C_FRONTEND_ABI_VERSION",
        "npm run check:objc3c:m225-roadmap-seeding",
        "tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py",
        "tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py",
        "tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py",
        "tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py",
        "tests/tooling/test_objc3c_m225_integration_roadmap_seed_contract.py",
    ):
        assert text in library_api_doc


def test_m225_integration_roadmap_seeding_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m225-roadmap-seeding"' in package_json
    assert (
        "npm run check:objc3c:m224-integration-release-readiness && python -m pytest "
        "tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py "
        "tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py "
        "tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py "
        "tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py "
        "tests/tooling/test_objc3c_m225_integration_roadmap_seed_contract.py -q"
    ) in package_json

    assert "Run M225 integration roadmap seeding ABI+version export gate" in workflow
    assert "npm run check:objc3c:m225-roadmap-seeding" in workflow
