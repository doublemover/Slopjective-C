#include "parse/objc3_parser_core_objc_actor_interface_declarations_diagnostics.inc"

#include "parse/objc3_parser_core_objc_actor_interface_declarations_header.inc"

#include "parse/objc3_parser_core_objc_actor_interface_declarations_protocol_super.inc"

#include "parse/objc3_parser_core_objc_actor_interface_declarations_members.inc"

#include "parse/objc3_parser_core_objc_actor_interface_declarations_finalization.inc"

  std::unique_ptr<Objc3InterfaceDecl> ParseObjcActorInterfaceDecl(
      const Token &actor_token,
      const Objc3InterfaceDecl *prefixed_dispatch_attributes = nullptr) {
    auto decl = ParseObjcActorInterfaceDeclHeader(actor_token);
    if (decl == nullptr) {
      return nullptr;
    }

    if (!ParseObjcActorInterfaceDeclProtocolSuperAndDispatchClauses(
            *decl, prefixed_dispatch_attributes)) {
      return nullptr;
    }

    FinalizeObjcActorInterfaceDeclScopeAndSemanticLinks(*decl);
    ParseObjcActorInterfaceDeclMembers(*decl);

    if (!FinalizeObjcActorInterfaceDecl(*decl)) {
      return nullptr;
    }

    return decl;
  }
