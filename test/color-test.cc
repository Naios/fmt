// Formatting library for C++ - color tests
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#include "fmt/color.h"

#include <iterator>
#include <string>
#include <utility>

#include "gtest-extra.h"

struct ColorFunctionPrintTag {};
struct ColorFunctionFormatTag {};
struct ColorFunctionFormatToOutputTag {};
struct ColorFunctionFormatToNTag {};
struct ColorFunctionFormatToBufferTag {};

template <typename T> struct ColorsTest;
template <> struct ColorsTest<ColorFunctionPrintTag> : public testing::Test {
  virtual ~ColorsTest();

  template <typename... Args>
  void expect_color(const std::string& expected, const fmt::text_style& ts,
                    Args&&... args) {
    EXPECT_WRITE(stdout, fmt::print(stdout, ts, std::forward<Args>(args)...),
                 expected);
  }
};

template <> struct ColorsTest<ColorFunctionFormatTag> : public testing::Test {
  virtual ~ColorsTest();

  template <typename... Args>
  void expect_color(const std::string& expected, const fmt::text_style& ts,
                    Args&&... args) {
    EXPECT_EQ(expected, fmt::format(ts, std::forward<Args>(args)...));
  }
};
template <>
struct ColorsTest<ColorFunctionFormatToOutputTag> : public testing::Test {
  virtual ~ColorsTest();

  template <typename... Args>
  void expect_color(const std::string& expected, const fmt::text_style& ts,
                    Args&&... args) {
    std::string str;
    fmt::format_to(std::back_inserter(str), ts, std::forward<Args>(args)...);
    EXPECT_EQ(expected, str);
  }
};
template <>
struct ColorsTest<ColorFunctionFormatToBufferTag> : public testing::Test {
  virtual ~ColorsTest();

  template <typename... Args>
  void expect_color(const std::string& expected, const fmt::text_style& ts,
                    Args&&... args) {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer, ts, std::forward<Args>(args)...);
    EXPECT_EQ(expected,
              std::string(buffer.data(), buffer.data() + buffer.size()));
  }
};

// Clang: error: 'ColorsTest<ColorFunctionFormatToBufferTag>' has no out-of-line
//               virtual method definitions
ColorsTest<ColorFunctionPrintTag>::~ColorsTest() {}
ColorsTest<ColorFunctionFormatTag>::~ColorsTest() {}
ColorsTest<ColorFunctionFormatToOutputTag>::~ColorsTest() {}
ColorsTest<ColorFunctionFormatToBufferTag>::~ColorsTest() {}

using types = ::testing::Types<ColorFunctionPrintTag, ColorFunctionFormatTag,
                               ColorFunctionFormatToOutputTag,
                               ColorFunctionFormatToBufferTag>;
TYPED_TEST_CASE(ColorsTest, types);

TYPED_TEST(ColorsTest, CanColorizeRBG) {
  this->expect_color("\x1b[38;2;255;020;030mrgb(255,20,30)\x1b[0m",
                     fg(fmt::rgb(255, 20, 30)), "rgb(255,20,30)");

  this->expect_color("\x1b[38;2;000;000;255mblue\x1b[0m", fg(fmt::color::blue),
                     "blue");

  this->expect_color(
      "\x1b[38;2;000;000;255m\x1b[48;2;255;000;000mtwo color\x1b[0m",
      fg(fmt::color::blue) | bg(fmt::color::red), "two color");
}

TYPED_TEST(ColorsTest, CanColorizeEmphasis) {
  this->expect_color("\x1b[1mbold\x1b[0m", fmt::emphasis::bold, "bold");

  this->expect_color("\x1b[3mitalic\x1b[0m", fmt::emphasis::italic, "italic");

  this->expect_color("\x1b[4munderline\x1b[0m", fmt::emphasis::underline,
                     "underline");

  this->expect_color("\x1b[9mstrikethrough\x1b[0m",
                     fmt::emphasis::strikethrough, "strikethrough");
}

TYPED_TEST(ColorsTest, CanColorizeRBGEmphasisMixed) {
  this->expect_color("\x1b[1m\x1b[38;2;000;000;255mblue/bold\x1b[0m",
                     fg(fmt::color::blue) | fmt::emphasis::bold, "blue/bold");

  this->expect_color("\x1b[1mbold error\x1b[0m", fmt::emphasis::bold,
                     "bold error");

  this->expect_color("\x1b[38;2;000;000;255mblue log\x1b[0m",
                     fg(fmt::color::blue), "blue log");
}

TYPED_TEST(ColorsTest, CanColorizeEmptyStyle) {
  this->expect_color("hi", fmt::text_style(), "hi");
}

TYPED_TEST(ColorsTest, CanColorizeTerminalColor) {
  this->expect_color("\x1b[31mtred\x1b[0m", fg(fmt::terminal_color::red),
                     "tred");

  this->expect_color("\x1b[46mtcyan\x1b[0m", bg(fmt::terminal_color::cyan),
                     "tcyan");

  this->expect_color("\x1b[92mtbgreen\x1b[0m",
                     fg(fmt::terminal_color::bright_green), "tbgreen");

  this->expect_color("\x1b[105mtbmagenta\x1b[0m",
                     bg(fmt::terminal_color::bright_magenta), "tbmagenta");
}
