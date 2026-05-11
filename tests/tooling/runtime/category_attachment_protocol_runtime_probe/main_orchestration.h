#pragma once

#include "attachment_actions.h"
#include "protocol_runtime_assertions.h"
#include "report_helpers.h"
#include "runtime_fixture_setup.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline CategoryAttachmentProtocolProbeRun
CaptureCategoryAttachmentProtocolRuntimeProbe() {
  CategoryAttachmentProtocolProbeRun run;
  CaptureCategoryAttachmentActions(run);
  CaptureProtocolRuntimeAssertions(run);
  CaptureCategoryAttachmentProtocolRuntimeSnapshots(run);
  return run;
}

inline int RunCategoryAttachmentProtocolRuntimeProbe() {
  const CategoryAttachmentProtocolProbeRun run =
      CaptureCategoryAttachmentProtocolRuntimeProbe();
  PrintCategoryAttachmentProtocolRuntimeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
