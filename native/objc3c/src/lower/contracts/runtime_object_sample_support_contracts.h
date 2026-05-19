#pragma once

#include <string>

// Runtime object sample support owns the canonical runnable object evidence
// boundary for alloc/new/init and realized dispatch samples while broader
// metadata-heavy object behavior remains runtime-export gated.
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId =
    "objc3c.runtime.canonical.runnable.object.sample.support.v1";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectExecutionModel =
    "canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel =
    "metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectFailClosedModel =
    "metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open";

std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary();
