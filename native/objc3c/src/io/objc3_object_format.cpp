#include "io/objc3_object_format.h"

#include <array>
#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "lower/objc3_lowering_contract.h"

namespace {

bool IsRecognizedCoffMachine(std::uint16_t machine) {
  switch (machine) {
    case 0x014c:  // IMAGE_FILE_MACHINE_I386
    case 0x8664:  // IMAGE_FILE_MACHINE_AMD64
    case 0x01c0:  // IMAGE_FILE_MACHINE_ARM
    case 0xaa64:  // IMAGE_FILE_MACHINE_ARM64
      return true;
    default:
      return false;
  }
}

bool IsMachOMagic(std::uint32_t magic) {
  switch (magic) {
    case 0xfeedfaceu:
    case 0xcefaedfeu:
    case 0xfeedfacfu:
    case 0xcffaedfeu:
    case 0xcafebabeu:
    case 0xbebafecau:
      return true;
    default:
      return false;
  }
}

std::uint16_t ReadU16(const std::vector<unsigned char> &bytes,
                      std::size_t offset) {
  return static_cast<std::uint16_t>(bytes[offset]) |
         (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u);
}

std::uint32_t ReadU32(const std::vector<unsigned char> &bytes,
                      std::size_t offset) {
  return static_cast<std::uint32_t>(bytes[offset]) |
         (static_cast<std::uint32_t>(bytes[offset + 1]) << 8u) |
         (static_cast<std::uint32_t>(bytes[offset + 2]) << 16u) |
         (static_cast<std::uint32_t>(bytes[offset + 3]) << 24u);
}

bool CoffSectionNameEquals(const std::vector<unsigned char> &bytes,
                           std::size_t section_header_offset,
                           std::string_view expected) {
  if (expected.size() > 8) {
    return false;
  }
  for (std::size_t index = 0; index < 8; ++index) {
    const unsigned char actual = bytes[section_header_offset + index];
    const unsigned char wanted =
        index < expected.size() ? static_cast<unsigned char>(expected[index])
                                : 0u;
    if (actual != wanted) {
      return false;
    }
  }
  return true;
}

bool LooksLikeObjectPath(std::string_view value) {
  if (value.find(".obj") == std::string_view::npos) {
    return false;
  }
  return value.find(':') != std::string_view::npos ||
         value.find('\\') != std::string_view::npos ||
         value.find('/') != std::string_view::npos;
}

bool NormalizeCoffDebugSObjectPaths(std::vector<unsigned char> &bytes) {
  if (bytes.size() < 20) {
    return false;
  }

  const std::uint16_t machine = ReadU16(bytes, 0);
  if (!IsRecognizedCoffMachine(machine)) {
    return false;
  }

  const std::uint16_t section_count = ReadU16(bytes, 2);
  const std::uint32_t symbol_table_offset = ReadU32(bytes, 8);
  const std::uint32_t symbol_count = ReadU32(bytes, 12);
  const std::uint16_t optional_header_size = ReadU16(bytes, 16);
  const std::size_t section_table_offset =
      20u + static_cast<std::size_t>(optional_header_size);
  if (section_count == 0 ||
      section_table_offset + static_cast<std::size_t>(section_count) * 40u >
          bytes.size()) {
    return false;
  }

  std::uint16_t debug_s_section_number = 0;
  std::size_t debug_s_raw_offset = 0;
  std::size_t debug_s_raw_size = 0;
  for (std::uint16_t index = 0; index < section_count; ++index) {
    const std::size_t header_offset =
        section_table_offset + static_cast<std::size_t>(index) * 40u;
    if (!CoffSectionNameEquals(bytes, header_offset, ".debug$S")) {
      continue;
    }
    debug_s_raw_size = ReadU32(bytes, header_offset + 16u);
    debug_s_raw_offset = ReadU32(bytes, header_offset + 20u);
    if (debug_s_raw_size == 0 ||
        debug_s_raw_offset + debug_s_raw_size > bytes.size()) {
      return false;
    }
    debug_s_section_number = static_cast<std::uint16_t>(index + 1u);
    break;
  }
  if (debug_s_section_number == 0) {
    return false;
  }

  static constexpr std::string_view kCanonicalObjectPath =
      "objc3c-deterministic-object.obj";
  bool changed = false;
  const std::size_t raw_end = debug_s_raw_offset + debug_s_raw_size;
  for (std::size_t cursor = debug_s_raw_offset; cursor < raw_end;) {
    std::size_t end = cursor;
    while (end < raw_end && bytes[end] != 0u) {
      ++end;
    }
    if (end > cursor) {
      const std::string_view field(
          reinterpret_cast<const char *>(bytes.data() + cursor), end - cursor);
      if (LooksLikeObjectPath(field) &&
          field.size() >= kCanonicalObjectPath.size()) {
        for (std::size_t index = 0; index < field.size(); ++index) {
          bytes[cursor + index] =
              index < kCanonicalObjectPath.size()
                  ? static_cast<unsigned char>(kCanonicalObjectPath[index])
                  : 0u;
        }
        changed = true;
      }
    }
    cursor = end + 1u;
  }

  if (!changed || symbol_table_offset == 0 || symbol_count == 0) {
    return changed;
  }
  const std::size_t symbol_table_start = symbol_table_offset;
  if (symbol_table_start >= bytes.size()) {
    return changed;
  }

  for (std::uint32_t symbol_index = 0; symbol_index < symbol_count;) {
    const std::size_t symbol_offset =
        symbol_table_start + static_cast<std::size_t>(symbol_index) * 18u;
    if (symbol_offset + 18u > bytes.size()) {
      return changed;
    }
    const std::uint16_t section_number = ReadU16(bytes, symbol_offset + 12u);
    const unsigned char aux_count = bytes[symbol_offset + 17u];
    if (section_number == debug_s_section_number) {
      for (unsigned char aux_index = 0; aux_index < aux_count; ++aux_index) {
        const std::size_t aux_offset =
            symbol_offset + static_cast<std::size_t>(aux_index + 1u) * 18u;
        if (aux_offset + 12u <= bytes.size()) {
          bytes[aux_offset + 8u] = 0u;
          bytes[aux_offset + 9u] = 0u;
          bytes[aux_offset + 10u] = 0u;
          bytes[aux_offset + 11u] = 0u;
        }
      }
    }
    symbol_index += static_cast<std::uint32_t>(aux_count) + 1u;
  }

  return true;
}

void NormalizeCoffDebugSObjectPathRecords(
    const std::filesystem::path &object_out) {
  std::ifstream input(object_out, std::ios::binary);
  if (!input.is_open()) {
    return;
  }
  std::vector<unsigned char> bytes(
      (std::istreambuf_iterator<char>(input)),
      std::istreambuf_iterator<char>());
  input.close();

  if (!NormalizeCoffDebugSObjectPaths(bytes)) {
    return;
  }

  std::ofstream output(object_out,
                       std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output.is_open()) {
    return;
  }
  output.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
}

void NormalizeCoffTimestamp(const std::filesystem::path &object_out) {
  std::error_code file_size_error;
  const std::uintmax_t size = std::filesystem::file_size(object_out, file_size_error);
  if (file_size_error || size < 8) {
    return;
  }

  std::fstream file(object_out, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    return;
  }

  std::array<unsigned char, 8> header{};
  file.read(reinterpret_cast<char *>(header.data()), static_cast<std::streamsize>(header.size()));
  if (!file.good() && !file.eof()) {
    return;
  }
  if (file.gcount() != static_cast<std::streamsize>(header.size())) {
    return;
  }

  const std::uint16_t machine =
      static_cast<std::uint16_t>(header[0]) |
      static_cast<std::uint16_t>(static_cast<std::uint16_t>(header[1]) << 8u);
  if (!IsRecognizedCoffMachine(machine)) {
    return;
  }

  const char zero_timestamp[4] = {0, 0, 0, 0};
  file.seekp(4, std::ios::beg);
  if (!file.good()) {
    return;
  }
  file.write(zero_timestamp, 4);
}

}  // namespace

