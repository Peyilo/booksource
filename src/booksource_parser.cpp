#include <booksource/booksource_parser.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static void debugPrint(const std::string& msg) {}

static void parse(const json &j, BookInfoRule &o) {
    // Sometimes BookInfoRule is an array → ignore
    if (!j.is_object()) {
        debugPrint("Warning: BookInfoRule expects an object but got a non-object. Skipped.\n");
        return;
    }

    for (auto& [key, value] : j.items()) {

        if (key == "init")              o.init = value.get<std::string>();
        else if (key == "name")         o.name = value.get<std::string>();
        else if (key == "author")       o.author = value.get<std::string>();
        else if (key == "intro")        o.intro = value.get<std::string>();
        else if (key == "kind")         o.kind = value.get<std::string>();
        else if (key == "lastChapter")  o.lastChapter = value.get<std::string>();
        else if (key == "updateTime")   o.updateTime = value.get<std::string>();
        else if (key == "coverUrl")     o.coverUrl = value.get<std::string>();
        else if (key == "tocUrl")       o.tocUrl = value.get<std::string>();
        else if (key == "wordCount")    o.wordCount = value.get<std::string>();
        else if (key == "canReName")    o.canReName = value.get<std::string>();
        else if (key == "downloadUrls") o.downloadUrls = value.get<std::string>();

        else {
            std::string keyName ="BookInfoRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}


static void parse(const json &j, BookListRule &o) {
    for (auto& [key, value] : j.items()) {

        if (key == "bookList")        o.bookList = value.get<std::string>();
        else if (key == "name")       o.name = value.get<std::string>();
        else if (key == "author")     o.author = value.get<std::string>();
        else if (key == "intro")      o.intro = value.get<std::string>();
        else if (key == "kind")       o.kind = value.get<std::string>();
        else if (key == "lastChapter")o.lastChapter = value.get<std::string>();
        else if (key == "updateTime") o.updateTime = value.get<std::string>();
        else if (key == "bookUrl")    o.bookUrl = value.get<std::string>();
        else if (key == "coverUrl")   o.coverUrl = value.get<std::string>();
        else if (key == "wordCount")  o.wordCount = value.get<std::string>();

        else {
            std::string keyName = "BookListRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}

static void parse(const json &j, ExploreRule &o) {
    for (auto& [key, value] : j.items()) {

        if (key == "bookList")        o.bookList = value.get<std::string>();
        else if (key == "name")       o.name = value.get<std::string>();
        else if (key == "author")     o.author = value.get<std::string>();
        else if (key == "intro")      o.intro = value.get<std::string>();
        else if (key == "kind")       o.kind = value.get<std::string>();
        else if (key == "lastChapter")o.lastChapter = value.get<std::string>();
        else if (key == "updateTime") o.updateTime = value.get<std::string>();
        else if (key == "bookUrl")    o.bookUrl = value.get<std::string>();
        else if (key == "coverUrl")   o.coverUrl = value.get<std::string>();
        else if (key == "wordCount")  o.wordCount = value.get<std::string>();

        else {
            std::string keyName = "ExploreRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}

static void parse(const json &j, SearchRule &o) {
    for (auto& [key, value] : j.items()) {

        if (key == "bookList")          o.bookList = value.get<std::string>();
        else if (key == "name")         o.name = value.get<std::string>();
        else if (key == "author")       o.author = value.get<std::string>();
        else if (key == "intro")        o.intro = value.get<std::string>();
        else if (key == "kind")         o.kind = value.get<std::string>();
        else if (key == "lastChapter")  o.lastChapter = value.get<std::string>();
        else if (key == "updateTime")   o.updateTime = value.get<std::string>();
        else if (key == "bookUrl")      o.bookUrl = value.get<std::string>();
        else if (key == "coverUrl")     o.coverUrl = value.get<std::string>();
        else if (key == "wordCount")    o.wordCount = value.get<std::string>();
        else if (key == "checkKeyWord") o.checkKeyWord = value.get<std::string>();

        else {
            std::string keyName = "SearchRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}


static void parse(const json &j, ContentRule &o) {
    if (!j.is_object()) return;

    for (auto& [key, value] : j.items()) {

        if (key == "content")        o.content = value.get<std::string>();
        else if (key == "title")     o.title = value.get<std::string>();
        else if (key == "nextContentUrl") o.nextContentUrl = value.get<std::string>();
        else if (key == "webJs")     o.webJs = value.get<std::string>();
        else if (key == "sourceRegex") o.sourceRegex = value.get<std::string>();
        else if (key == "replaceRegex") o.replaceRegex = value.get<std::string>();
        else if (key == "imageStyle") o.imageStyle = value.get<std::string>();
        else if (key == "imageDecode") o.imageDecode = value.get<std::string>();
        else if (key == "payAction")  o.payAction = value.get<std::string>();

        else {
            std::string keyName = "ContentRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}


static void parse(const json &j, ReviewRule &o) {
    if (!j.is_object()) return;

    for (auto& [key, value] : j.items()) {

        if (key == "reviewUrl")        o.reviewUrl = value.get<std::string>();
        else if (key == "avatarRule")  o.avatarRule = value.get<std::string>();
        else if (key == "contentRule") o.contentRule = value.get<std::string>();
        else if (key == "postTimeRule")o.postTimeRule = value.get<std::string>();
        else if (key == "reviewQuoteUrl") o.reviewQuoteUrl = value.get<std::string>();

        else if (key == "voteUpUrl")   o.voteUpUrl = value.get<std::string>();
        else if (key == "voteDownUrl") o.voteDownUrl = value.get<std::string>();
        else if (key == "postReviewUrl") o.postReviewUrl = value.get<std::string>();
        else if (key == "postQuoteUrl")  o.postQuoteUrl = value.get<std::string>();
        else if (key == "deleteUrl")     o.deleteUrl = value.get<std::string>();

        else {
            std::string keyName = "ReviewRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}


static void parse(const json &j, TocRule &o) {
    if (!j.is_object()) return;

    for (auto& [key, value] : j.items()) {

        if (key == "preUpdateJs")   o.preUpdateJs = value.get<std::string>();
        else if (key == "chapterList") o.chapterList = value.get<std::string>();
        else if (key == "chapterName") o.chapterName = value.get<std::string>();
        else if (key == "chapterUrl")  o.chapterUrl = value.get<std::string>();
        else if (key == "formatJs")    o.formatJs = value.get<std::string>();
        else if (key == "isVolume")    o.isVolume = value.get<std::string>();
        else if (key == "isVip")       o.isVip = value.get<std::string>();
        else if (key == "isPay")       o.isPay = value.get<std::string>();
        else if (key == "updateTime")  o.updateTime = value.get<std::string>();
        else if (key == "nextTocUrl")  o.nextTocUrl = value.get<std::string>();

        else {
            std::string keyName = "TocRule." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}


static void parse(const json &j, BookSource &o) {
    for (auto& [key, value] : j.items()) {
        if (key == "bookSourceUrl")
            o.bookSourceUrl = value.get<std::string>();
        else if (key == "bookSourceName")
            o.bookSourceName = value.get<std::string>();
        else if (key == "bookSourceGroup")
            o.bookSourceGroup = value.get<std::string>();
        else if (key == "bookSourceType")
            o.bookSourceType = value.get<int>();
        else if (key == "bookUrlPattern")
            o.bookUrlPattern = value.get<std::string>();
        else if (key == "customOrder")
            o.customOrder = value.get<int>();
        else if (key == "enabled")
            o.enabled = value.get<bool>();
        else if (key == "enabledExplore")
            o.enabledExplore = value.get<bool>();
        else if (key == "jsLib")
            o.jsLib = value.get<std::string>();
        else if (key == "enabledCookieJar")
            o.enabledCookieJar = value.get<bool>();
        else if (key == "concurrentRate")
            o.concurrentRate = value.get<std::string>();
        else if (key == "header")
            o.header = value.get<std::string>();
        else if (key == "loginUrl")
            o.loginUrl = value.get<std::string>();
        else if (key == "loginUi")
            o.loginUi = value.get<std::string>();
        else if (key == "loginCheckJs")
            o.loginCheckJs = value.get<std::string>();
        else if (key == "coverDecodeJs")
            o.coverDecodeJs = value.get<std::string>();
        else if (key == "bookSourceComment")
            o.bookSourceComment = value.get<std::string>();
        else if (key == "variableComment")
            o.variableComment = value.get<std::string>();
        else if (key == "lastUpdateTime") {
            // 支持字符串或整数
            if (value.is_number_integer())
                o.lastUpdateTime = value.get<long long>();
            else if (value.is_string())
                o.lastUpdateTime = std::stoll(value.get<std::string>());
            else
                debugPrint("Error: Field \"lastUpdateTime\" must be integer or string.\n");
        }
        else if (key == "respondTime") {
            if (value.is_number_integer())
                o.respondTime = value.get<long long>();
            else if (value.is_string())
                o.respondTime = std::stoll(value.get<std::string>());
            else
                debugPrint("Error: Field \"respondTime\" must be integer or string.\n");
        }
        else if (key == "weight")
            o.weight = value.get<int>();
        else if (key == "exploreUrl")
            o.exploreUrl = value.get<std::string>();
        else if (key == "exploreScreen")
            o.exploreScreen = value.get<std::string>();
        else if (key == "searchUrl")
            o.searchUrl = value.get<std::string>();

        // 嵌套字段处理
        else if (key == "ruleExplore") {
            if (!o.ruleExplore) o.ruleExplore.emplace();
            parse(value, *o.ruleExplore);
        }

        else if (key == "ruleSearch") {
            if (!o.ruleSearch) o.ruleSearch.emplace();
            parse(value, *o.ruleSearch);
        }

        else if (key == "ruleBookInfo") {
            if (!o.ruleBookInfo) o.ruleBookInfo.emplace();
            parse(value, *o.ruleBookInfo);
        }

        else if (key == "ruleToc") {
            if (!o.ruleToc) o.ruleToc.emplace();
            parse(value, *o.ruleToc);
        }

        else if (key == "ruleContent") {
            if (!o.ruleContent) o.ruleContent.emplace();
            parse(value, *o.ruleContent);
        }

        else if (key == "ruleReview") {
            if (!o.ruleReview) o.ruleReview.emplace();
            parse(value, *o.ruleReview);
        }

        // 未知字段
        else {
            std::string keyName = "BookSource." + key;
            debugPrint("Warning: Unknown field \"" + keyName + "\". Ignored.\n");
        }
    }
}

BookSource BookSourceParser::parseBookSource(const std::string& jsonStr) {
    const json j = json::parse(jsonStr);
    BookSource o;
    parse(j, o);
    return o;
}

std::vector<BookSource> BookSourceParser::parseBookSourceList(const std::string& jsonStr) {
    const json arr = json::parse(jsonStr);
    std::vector<BookSource> list;

    if (!arr.is_array()) {
        std::cerr << "parseBookSourceList failed：JSON is not array\n";
        return list;
    }

    list.reserve(arr.size());

    int index = 0;
    for (const auto& item : arr) {
        try {
            BookSource o;
            parse(item, o);   // 如果解析失败，就会抛异常
            list.push_back(std::move(o));
        } catch (std::exception& e) {
            std::cerr << "Failed to parse element at index " << index
                      << ": " << e.what() << "\n";
        }
        index++;
    }

    return list;
}
