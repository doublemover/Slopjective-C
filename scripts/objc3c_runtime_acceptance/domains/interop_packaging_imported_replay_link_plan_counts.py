"""Imported-runtime packaging replay link-plan aggregate assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_link_plan_counts(
    link_plan: dict[str, Any],
    provider_registration_manifest: dict[str, Any],
    consumer_registration_manifest: dict[str, Any],
) -> None:
    expected_imported_counts = {
        "imported_class_descriptor_count": provider_registration_manifest[
            "class_descriptor_count"
        ],
        "imported_protocol_descriptor_count": provider_registration_manifest[
            "protocol_descriptor_count"
        ],
        "imported_category_descriptor_count": provider_registration_manifest[
            "category_descriptor_count"
        ],
        "imported_property_descriptor_count": provider_registration_manifest[
            "property_descriptor_count"
        ],
        "imported_ivar_descriptor_count": provider_registration_manifest[
            "ivar_descriptor_count"
        ],
        "imported_total_descriptor_count": provider_registration_manifest[
            "total_descriptor_count"
        ],
    }
    expected_local_counts = {
        "local_class_descriptor_count": consumer_registration_manifest[
            "class_descriptor_count"
        ],
        "local_protocol_descriptor_count": consumer_registration_manifest[
            "protocol_descriptor_count"
        ],
        "local_category_descriptor_count": consumer_registration_manifest[
            "category_descriptor_count"
        ],
        "local_property_descriptor_count": consumer_registration_manifest[
            "property_descriptor_count"
        ],
        "local_ivar_descriptor_count": consumer_registration_manifest[
            "ivar_descriptor_count"
        ],
        "local_total_descriptor_count": consumer_registration_manifest[
            "total_descriptor_count"
        ],
    }
    for field_name, expected_value in expected_imported_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    for field_name, expected_value in expected_local_counts.items():
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    expect(
        link_plan.get("module_image_count") == 2,
        "expected cross-module link plan to preserve a two-image runtime topology",
    )
    expect(
        link_plan.get("module_names_lexicographic")
        == ["runtimePackagingConsumer", "runtimePackagingProvider"],
        "expected cross-module link plan to preserve the stable module-name ordering",
    )
    expect(
        link_plan.get("direct_import_input_count") == 1,
        "expected cross-module link plan to preserve one direct imported runtime surface",
    )
    direct_import_surface_artifact_paths = link_plan.get(
        "direct_import_surface_artifact_paths"
    )
    expect(
        isinstance(direct_import_surface_artifact_paths, list)
        and len(direct_import_surface_artifact_paths) == 1
        and direct_import_surface_artifact_paths[0].endswith(
            "provider/module.runtime-import-surface.json"
        ),
        "expected cross-module link plan to preserve the imported runtime surface artifact path",
    )
    for field_name, expected_value in (
        (
            "transitive_class_descriptor_count",
            provider_registration_manifest["class_descriptor_count"]
            + consumer_registration_manifest["class_descriptor_count"],
        ),
        (
            "transitive_protocol_descriptor_count",
            provider_registration_manifest["protocol_descriptor_count"]
            + consumer_registration_manifest["protocol_descriptor_count"],
        ),
        (
            "transitive_category_descriptor_count",
            provider_registration_manifest["category_descriptor_count"]
            + consumer_registration_manifest["category_descriptor_count"],
        ),
        (
            "transitive_property_descriptor_count",
            provider_registration_manifest["property_descriptor_count"]
            + consumer_registration_manifest["property_descriptor_count"],
        ),
        (
            "transitive_ivar_descriptor_count",
            provider_registration_manifest["ivar_descriptor_count"]
            + consumer_registration_manifest["ivar_descriptor_count"],
        ),
        (
            "transitive_total_descriptor_count",
            provider_registration_manifest["total_descriptor_count"]
            + consumer_registration_manifest["total_descriptor_count"],
        ),
    ):
        expect(
            link_plan.get(field_name) == expected_value,
            f"expected cross-module link plan to preserve {field_name}",
        )
    link_object_artifacts = link_plan.get("link_object_artifacts")
    expect(
        isinstance(link_object_artifacts, list) and len(link_object_artifacts) == 2,
        "expected cross-module link plan to publish two ordered link objects",
    )


__all__ = ["assert_imported_runtime_link_plan_counts"]
