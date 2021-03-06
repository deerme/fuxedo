#pragma once
// This file is part of Fuxedo
// Copyright (C) 2017 Aivars Kalvans <aivars.kalvans@gmail.com>

#include <memory>

#include <fml32.h>
#include "basic_parser.h"
#include "fux.h"

class extreader : public basic_parser {
 public:
  extreader(std::istream &f) : basic_parser(f) {}
  extreader(FILE *f) : basic_parser(f) {}

  bool parse() {
    if (eof()) {
      return false;
    }
    while (parse_line(buf, 0))
      ;
    return true;
  }
  FBFR32 *get() { return buf.get(); }

 private:
  fux::fml32ptr buf;
  fux::fml32ptr parse_buf(int indent) {
    auto fml = fux::fml32ptr();
    while (parse_line(fml, indent))
      ;
    return fml;
  }

  bool parse_line(fux::fml32ptr &fml, int indent) {
    if (accept('\n')) {
      // empty line terminates buffer
      return false;
    }

    for (int i = 0; i < indent; i++) {
      if (!accept('\t')) {
        throw basic_parser_error(
            "incomplete input", row_, col_,
            "expected tab, found '" + std::string(1, sym_) + "'");
      }
    }

    std::string f, fname, fvalue;
    flag(&f);

    if (!field_name(&fname)) {
      throw basic_parser_error(
          "field name not found", row_, col_,
          "expected field name, found '" + std::string(1, sym_) + "'");
    }
    if (!accept('\t')) {
      throw basic_parser_error(
          "incomplete input", row_, col_,
          "expected tab, found '" + std::string(1, sym_) + "'");
    }

    value(&fvalue);

    if (!accept('\n')) {
      throw basic_parser_error(
          "incomplete input", row_, col_,
          "expected newline '\\n', found '" + std::string(1, sym_) + "'");
    }

    auto fieldid = Fldid32(const_cast<char *>(fname.c_str()));
    if (fieldid == BADFLDID) {
    }

    if (Fldtype32(fieldid) == FLD_FML32) {
      auto nested = parse_buf(indent + 1);
      fml.mutate([&](FBFR32 *fbfr) {
        return Fadd32(fbfr, fieldid, reinterpret_cast<char *>(nested.get()), 0);
      });
    } else {
      fml.mutate([&](FBFR32 *fbfr) {
        return CFadd32(fbfr, fieldid, const_cast<char *>(fvalue.c_str()), 0,
                       FLD_STRING);
      });
    }

    return true;
  }

  bool flag(std::string *s = nullptr) {
    if (accept([](int c) { return strchr("+-=", c) != nullptr; }, s)) {
      return true;
    }
    return false;
  }

  bool value(std::string *s = nullptr) {
    while (true) {
      if (accept('\\')) {
        hex(s);
      } else if (accept([](int c) { return c != '\n'; }, s)) {
      } else {
        break;
      }
    }
    // No value is fine as well
    return true;
  }

  bool comment(std::string *s = nullptr) {
    while (accept([](int c) { return c != '\n'; }, s))
      ;
    return true;
  }
};
