#pragma once

#include <cstddef>
#include <iosfwd>

struct Objc3IRMethodDefinition;

struct Objc3IRSyntheticMethodEmissionStats {
  std::size_t getter_definition_count = 0;
  std::size_t setter_definition_count = 0;
  std::size_t current_property_read_helper_call_count = 0;
  std::size_t current_property_write_helper_call_count = 0;
  std::size_t current_property_exchange_helper_call_count = 0;
  std::size_t weak_current_property_load_helper_call_count = 0;
  std::size_t weak_current_property_store_helper_call_count = 0;
  std::size_t retain_helper_call_count = 0;
  std::size_t release_helper_call_count = 0;
  std::size_t autorelease_helper_call_count = 0;
};

void EmitObjc3IRSyntheticMethod(
    const Objc3IRMethodDefinition &method_def, std::ostringstream &out,
    Objc3IRSyntheticMethodEmissionStats &stats);
