#pragma once

#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <booksource/data.h>
#include <booksource/rule.h>


namespace WebBook {
    using BookFilter = std::function<bool(const std::string &name, const std::string &author)>;

    using BreakCondition = std::function<bool(int size)>;

    inline std::vector<SearchBook> searchBook(
        BookSource &bookSource,
        const std::string &key,
        std::optional<int> page = 1,
        const std::optional<BookFilter> &filter = std::nullopt,
        const std::optional<BreakCondition> &shouldBreak = std::nullopt
    ) {
        const auto &searchUrl = bookSource.searchUrl;
        // searchUrl为null或者空字符串时抛异常
        if (!searchUrl || searchUrl.value().empty()) {
            throw std::invalid_argument("searchUrl is empty");
        }
        auto ruleData = RuleData();
        auto analyzeUrl = AnalyzeUrl(
            *searchUrl, key, page,
            std::nullopt, std::nullopt,
            bookSource.bookSourceUrl,
            &bookSource,
            &ruleData
        );
        // 获取搜索结果的网页内容
        auto res = analyzeUrl.getStrResponse();
        // 检测书源是否已登录
        // bookSource.loginCheckJs?.let { checkJs ->
        //     if (checkJs.isNotBlank()) {
        //         res = analyzeUrl.evalJS(checkJs, res) as StrResponse
        //     }
        // }
        // TODO
        return BookList::analyzeBookList(
            bookSource, ruleData, analyzeUrl,
            res.url, res.body, true, false,
            filter, shouldBreak
        );
    }
}
