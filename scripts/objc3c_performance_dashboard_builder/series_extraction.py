from __future__ import annotations

from datetime import datetime, timezone

from objc3c_tooling.paths import repo_rel

from objc3c_performance_dashboard.metrics import load_packet_machine_profiles
from objc3c_performance_dashboard.metrics import report_timestamp
from objc3c_performance_dashboard.metrics import summarize_machine_profile
from objc3c_performance_dashboard.paths import COMPILER_SUMMARY_PATH
from objc3c_performance_dashboard.paths import PERFORMANCE_SUMMARY_PATH
from objc3c_performance_dashboard.paths import RUNTIME_SUMMARY_PATH

from objc3c_performance_dashboard_builder.input_loading import DashboardInputs
from objc3c_performance_dashboard_builder.models import DashboardSeries
from objc3c_performance_dashboard_builder.normalization import collect_comparative_packets
from objc3c_performance_dashboard_builder.normalization import collect_packet_paths

MAX_REPORT_AGE_HOURS = 24.0


def build_dashboard_series(
    inputs: DashboardInputs,
    *,
    now: datetime | None = None,
) -> DashboardSeries:
    observed_at = now or datetime.now(timezone.utc)
    compile_packets = collect_comparative_packets(inputs.comparative_summary, marker="/compile/")
    runtime_packets = collect_comparative_packets(inputs.comparative_summary, marker="/runtime/")
    derived = {
        "compile_packet_count": len(compile_packets),
        "runtime_packet_count": len(runtime_packets),
        "performance_report_age_hours": round(
            (observed_at - report_timestamp(inputs.performance_summary, PERFORMANCE_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
        "compiler_throughput_report_age_hours": round(
            (observed_at - report_timestamp(inputs.compiler_summary, COMPILER_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
        "runtime_performance_report_age_hours": round(
            (observed_at - report_timestamp(inputs.runtime_summary, RUNTIME_SUMMARY_PATH)).total_seconds()
            / 3600.0,
            3,
        ),
    }

    packet_paths = collect_packet_paths(
        inputs.performance_summary,
        inputs.comparative_summary,
        inputs.runtime_summary,
    )
    machine_profiles = load_packet_machine_profiles(packet_paths)
    machine_profile_keys = {summarize_machine_profile(profile) for profile in machine_profiles}
    machine_profiles_consistent = len(machine_profile_keys) <= 1

    stale_report_paths: list[str] = []
    for path, age_hours in (
        (repo_rel(PERFORMANCE_SUMMARY_PATH), derived["performance_report_age_hours"]),
        (repo_rel(COMPILER_SUMMARY_PATH), derived["compiler_throughput_report_age_hours"]),
        (repo_rel(RUNTIME_SUMMARY_PATH), derived["runtime_performance_report_age_hours"]),
    ):
        if float(age_hours) > MAX_REPORT_AGE_HOURS:
            stale_report_paths.append(path)

    environment_issues: list[str] = []
    if not machine_profiles_consistent:
        environment_issues.append("machine profile drift detected across live telemetry packets")
    if stale_report_paths:
        environment_issues.append("one or more upstream performance reports exceeded the freshness ceiling")

    return DashboardSeries(
        compile_packets=compile_packets,
        runtime_packets=runtime_packets,
        derived=derived,
        machine_profiles_consistent=machine_profiles_consistent,
        stale_report_paths=stale_report_paths,
        environment_issues=environment_issues,
    )
