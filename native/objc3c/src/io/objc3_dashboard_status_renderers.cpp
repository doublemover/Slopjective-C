#include "io/objc3_dashboard_status_renderers.h"

#include <initializer_list>
#include <sstream>
#include <string_view>

#include "io/objc3_process_internal.h"

namespace {

std::string RenderDashboardDependencyStatus(std::string_view status) {
  std::ostringstream out;
  JsonObjectWriter dependency_status(out);
  dependency_status.StringField("B-04", status);
  dependency_status.StringField("B-10", status);
  dependency_status.StringField("B-11", status);
  dependency_status.StringField("B-12", status);
  return FinishJsonObject(dependency_status, out);
}

std::string RenderDashboardProfile(std::string_view profile_id,
                                   std::string_view status,
                                   std::string_view blocker_ids) {
  std::ostringstream out;
  JsonObjectWriter profile(out);
  profile.StringField("profile_id", profile_id);
  profile.StringField("status", status);
  profile.RawJsonField("dependency_status", RenderDashboardDependencyStatus(status));
  profile.StringField("last_refresh", kObjc3DeterministicReplayTimestamp);
  profile.RawJsonField("blocker_ids", blocker_ids);
  return FinishJsonObject(profile, out);
}

std::string RenderDashboardDependency(std::string_view dependency_id,
                                      std::string_view artifact_ref) {
  std::ostringstream out;
  JsonObjectWriter dependency(out);
  dependency.StringField("dependency_id", dependency_id);
  dependency.StringField("status", "pass");
  dependency.StringField("refreshed_at", kObjc3DeterministicReplayTimestamp);
  dependency.IntField("stale_after_hours", 24);
  dependency.RawJsonField(
      "artifact_refs",
      BuildIndentedStringArrayJson({std::string(artifact_ref)}, "    "));
  dependency.RawJsonField("failure_codes", "[]");
  return FinishJsonObject(dependency, out);
}

std::string RenderDashboardArtifact(std::string_view artifact_id,
                                    std::string_view dependency_id,
                                    std::string_view artifact_path,
                                    std::string_view artifact_json,
                                    std::string_view issue_ref) {
  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("artifact_id", artifact_id);
  artifact.StringField("dependency_id", dependency_id);
  artifact.StringField("profile_scope", "all");
  artifact.StringField("artifact_path", artifact_path);
  artifact.StringField(
      "file_sha256",
      ComputeSha256ShapedContentDigest(std::string(artifact_json)));
  artifact.StringField("generated_at", kObjc3DeterministicReplayTimestamp);
  artifact.StringField("source_revision", kObjc3DeterministicSourceRevision);
  artifact.StringField("validation_state", "valid");
  artifact.StringField("issue_ref", issue_ref);
  return FinishJsonObject(artifact, out);
}

std::string RenderDashboardCountObject(std::initializer_list<const char *> keys,
                                       int pass_count) {
  std::ostringstream out;
  JsonObjectWriter counts(out);
  bool first = true;
  for (const char *key : keys) {
    counts.IntField(key, first ? pass_count : 0);
    first = false;
  }
  return FinishJsonObject(counts, out);
}

std::string RenderDashboardProfileCounts() {
  std::ostringstream out;
  JsonObjectWriter counts(out);
  counts.IntField("pass", 3);
  counts.IntField("fail", 0);
  counts.IntField("blocked", 1);
  counts.IntField("incomplete", 0);
  return FinishJsonObject(counts, out);
}

std::string RenderDashboardBlockerCounts() {
  std::ostringstream out;
  JsonObjectWriter counts(out);
  counts.IntField("open", 1);
  counts.IntField("resolved", 0);
  counts.IntField("high_or_critical", 1);
  return FinishJsonObject(counts, out);
}

}  // namespace

std::string RenderDashboardProfiles() {
  std::ostringstream out;
  objc3::io::json::JsonArrayWriter profiles(out);
  profiles.RawJsonValue(RenderDashboardProfile("core", "pass", "[]"));
  profiles.RawJsonValue(RenderDashboardProfile("strict", "pass", "[]"));
  profiles.RawJsonValue(
      RenderDashboardProfile("strict-concurrency", "pass", "[]"));
  profiles.RawJsonValue(RenderDashboardProfile(
      "strict-system", "blocked", "[\"BLK-STRICT-PROFILES\"]"));
  profiles.End();
  return out.str();
}

std::string RenderDashboardDependencies() {
  std::ostringstream out;
  objc3::io::json::JsonArrayWriter dependencies(out);
  dependencies.RawJsonValue(RenderDashboardDependency("B-04", "ART-B04-REPORT"));
  dependencies.RawJsonValue(
      RenderDashboardDependency("B-10", "ART-B10-PUBLICATION"));
  dependencies.RawJsonValue(
      RenderDashboardDependency("B-11", "ART-B11-VALIDATION"));
  dependencies.RawJsonValue(
      RenderDashboardDependency("B-12", "ART-B12-RELEASE-EVIDENCE"));
  dependencies.End();
  return out.str();
}

