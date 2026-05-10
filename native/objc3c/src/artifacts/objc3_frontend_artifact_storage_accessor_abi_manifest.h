#pragma once

#include <iosfwd>

struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeLinkHostLinkContract;

namespace objc3::artifacts::frontend {

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract);

}  // namespace objc3::artifacts::frontend
