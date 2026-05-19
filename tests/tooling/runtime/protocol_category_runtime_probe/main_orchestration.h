#pragma once

#include "attachment_realization_actions.h"
#include "report_helpers.h"
#include "runtime_fixture_setup.h"

namespace objc3c::runtime::probe::protocol_category_runtime {

inline int RunProtocolCategoryRuntimeProbe() {
  ProtocolCategoryProbeRun run;
  CaptureRuntimeRegistryFixture(run.registry);
  CaptureProtocolCategoryAttachmentActions(run);
  PrintProtocolCategoryRuntimeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::protocol_category_runtime
