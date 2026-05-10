import importlib

from runtime_acceptance_route_owner_split_support import ACTION_ROOT, OWNER_MODULES


def runtime_acceptance_routes_is_public_import_surface_only() -> None:
    facade_text = (ACTION_ROOT / "runtime_acceptance_routes.py").read_text(
        encoding="utf-8"
    )

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.actions.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "dataclass" not in facade_text
    assert "from ..commands import run" not in facade_text
    assert "check_objc3c_runtime_acceptance.py" not in facade_text
