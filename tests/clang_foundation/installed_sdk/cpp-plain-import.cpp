int main(void) {
  PlainData input{40,{7,11}};
  auto result=cpp_plain_echo(c_plain_echo(input));
  return result.marker==42 && result.values[0]==7 && result.values[1]==11 &&
      plain_overload(1)==101 && plain_overload(2L)==202 && plain_mode()==PlainMode::Copy ? 0 : 1;
}
