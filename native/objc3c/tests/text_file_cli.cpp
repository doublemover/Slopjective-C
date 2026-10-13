#include "runtime/stdlib/stdlib_service_contract.h"

#include <cerrno>
#include <charconv>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <new>
#include <string>
#include <system_error>
#include <vector>

namespace {
constexpr std::uint64_t kMaxInputBytes = 1U << 20;

enum class AppStatus : int { Usage = 64, InvalidRecord = 65, IoError = 66,
                             Overflow = 70, OutputError = 74 };

struct TextOwner {
  objc3_text_v2_t value = nullptr;
  ~TextOwner() {
    if (value) (void)objc3_runtime_stdlib_text_v2_release_checked(value);
  }
  TextOwner() = default;
  explicit TextOwner(objc3_text_v2_t text) : value(text) {}
  TextOwner(const TextOwner &) = delete;
  TextOwner &operator=(const TextOwner &) = delete;
};

struct BuilderOwner {
  objc3_text_builder_v2_t value = nullptr;
  ~BuilderOwner() {
    if (value) (void)objc3_runtime_stdlib_text_builder_v2_release(value);
  }
  BuilderOwner() = default;
  BuilderOwner(const BuilderOwner &) = delete;
  BuilderOwner &operator=(const BuilderOwner &) = delete;
};

struct Failure {
  AppStatus app = AppStatus::InvalidRecord;
  int service = OBJC3_STDLIB_V2_OK;
  int system = 0;
  const char *kind = "invalid_record";
};

int report(const Failure &failure) {
  if (failure.service != OBJC3_STDLIB_V2_OK)
    std::fprintf(stderr, "error kind=%s service_status=%d errno=%d\n",
                 failure.kind, failure.service, failure.system);
  else
    std::fprintf(stderr, "error kind=%s app_status=%d errno=%d\n",
                 failure.kind, static_cast<int>(failure.app), failure.system);
  return failure.service != OBJC3_STDLIB_V2_OK
             ? failure.service : static_cast<int>(failure.app);
}

bool read_file(const char *path, std::vector<char> &bytes, Failure &failure) {
  std::FILE *file = nullptr;
#if defined(_WIN32)
  const int open_status = fopen_s(&file, path, "rb");
#else
  file = std::fopen(path, "rb");
  const int open_status = file ? 0 : errno;
#endif
  if (!file || open_status != 0) {
    failure = {AppStatus::IoError, OBJC3_STDLIB_V2_OK,
               open_status ? open_status : errno, "io"};
    return false;
  }
  // Allocation failures during input growth must also close the file.
  std::unique_ptr<std::FILE, decltype(&std::fclose)> owner(file, &std::fclose);
  char chunk[4096];
  while (true) {
    const std::size_t count = std::fread(chunk, 1, sizeof(chunk), file);
    if (count != 0) {
      if (count > kMaxInputBytes - bytes.size()) {
        failure = {AppStatus::Overflow, OBJC3_STDLIB_V2_CAPACITY_EXCEEDED, 0,
                   "input_too_large"};
        return false;
      }
      bytes.insert(bytes.end(), chunk, chunk + count);
    }
    if (count < sizeof(chunk)) {
      if (std::ferror(file)) {
        const int error = errno;
        failure = {AppStatus::IoError, OBJC3_STDLIB_V2_OK, error, "io"};
        return false;
      }
      break;
    }
  }
  if (std::fclose(owner.release()) != 0) {
    failure = {AppStatus::IoError, OBJC3_STDLIB_V2_OK, errno, "io"};
    return false;
  }
  return true;
}

int status_failure(int status, const char *kind) {
  return report({AppStatus::InvalidRecord, status, 0, kind});
}

int append_ascii(objc3_text_builder_v2_t builder, const char *text,
                 std::uint64_t length) {
  return objc3_runtime_stdlib_text_builder_v2_append_utf8(
      builder, text, length);
}

int append_number(objc3_text_builder_v2_t builder, std::int64_t number) {
  char buffer[32];
  const auto result = std::to_chars(buffer, buffer + sizeof(buffer), number);
  if (result.ec != std::errc{})
    return OBJC3_STDLIB_V2_OVERFLOW;
  return append_ascii(builder, buffer,
                      static_cast<std::uint64_t>(result.ptr - buffer));
}

bool parse_value(objc3_text_v2_t line, std::uint64_t scalar_count,
                 std::int64_t &out_value, Failure &failure) {
  const char colon[] = {':' };
  int found = 0;
  std::uint64_t colon_index = 0;
  int status = objc3_runtime_stdlib_text_v2_find_scalars(
      line, colon, sizeof(colon), 0, &found, &colon_index);
  if (status != OBJC3_STDLIB_V2_OK) {
    failure = {AppStatus::InvalidRecord, status, 0, "text_service"};
    return false;
  }
  if (!found || colon_index == 0 || colon_index + 1 >= scalar_count) {
    failure = {AppStatus::InvalidRecord, OBJC3_STDLIB_V2_OK, 0,
               "invalid_record"};
    return false;
  }
  std::uint64_t second_colon = 0;
  status = objc3_runtime_stdlib_text_v2_find_scalars(
      line, colon, sizeof(colon), colon_index + 1, &found, &second_colon);
  if (status != OBJC3_STDLIB_V2_OK || found) {
    failure = {AppStatus::InvalidRecord, status, 0, "invalid_record"};
    return false;
  }

  TextOwner number;
  status = objc3_runtime_stdlib_text_v2_slice_scalars(
      line, colon_index + 1, scalar_count - colon_index - 1, &number.value);
  if (status != OBJC3_STDLIB_V2_OK) {
    failure = {AppStatus::InvalidRecord, status, 0, "text_service"};
    return false;
  }
  std::uint64_t byte_count = 0;
  status = objc3_runtime_stdlib_text_v2_byte_count_checked(number.value,
                                                           &byte_count);
  if (status != OBJC3_STDLIB_V2_OK) {
    failure = {AppStatus::InvalidRecord, status, 0, "text_service"};
    return false;
  }
  std::string digits;
  digits.resize(static_cast<std::size_t>(byte_count));
  for (std::uint64_t index = 0; index < byte_count; ++index) {
    std::uint8_t byte = 0;
    status = objc3_runtime_stdlib_text_v2_byte_at_checked(number.value, index,
                                                          &byte);
    if (status != OBJC3_STDLIB_V2_OK) {
      failure = {AppStatus::InvalidRecord, status, 0, "text_service"};
      return false;
    }
    digits[static_cast<std::size_t>(index)] = static_cast<char>(byte);
  }
  if (!digits.empty() && digits.back() == '\r')
    digits.pop_back();
  if (digits.empty()) {
    failure = {AppStatus::InvalidRecord, OBJC3_STDLIB_V2_OK, 0,
               "invalid_integer"};
    return false;
  }
  const auto parsed = std::from_chars(digits.data(),
                                      digits.data() + digits.size(), out_value);
  if (parsed.ec != std::errc{} || parsed.ptr != digits.data() + digits.size()) {
    failure = {AppStatus::InvalidRecord, OBJC3_STDLIB_V2_OK, 0,
               "invalid_integer"};
    return false;
  }
  return true;
}

bool write_text(objc3_text_v2_t text, Failure &failure) {
  std::uint64_t byte_count = 0;
  const int status = objc3_runtime_stdlib_text_v2_byte_count_checked(
      text, &byte_count);
  if (status != OBJC3_STDLIB_V2_OK) {
    failure = {AppStatus::OutputError, status, 0, "text_service"};
    return false;
  }
  for (std::uint64_t index = 0; index < byte_count; ++index) {
    std::uint8_t byte = 0;
    const int byte_status = objc3_runtime_stdlib_text_v2_byte_at_checked(
        text, index, &byte);
    if (byte_status != OBJC3_STDLIB_V2_OK || std::fputc(byte, stdout) == EOF) {
      failure = {AppStatus::OutputError, byte_status, errno, "output_io"};
      return false;
    }
  }
  if (std::fflush(stdout) != 0) {
    failure = {AppStatus::OutputError, OBJC3_STDLIB_V2_OK, errno, "output_io"};
    return false;
  }
  return true;
}

int run(const char *path) {
  Failure failure;
  std::vector<char> input;
  if (!read_file(path, input, failure))
    return report(failure);

  TextOwner text;
  int status = objc3_runtime_stdlib_text_v2_create_utf8_checked(
      input.data(), input.size(), &text.value);
  if (status != OBJC3_STDLIB_V2_OK)
    return status_failure(status, status == OBJC3_STDLIB_V2_INVALID_UTF8
                                      ? "invalid_utf8" : "text_service");

  std::uint64_t scalar_count = 0;
  status = objc3_runtime_stdlib_text_v2_scalar_count_checked(text.value,
                                                              &scalar_count);
  if (status != OBJC3_STDLIB_V2_OK)
    return status_failure(status, "text_service");

  std::uint64_t cursor = 0;
  std::uint64_t records = 0;
  std::int64_t sum = 0;
  const char newline[] = {'\n'};
  while (cursor < scalar_count) {
    int found = 0;
    std::uint64_t line_end = 0;
    status = objc3_runtime_stdlib_text_v2_find_scalars(
        text.value, newline, sizeof(newline), cursor, &found, &line_end);
    if (status != OBJC3_STDLIB_V2_OK)
      return status_failure(status, "text_service");
    if (!found)
      line_end = scalar_count;
    if (line_end == cursor) {
      return report({AppStatus::InvalidRecord, OBJC3_STDLIB_V2_OK, 0,
                     "empty_record"});
    }

    TextOwner line;
    status = objc3_runtime_stdlib_text_v2_slice_scalars(
        text.value, cursor, line_end - cursor, &line.value);
    if (status != OBJC3_STDLIB_V2_OK)
      return status_failure(status, "text_service");
    const std::uint64_t line_scalar_count = line_end - cursor;
    std::int64_t value = 0;
    if (!parse_value(line.value, line_scalar_count, value, failure))
      return report(failure);
    if ((value > 0 && sum > std::numeric_limits<std::int64_t>::max() - value) ||
        (value < 0 && sum < std::numeric_limits<std::int64_t>::min() - value))
      return report({AppStatus::Overflow, OBJC3_STDLIB_V2_OVERFLOW, 0,
                     "aggregate_overflow"});
    if (records == std::numeric_limits<std::uint64_t>::max())
      return report({AppStatus::Overflow, OBJC3_STDLIB_V2_OVERFLOW, 0,
                     "record_count_overflow"});
    sum += value;
    ++records;
    cursor = line_end + (found ? 1 : 0);
  }

  BuilderOwner output;
  status = objc3_runtime_stdlib_text_builder_v2_create(&output.value);
  if (status != OBJC3_STDLIB_V2_OK)
    return status_failure(status, "text_service");
  status = append_ascii(output.value, "records=", 8);
  if (status == OBJC3_STDLIB_V2_OK)
    status = append_number(output.value, static_cast<std::int64_t>(records));
  if (status == OBJC3_STDLIB_V2_OK)
    status = append_ascii(output.value, " sum=", 5);
  if (status == OBJC3_STDLIB_V2_OK)
    status = append_number(output.value, sum);
  if (status == OBJC3_STDLIB_V2_OK)
    status = objc3_runtime_stdlib_text_builder_v2_append_scalar(output.value,
                                                                 '\n');
  if (status != OBJC3_STDLIB_V2_OK)
    return status_failure(status, "text_builder");
  TextOwner rendered;
  status = objc3_runtime_stdlib_text_builder_v2_finish(output.value,
                                                        &rendered.value);
  if (status != OBJC3_STDLIB_V2_OK)
    return status_failure(status, "text_service");
  if (!write_text(rendered.value, failure))
    return report(failure);
  return 0;
}
}  // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    std::fprintf(stderr, "error kind=usage app_status=%d\n",
                 static_cast<int>(AppStatus::Usage));
    return static_cast<int>(AppStatus::Usage);
  }
  try {
    return run(argv[1]);
  } catch (const std::bad_alloc &) {
    return report({AppStatus::Overflow, OBJC3_STDLIB_V2_OUT_OF_MEMORY, 0,
                   "out_of_memory"});
  }
}
