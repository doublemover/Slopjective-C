#include "parse/objc3_parser_core_objc_implementation_declarations_diagnostics_recovery.inc"

#include "parse/objc3_parser_core_objc_implementation_declarations_header.inc"

#include "parse/objc3_parser_core_objc_implementation_declarations_finalization_publication.inc"

#include "parse/objc3_parser_core_objc_implementation_declarations_property_members.inc"

#include "parse/objc3_parser_core_objc_implementation_declarations_method_members.inc"

#include "parse/objc3_parser_core_objc_implementation_declarations_members.inc"

  std::unique_ptr<Objc3ImplementationDecl> ParseObjcImplementationDecl() {
    auto decl = ParseObjcImplementationDeclHeader();
    if (decl == nullptr) {
      return nullptr;
    }

    FinalizeObjcImplementationDeclScopeAndSemanticLinks(*decl);
    ParseObjcImplementationDeclMembers(*decl);

    if (!FinalizeObjcImplementationDecl(*decl)) {
      return nullptr;
    }
    return decl;
  }
