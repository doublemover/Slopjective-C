#include <objc3c/runtime/objc3_collections.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

static void checked(int status) {
  if (status != 0) std::exit(3);
}

int main(int argc, char **argv) {
  if (argc != 5) return 2;
  const int size = std::atoi(argv[1]), rounds = std::atoi(argv[2]);
  const int seed = std::atoi(argv[3]), delay = std::atoi(argv[4]);
  if (size < 1 || size > 2048 || rounds < 1 || rounds > 128 ||
      seed < 0 || seed > 1024 || delay < 0 || delay > 1) return 2;
  unsigned long long checksum = 0, operations = 0;
  const auto started = std::chrono::steady_clock::now();
  for (int round = 0; round != rounds; ++round) {
    objc3_map_i32_i32_v2_t map = nullptr;
    checked(objc3_runtime_stdlib_map_i32_i32_v2_create(&map));
    ++operations;
    for (int key = 0; key != size; ++key) {
      checked(objc3_runtime_stdlib_map_i32_i32_v2_insert(map, key, key + seed));
      ++operations;
    }
    for (int key = size - 1; key >= 0; --key) {
      int found = 0;
      int32_t value = -1;
      checked(objc3_runtime_stdlib_map_i32_i32_v2_get(map, key, &found, &value));
      if (!found || value != key + seed) return 4;
      checksum += value;
      ++operations;
    }
    uint64_t count = 0;
    checked(objc3_runtime_stdlib_map_i32_i32_v2_count(map, &count));
    if (count != static_cast<uint64_t>(size)) return 5;
    ++operations;
    checked(objc3_runtime_stdlib_map_i32_i32_v2_release(map));
    ++operations;
    // Test-only paired measurement control; semantic work and checksum stay
    // identical. It is never enabled for a product performance sample.
    if (delay) std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - started).count();
  std::printf("{\"checksum\":%llu,\"completed_operations\":%llu,\"nanoseconds\":%lld}\n",
      checksum, operations, static_cast<long long>(nanos));
}