ProducedObjectFormat DetectProducedObjectFormat(
    const std::filesystem::path &object_out) {
  std::error_code file_size_error;
  const std::uintmax_t size =
      std::filesystem::file_size(object_out, file_size_error);
  if (file_size_error || size < 4) {
    return ProducedObjectFormat::kUnknown;
  }

  std::ifstream file(object_out, std::ios::binary);
  if (!file.is_open()) {
    return ProducedObjectFormat::kUnknown;
  }

  std::array<unsigned char, 8> header{};
  file.read(reinterpret_cast<char *>(header.data()),
            static_cast<std::streamsize>(header.size()));
  if (!file.good() && !file.eof()) {
    return ProducedObjectFormat::kUnknown;
  }
  if (file.gcount() < 4) {
    return ProducedObjectFormat::kUnknown;
  }

  if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' &&
      header[3] == 'F') {
    return ProducedObjectFormat::kElf;
  }

  const std::uint32_t magic =
      static_cast<std::uint32_t>(header[0]) |
      (static_cast<std::uint32_t>(header[1]) << 8u) |
      (static_cast<std::uint32_t>(header[2]) << 16u) |
      (static_cast<std::uint32_t>(header[3]) << 24u);
  if (IsMachOMagic(magic)) {
    return ProducedObjectFormat::kMachO;
  }

  if (file.gcount() >= 2) {
    const std::uint16_t machine =
        static_cast<std::uint16_t>(header[0]) |
        (static_cast<std::uint16_t>(header[1]) << 8u);
    if (IsRecognizedCoffMachine(machine)) {
      return ProducedObjectFormat::kCoff;
    }
  }

  return ProducedObjectFormat::kUnknown;
}

std::string ProducedObjectFormatName(ProducedObjectFormat format) {
  switch (format) {
    case ProducedObjectFormat::kCoff:
      return kObjc3RuntimeMetadataObjectFormatCoff;
    case ProducedObjectFormat::kElf:
      return kObjc3RuntimeMetadataObjectFormatElf;
    case ProducedObjectFormat::kMachO:
      return kObjc3RuntimeMetadataObjectFormatMachO;
    case ProducedObjectFormat::kUnknown:
    default:
      return "";
  }
}

void NormalizeObjectDeterminism(const std::filesystem::path &object_out) {
  switch (DetectProducedObjectFormat(object_out)) {
    case ProducedObjectFormat::kCoff:
      NormalizeCoffTimestamp(object_out);
      NormalizeCoffDebugSObjectPathRecords(object_out);
      return;
    case ProducedObjectFormat::kElf:
    case ProducedObjectFormat::kMachO:
    case ProducedObjectFormat::kUnknown:
    default:
      return;
  }
}
