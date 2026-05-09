#pragma once

#include <string>

#include "sema/objc3_parser_sema_handoff_contract.h"
#include "sema/objc3_parser_sema_handoff_scaffold.h"
#include "sema/objc3_sema_pass_manager_contract.h"

struct Objc3SemaParserHandoffPublication {
  Objc3ParserSemaHandoffOwnerRecord owner_record;
  bool owner_record_deterministic = false;
  Objc3ParserSemaHandoffPublicationEvidenceRecord evidence_record;
  bool evidence_record_deterministic = false;
  bool ready = false;
};

Objc3SemaParserHandoffPublication PublishObjc3ParserSemaHandoff(
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result);
