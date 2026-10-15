#include "io/objc3_object_format.h"

#include <algorithm>
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

void WriteU16(std::vector<unsigned char> &bytes, std::size_t offset,
              std::uint16_t value) {
  bytes[offset] = static_cast<unsigned char>(value);
  bytes[offset + 1] = static_cast<unsigned char>(value >> 8u);
}

void WriteU32(std::vector<unsigned char> &bytes, std::size_t offset,
              std::uint32_t value) {
  for (unsigned index = 0; index != 4; ++index) {
    bytes[offset + index] = static_cast<unsigned char>(value >> (8u * index));
  }
}

bool NormalizeCoffDebugSObjectPaths(std::vector<unsigned char> &bytes) {
  // Rewrite the actual C13 S_OBJNAME record, including its lengths. Padding
  // a replacement path to the old path's size leaves output-directory lengths
  // observable in the object. Do not scan arbitrary debug bytes as C strings.
  if (bytes.size() < 20 || bytes.size() > 0xffffffffu ||
      !IsRecognizedCoffMachine(ReadU16(bytes, 0)) ||
      ReadU16(bytes, 16) != 0) {
    return false;
  }
  const std::size_t section_count = ReadU16(bytes, 2);
  const std::size_t section_table_end = 20 + section_count * 40;
  const std::size_t symbols = ReadU32(bytes, 8);
  const std::size_t symbol_count = ReadU32(bytes, 12);
  if (!section_count || section_table_end > bytes.size() ||
      (symbol_count && (symbols < section_table_end || symbols > bytes.size() ||
                        symbol_count > (bytes.size() - symbols) / 18))) {
    return false;
  }
  std::size_t section_index = 0;
  for (; section_index < section_count; ++section_index) {
    if (CoffSectionNameEquals(bytes, 20 + section_index * 40, ".debug$S")) {
      break;
    }
  }
  if (section_index == section_count) {
    return false;
  }
  const std::size_t header = 20 + section_index * 40;
  const std::size_t raw = ReadU32(bytes, header + 20);
  const std::size_t size = ReadU32(bytes, header + 16);
  // This rewrite owns the compiler-information subsection, which has no
  // relocations or line records. Leave other layouts untouched.
  if (raw < section_table_end || raw > bytes.size() || size > bytes.size() - raw ||
      size < 20 || ReadU16(bytes, header + 32) || ReadU16(bytes, header + 34) ||
      ReadU32(bytes, raw) != 4 || ReadU32(bytes, raw + 4) != 0xf1) {
    return false;
  }
  const std::size_t subsection_size = ReadU32(bytes, raw + 8);
  const std::size_t record = raw + 12;
  const std::size_t record_size = ReadU16(bytes, record) + 2;
  if (subsection_size > size - 12 || record_size < 12 ||
      record_size > subsection_size || record_size % 4 ||
      ReadU16(bytes, record + 2) != 0x1101) {
    return false;
  }
  std::size_t name_end = record + 8;
  while (name_end < record + record_size && bytes[name_end]) {
    ++name_end;
  }
  if (name_end == record + record_size) {
    return false;
  }
  constexpr std::string_view canonical = "objc3c-deterministic-object.obj";
  const std::size_t canonical_size = (8 + canonical.size() + 1 + 3) & ~std::size_t(3);
  const std::int64_t delta = static_cast<std::int64_t>(canonical_size) -
                             static_cast<std::int64_t>(record_size);
  const std::size_t removed_end = record + record_size;
  auto shifted = [&](std::uint32_t offset) -> std::uint32_t {
    return offset >= removed_end ? static_cast<std::uint32_t>(offset + delta) : offset;
  };
  auto valid_offset = [&](std::uint32_t offset) {
    return !offset || (offset <= bytes.size() &&
        (offset < record || offset >= removed_end) &&
        (offset < removed_end ||
         static_cast<std::uint64_t>(offset + delta) <= 0xffffffffu));
  };
  if (!valid_offset(static_cast<std::uint32_t>(symbols)) ||
      static_cast<std::int64_t>(size) + delta < 0 ||
      static_cast<std::int64_t>(size) + delta > 0xffffffffu ||
      static_cast<std::int64_t>(subsection_size) + delta < 0 ||
      static_cast<std::int64_t>(subsection_size) + delta > 0xffffffffu ||
      static_cast<std::int64_t>(bytes.size()) + delta > 0xffffffffu) {
    return false;
  }
  for (std::size_t index = 0; index < section_count; ++index) {
    const std::size_t section = 20 + index * 40;
    for (std::size_t field : {20u, 24u, 28u}) {
      if (!valid_offset(ReadU32(bytes, section + field))) {
        return false;
      }
    }
  }
  // Validate symbol and auxiliary records before changing any bytes. Native
  // compiler-info sections have only a section symbol at value zero.
  for (std::size_t index = 0; index < symbol_count;) {
    const std::size_t symbol = symbols + index * 18;
    const std::size_t auxiliaries = bytes[symbol + 17];
    if (auxiliaries >= symbol_count - index ||
        (ReadU16(bytes, symbol + 12) == section_index + 1 &&
         (ReadU32(bytes, symbol + 8) != 0 || bytes[symbol + 16] != 3 || auxiliaries != 1))) {
      return false;
    }
    index += auxiliaries + 1;
  }
  std::vector<unsigned char> rewritten(bytes.begin(), bytes.begin() + record);
  rewritten.resize(record + canonical_size, 0);
  WriteU16(rewritten, record, static_cast<std::uint16_t>(canonical_size - 2));
  WriteU16(rewritten, record + 2, 0x1101);
  WriteU32(rewritten, record + 4, ReadU32(bytes, record + 4));
  std::copy(canonical.begin(), canonical.end(), rewritten.begin() + record + 8);
  rewritten.insert(rewritten.end(), bytes.begin() + removed_end, bytes.end());
  WriteU32(rewritten, raw + 8, static_cast<std::uint32_t>(subsection_size + delta));
  WriteU32(rewritten, header + 16, static_cast<std::uint32_t>(size + delta));
  WriteU32(rewritten, 8, shifted(static_cast<std::uint32_t>(symbols)));
  for (std::size_t index = 0; index < section_count; ++index) {
    const std::size_t section = 20 + index * 40;
    for (std::size_t field : {20u, 24u, 28u}) {
      WriteU32(rewritten, section + field, shifted(ReadU32(bytes, section + field)));
    }
  }
  for (std::size_t index = 0; index < symbol_count;) {
    const std::size_t symbol = symbols + index * 18;
    const std::size_t auxiliaries = bytes[symbol + 17];
    if (ReadU16(bytes, symbol + 12) == section_index + 1) {
      const std::size_t auxiliary = shifted(static_cast<std::uint32_t>(symbols)) + (index + 1) * 18;
      WriteU32(rewritten, auxiliary, static_cast<std::uint32_t>(size + delta));
      WriteU32(rewritten, auxiliary + 8, 0); // Checksum of rewritten debug data.
    }
    index += auxiliaries + 1;
  }
  if (rewritten == bytes) {
    return false;
  }
  bytes.swap(rewritten);
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
