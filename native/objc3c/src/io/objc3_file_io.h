#pragma once

#include <filesystem>
#include <string>
#include <vector>

void WriteText(const std::filesystem::path &path, const std::string &contents);
std::string ReadText(const std::filesystem::path &path);
std::string JoinLines(const std::vector<std::string> &lines);
