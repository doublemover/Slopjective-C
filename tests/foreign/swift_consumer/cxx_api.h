namespace MeterAPI {
struct Reading {
  int count;
  long long total;
};
Reading initialReading() noexcept;
Reading adjustedReading(Reading value, int increment) noexcept;

class ReadingOwner {
public:
  explicit ReadingOwner(int amount) noexcept;
  ReadingOwner(const ReadingOwner &other) noexcept;
  ReadingOwner(ReadingOwner &&other) noexcept;
  ~ReadingOwner() noexcept;
  int amount() const noexcept;

private:
  int *storage;
};

ReadingOwner initialOwner(int amount) noexcept;
ReadingOwner adjustedOwner(ReadingOwner value, int increment) noexcept;
int ownerAllocations() noexcept;
int ownerDestructions() noexcept;
int ownerCopies() noexcept;

class UniqueReading {
public:
  explicit UniqueReading(int amount) noexcept;
  UniqueReading(const UniqueReading &) = delete;
  UniqueReading(UniqueReading &&other) noexcept;
  ~UniqueReading() noexcept;
  int amount() const noexcept;
  void add(int increment) noexcept;

private:
  int *storage;
};

UniqueReading initialUniqueReading(int amount) noexcept;
UniqueReading adjustedUniqueReading(UniqueReading value, int increment) noexcept;
int uniqueAllocations() noexcept;
int uniqueDestructions() noexcept;
int uniqueMoves() noexcept;
}
