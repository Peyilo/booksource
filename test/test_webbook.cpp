//
// Created by 夏莳 on 2025/11/30.
//
#include <booksource/webbook.h>
#include <booksource/booksource_parser.h>
#include <iostream>
#include "test_utils.h"

int main() {
    const auto bss = BookSourceParser::parseBookSourceList(getResourceText("bs2.json"));
    auto bs = bss[12];      // 12: qidian.com
    auto searchBooks = WebBook::searchBook(
        bs, "斗破"
    );
    return 0;
}
