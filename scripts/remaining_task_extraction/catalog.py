"""Catalog loading, filtering, grouping, and dispatch capacity policy."""

from __future__ import annotations

from remaining_task_extraction.catalog_capacity import capacity_status_severity
from remaining_task_extraction.catalog_capacity import classify_capacity_status
from remaining_task_extraction.catalog_capacity import classify_threshold_status
from remaining_task_extraction.catalog_capacity import compute_capacity_rows
from remaining_task_extraction.catalog_capacity import compute_dispatch_intake_status
from remaining_task_extraction.catalog_capacity import compute_overlap_conflicts
from remaining_task_extraction.catalog_capacity import dispatch_state_for_status
from remaining_task_extraction.catalog_capacity import intake_recommendation_for_status
from remaining_task_extraction.catalog_capacity import ratio
from remaining_task_extraction.catalog_normalization import load_catalog_rows
from remaining_task_extraction.catalog_normalization import normalize_catalog_path
from remaining_task_extraction.catalog_normalization import normalize_inline_text
from remaining_task_extraction.catalog_normalization import normalize_lane
from remaining_task_extraction.catalog_normalization import normalize_status
from remaining_task_extraction.catalog_normalization import parse_positive_line
from remaining_task_extraction.catalog_payload import build_payload
from remaining_task_extraction.catalog_payload import capacity_row_to_dict
from remaining_task_extraction.catalog_payload import overlap_conflict_row_to_dict
from remaining_task_extraction.catalog_payload import row_to_dict
from remaining_task_extraction.catalog_queries import build_groups
from remaining_task_extraction.catalog_queries import count_by
from remaining_task_extraction.catalog_queries import filter_rows
from remaining_task_extraction.catalog_queries import group_value_sort_key
from remaining_task_extraction.catalog_queries import normalize_lane_filter
from remaining_task_extraction.catalog_queries import normalize_status_filter
from remaining_task_extraction.catalog_sorting import capacity_status_sort_key
from remaining_task_extraction.catalog_sorting import dedupe_sort
from remaining_task_extraction.catalog_sorting import lane_sort_key
from remaining_task_extraction.catalog_sorting import row_sort_key
from remaining_task_extraction.catalog_sorting import status_sort_key
from remaining_task_extraction.catalog_sorting import text_sort_key

__all__ = [
    "build_groups",
    "build_payload",
    "capacity_row_to_dict",
    "capacity_status_severity",
    "capacity_status_sort_key",
    "classify_capacity_status",
    "classify_threshold_status",
    "compute_capacity_rows",
    "compute_dispatch_intake_status",
    "compute_overlap_conflicts",
    "count_by",
    "dedupe_sort",
    "dispatch_state_for_status",
    "filter_rows",
    "group_value_sort_key",
    "intake_recommendation_for_status",
    "lane_sort_key",
    "load_catalog_rows",
    "normalize_catalog_path",
    "normalize_inline_text",
    "normalize_lane",
    "normalize_lane_filter",
    "normalize_status",
    "normalize_status_filter",
    "overlap_conflict_row_to_dict",
    "parse_positive_line",
    "ratio",
    "row_sort_key",
    "row_to_dict",
    "status_sort_key",
    "text_sort_key",
]
