#include "parse/objc3_parser_core_objc_interface_declarations_diagnostics.inc"

#include "parse/objc3_parser_core_objc_interface_declarations_header.inc"

#include "parse/objc3_parser_core_objc_interface_declarations_protocol_super.inc"

#include "parse/objc3_parser_core_objc_interface_declarations_members.inc"

#include "parse/objc3_parser_core_objc_interface_declarations_finalization.inc"

  std::unique_ptr<Objc3InterfaceDecl> ParseObjcInterfaceDecl(
      const Objc3InterfaceDecl *prefixed_dispatch_attributes = nullptr) {
    auto decl = ParseObjcInterfaceDeclHeader();
    if (decl == nullptr) {
      return nullptr;
    }

    ParseObjcInterfaceDeclProtocolAndSuperClauses(
        *decl, prefixed_dispatch_attributes);
    FinalizeObjcInterfaceDeclScopeAndSemanticLinks(*decl);
    ParseObjcInterfaceDeclMembers(*decl);

    if (!FinalizeObjcInterfaceDecl(*decl)) {
      return nullptr;
    }
    return decl;
  }
