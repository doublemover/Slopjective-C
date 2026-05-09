#pragma once

#include <string>

#include "sema/objc3_parser_sema_handoff_contract.h"
#include "sema/objc3_parser_sema_handoff_scaffold.h"
#include "sema/objc3_sema_pass_manager_contract.h"

struct Objc3SemaParserHandoffPublication {
  Objc3ParserSemaHandoffOwnerRecord owner_record;
  bool owner_record_deterministic = false;
  bool ready = false;
  bool parser_recovery_replay_ready = false;
  bool parser_recovery_replay_case_present = false;
  bool parser_recovery_replay_case_passed = false;
  bool parser_recovery_replay_contract_satisfied = false;
  std::string recovery_replay_key;
  bool recovery_replay_key_deterministic = false;
  bool recovery_determinism_hardening_satisfied = false;
};

Objc3SemaParserHandoffPublication PublishObjc3ParserSemaHandoff(
    const Objc3ParserSemaHandoffScaffold &handoff,
    Objc3SemaPassManagerResult &result);
