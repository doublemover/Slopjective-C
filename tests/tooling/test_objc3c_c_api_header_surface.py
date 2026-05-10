from __future__ import annotations

from objc3c_c_api_header_surface_behavior import (
    assert_c_api_header_exposes_public_surface,
    assert_frontend_public_headers_are_domain_owned,
    assert_public_frontend_surface_has_no_lane_or_roadmap_comments,
)
from objc3c_c_api_header_surface_sources import load_c_api_header_surface


def test_c_api_header_exposes_public_surface() -> None:
    assert_c_api_header_exposes_public_surface(load_c_api_header_surface())


def test_frontend_public_headers_are_domain_owned() -> None:
    assert_frontend_public_headers_are_domain_owned()


def test_public_frontend_surface_has_no_lane_or_roadmap_comments() -> None:
    assert_public_frontend_surface_has_no_lane_or_roadmap_comments()
