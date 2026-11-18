namespace MeterAPI {
struct Reading {
  int count;
  long long total;
};
Reading initialReading() noexcept;
Reading adjustedReading(Reading value, int increment) noexcept;
}
