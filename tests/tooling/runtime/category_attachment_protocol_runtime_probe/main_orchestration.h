#pragma once

#include "attachment_actions.h"
#include "protocol_runtime_assertions.h"
#include "report_helpers.h"
#include "runtime_fixture_setup.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline void CaptureCategoryAttachmentProtocolRuntimeProbe(
    CategoryAttachmentProtocolProbeRun &run) {
  run = CategoryAttachmentProtocolProbeRun{};
  CaptureRealizedGraphState(run.graph_state);
  CaptureRealizedClassEntry(kWidgetClassName, run.widget_entry);
  CaptureRealizedClassEntry(kBaseClassName, run.base_entry);
  CaptureProtocolRuntimeAssertions(run);
  CaptureCategoryAttachmentActions(run);
  CaptureCategoryAttachmentProtocolRuntimeSnapshots(run);
}

inline int RunCategoryAttachmentProtocolRuntimeProbe() {
  CategoryAttachmentProtocolProbeRun run{};
  CaptureCategoryAttachmentProtocolRuntimeProbe(run);
  PrintCategoryAttachmentProtocolRuntimeReport(run);
  return CategoryAttachmentProtocolRuntimeProbePassed(run) ? 0 : 1;
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
