#pragma once

#include <iosfwd>

struct Objc3DispatchSurfaceClassificationContract;
struct Objc3MessageSendSelectorLoweringContract;
struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeLinkHostLinkContract;

namespace objc3::artifacts::frontend {

void WriteDispatchAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract);

}  // namespace objc3::artifacts::frontend
