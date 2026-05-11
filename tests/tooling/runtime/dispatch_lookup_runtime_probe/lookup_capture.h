#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_LOOKUP_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_LOOKUP_CAPTURE_H_

#include "probe_state.h"
#include "runtime_assertion_helpers.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline SelectorLookupCapture CaptureSelectorLookups() {
  const objc3_runtime_selector_handle *null_selector =
      objc3_runtime_lookup_selector(nullptr);
  const objc3_runtime_selector_handle *copy_first =
      objc3_runtime_lookup_selector(kCopySelector);
  const objc3_runtime_selector_handle *copy_second =
      objc3_runtime_lookup_selector(kCopySelector);
  const objc3_runtime_selector_handle *gamma =
      objc3_runtime_lookup_selector(kGammaSelector);

  SelectorLookupCapture capture;
  capture.lookup_null_is_null = null_selector == nullptr;
  capture.copy_selector_reused =
      copy_first != nullptr && copy_first == copy_second;
  capture.copy_selector_spelling_matches =
      SelectorSpellingMatches(copy_first, kCopySelector);
  capture.copy_selector_stable_id = SelectorStableId(copy_first);
  capture.gamma_selector_stable_id = SelectorStableId(gamma);
  return capture;
}

inline ResetLookupCapture CaptureSelectorLookupAfterReset() {
  const objc3_runtime_selector_handle *copy_after_reset =
      objc3_runtime_lookup_selector(kCopySelector);

  ResetLookupCapture capture;
  capture.copy_after_reset_stable_id = SelectorStableId(copy_after_reset);
  return capture;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_LOOKUP_CAPTURE_H_
