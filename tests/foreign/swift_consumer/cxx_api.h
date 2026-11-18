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
}
