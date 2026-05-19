#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/objc3_sema_contract_type_handoff.h"
#include "token/objc3_sema_token_metadata.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &pointer_declarator_tokens,
    const std::vector<Objc3SemaTokenMetadata> &nullability_suffix_tokens,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary);

void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary);

}  // namespace detail

Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff);

Objc3FrontendClassProtocolCategoryLinkingSummary
BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff);

Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(
    const Objc3Program &program);

Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(
    const Objc3Program &program);

Objc3FrontendObjectPointerNullabilityGenericsSummary
BuildObjectPointerNullabilityGenericsSummary(const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