std::string RenderDashboardArtifacts(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json) {
  std::ostringstream out;
  objc3::io::json::JsonArrayWriter artifacts(out);
  artifacts.RawJsonValue(RenderDashboardArtifact(
      "ART-B04-REPORT", "B-04", inputs.report_artifact_path, report_json,
      "#140/conformance-report"));
  artifacts.RawJsonValue(RenderDashboardArtifact(
      "ART-B10-PUBLICATION", "B-10", inputs.publication_artifact_path,
      publication_json, "#158/conformance-publication"));
  artifacts.RawJsonValue(RenderDashboardArtifact(
      "ART-B11-VALIDATION", "B-11", inputs.validation_artifact_path,
      validation_json, "#161/conformance-validation"));
  artifacts.RawJsonValue(RenderDashboardArtifact(
      "ART-B12-RELEASE-EVIDENCE", "B-12",
      inputs.release_evidence_operation_artifact_path,
      release_evidence_operation_json, "#167/release-evidence"));
  artifacts.End();
  return out.str();
}

std::string RenderDashboardBlockers() {
  std::ostringstream entry_out;
  JsonObjectWriter blocker(entry_out);
  blocker.StringField("blocker_id", "BLK-STRICT-PROFILES");
  blocker.StringField("severity", "high");
  blocker.StringField("state", "open");
  blocker.StringField(
      "title",
      "Strict-system remains targeted but unclaimed until system evidence lands");
  blocker.RawJsonField(
      "dependency_ids",
      BuildIndentedStringArrayJson({"B-04", "B-10", "B-11", "B-12"}, "    "));
  blocker.RawJsonField(
      "profile_ids",
      BuildIndentedStringArrayJson(
          {"strict-system"}, "    "));
  blocker.StringField("created_at", kObjc3DeterministicReplayTimestamp);
  blocker.StringField("owner", "objc3-roadmap");
  blocker.StringField("failure_code", "DASH-B04-STRICT-SYSTEM-NOT-CLAIMED");
  blocker.RawJsonField(
      "artifact_refs",
      BuildIndentedStringArrayJson({"ART-B04-REPORT", "ART-B10-PUBLICATION",
                                    "ART-B11-VALIDATION",
                                    "ART-B12-RELEASE-EVIDENCE"},
                                   "    "));
  blocker.StringField("taxonomy", "coverage-gap");

  std::ostringstream out;
  objc3::io::json::JsonArrayWriter blockers(out);
  blockers.RawJsonValue(FinishJsonObject(blocker, entry_out));
  blockers.End();
  return out.str();
}

std::string RenderDashboardSummary() {
  std::ostringstream out;
  JsonObjectWriter summary(out);
  summary.RawJsonField("profile_counts", RenderDashboardProfileCounts());
  summary.RawJsonField(
      "dependency_counts",
      RenderDashboardCountObject({"pass", "fail", "blocked", "stale",
                                  "missing"},
                                 4));
  summary.RawJsonField("blocker_counts", RenderDashboardBlockerCounts());
  return FinishJsonObject(summary, out);
}

std::string RenderDashboardRefresh() {
  std::ostringstream cadence_out;
  JsonObjectWriter cadence(cadence_out);
  cadence.IntField("merge_latency_target_minutes", 30);
  cadence.IntField("scheduled_latency_target_minutes", 60);
  cadence.IntField("rc_fast_refresh_hours", 4);

  std::ostringstream out;
  JsonObjectWriter refresh(out);
  refresh.StringField("trigger", "manual-replay");
  refresh.RawJsonField("cadence", FinishJsonObject(cadence, cadence_out));
  refresh.StringField("last_successful_refresh",
                      kObjc3DeterministicReplayTimestamp);
  refresh.StringField("next_scheduled_refresh",
                      kObjc3DeterministicReplayTimestamp);
  refresh.RawJsonField("stale_dependency_ids", "[]");
  refresh.IntField("missed_scheduled_refreshes", 0);
  refresh.StringField("escalation_state", "high");
  return FinishJsonObject(refresh, out);
}

std::string RenderDashboardChangeHistory() {
  std::ostringstream entry_out;
  JsonObjectWriter entry(entry_out);
  entry.StringField("snapshot_id", kObjc3DashboardReleaseId);
  entry.RawJsonField("previous_snapshot_id", "null");
  entry.StringField("change_kind", "refresh-only");
  entry.StringField("changed_at", kObjc3DeterministicReplayTimestamp);
  entry.StringField(
      "summary",
      "Deterministic claim dashboard refresh with strict-system blocked.");

  std::ostringstream out;
  objc3::io::json::JsonArrayWriter history(out);
  history.RawJsonValue(FinishJsonObject(entry, entry_out));
  history.End();
  return out.str();
}
