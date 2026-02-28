#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_parser_contract.h"

struct Objc3IRFrontendMetadata {
  std::uint8_t language_version = 3u;
  std::string compatibility_mode = "canonical";
  bool migration_assist = false;
  std::size_t migration_legacy_yes = 0;
  std::size_t migration_legacy_no = 0;
  std::size_t migration_legacy_null = 0;

  std::size_t migration_legacy_total() const { return migration_legacy_yes + migration_legacy_no + migration_legacy_null; }
};

bool EmitObjc3IRText(const Objc3ParsedProgram &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error);
