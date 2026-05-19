#pragma once

#include "attachment_actions.h"
#include "protocol_runtime_assertions.h"
#include "report_helpers.h"
#include "runtime_fixture_setup.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline void CaptureCategoryAttachmentProtocolRuntimeProbe(
    CategoryAttachmentProtocolProbeRun &run) {
  run = CategoryAttachmentProtocolProbeRun{};
  CaptureCategoryAttachmentActions(run);
  CaptureProtocolRuntimeAssertions(run);
  CaptureCategoryAttachmentProtocolRuntimeSnapshots(run);
}

inline int RunCategoryAttachmentProtocolRuntimeProbe() {
  CategoryAttachmentProtocolProbeRun run{};
  CaptureCategoryAttachmentProtocolRuntimeProbe(run);
  PrintCategoryAttachmentProtocolRuntimeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
