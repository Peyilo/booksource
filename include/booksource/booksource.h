#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <booksource/engine.h>
#include <nlohmann/json.hpp>
#include <booksource/constants.h>

using std::optional;
using std::string;

struct BookInfoRule {
    optional<string> init = std::nullopt;
    optional<string> name = std::nullopt;
    optional<string> author = std::nullopt;
    optional<string> intro = std::nullopt;
    optional<string> kind = std::nullopt;
    optional<string> lastChapter = std::nullopt;
    optional<string> updateTime = std::nullopt;
    optional<string> coverUrl = std::nullopt;
    optional<string> tocUrl = std::nullopt;
    optional<string> wordCount = std::nullopt;
    optional<string> canReName = std::nullopt;
    optional<string> downloadUrls = std::nullopt;
};

struct BookListRule {
    optional<string> bookList = std::nullopt;
    optional<string> name = std::nullopt;
    optional<string> author = std::nullopt;
    optional<string> intro = std::nullopt;
    optional<string> kind = std::nullopt;
    optional<string> lastChapter = std::nullopt;
    optional<string> updateTime = std::nullopt;
    optional<string> bookUrl = std::nullopt;
    optional<string> coverUrl = std::nullopt;
    optional<string> wordCount = std::nullopt;
};

struct ContentRule {
    optional<string> content = std::nullopt;
    optional<string> title = std::nullopt;
    optional<string> nextContentUrl = std::nullopt;
    optional<string> webJs = std::nullopt;
    optional<string> sourceRegex = std::nullopt;
    optional<string> replaceRegex = std::nullopt;
    optional<string> imageStyle = std::nullopt;
    optional<string> imageDecode = std::nullopt;
    optional<string> payAction = std::nullopt;
};

struct ExploreRule : BookListRule {
};

struct SearchRule : BookListRule {
    optional<string> checkKeyWord = std::nullopt; // 校验关键字
};

struct ReviewRule {
    optional<string> reviewUrl = std::nullopt;
    optional<string> avatarRule = std::nullopt;
    optional<string> contentRule = std::nullopt;
    optional<string> postTimeRule = std::nullopt;
    optional<string> reviewQuoteUrl = std::nullopt;

    optional<string> voteUpUrl = std::nullopt;
    optional<string> voteDownUrl = std::nullopt;
    optional<string> postReviewUrl = std::nullopt;
    optional<string> postQuoteUrl = std::nullopt;
    optional<string> deleteUrl = std::nullopt;
};

struct TocRule {
    optional<string> preUpdateJs = std::nullopt;
    optional<string> chapterList = std::nullopt;
    optional<string> chapterName = std::nullopt;
    optional<string> chapterUrl = std::nullopt;
    optional<string> formatJs = std::nullopt;
    optional<string> isVolume = std::nullopt;
    optional<string> isVip = std::nullopt;
    optional<string> isPay = std::nullopt;
    optional<string> updateTime = std::nullopt;
    optional<string> nextTocUrl = std::nullopt;
};

class BaseSource;

class JsExtensions {
public:
    virtual ~JsExtensions() = default;

    virtual BaseSource* getSource() = 0;
};

class BaseSource : public JsExtensions {
public:
    optional<string> concurrentRate = std::nullopt;
    optional<string> loginUrl = std::nullopt;
    optional<string> loginUi = std::nullopt;
    optional<string> header = std::nullopt;
    optional<bool> enabledCookieJar = std::nullopt;
    optional<string> jsLib = std::nullopt;

    virtual string getTag() = 0;

    virtual string getKey() = 0;

    BaseSource* getSource() override {
        return this;
    }

    optional<string> getLoginJs() const {
        if (!loginUrl.has_value()) {
            return std::nullopt;
        }
        const std::string &loginJs = *loginUrl;
        // @js:xxxx
        if (loginJs.rfind("@js:", 0) == 0) {
            // startsWith("@js:")
            return loginJs.substr(4);
        }
        // <js>xxx</js>
        if (loginJs.rfind("<js>", 0) == 0) {
            // startsWith("<js>")
            size_t endPos = loginJs.find_last_of('<');
            if (endPos != std::string::npos && endPos > 4) {
                return loginJs.substr(4, endPos - 4);
            }
        }
        // default
        return loginJs;
    }

    std::unordered_map<string, string> getHeaderMap(bool hasLoginHeader = false) {
        std::unordered_map<string, string> result;
        if (header.has_value()) {
            try {
                string json;
                const string &headerStr = header.value();
                if (headerStr.rfind("@js:", 0) == 0) {
                    // startsWith("@js:")
                    json = evalJS(headerStr.substr(4));
                } else if (headerStr.rfind("<js>", 0) == 0) {
                    // startsWith("<js>")
                    size_t endPos = headerStr.find_last_of('<');
                    json = evalJS(headerStr.substr(4, endPos - 4));
                } else {
                    json = headerStr;
                }
                parseHeaderMap(json, result);
            } catch (...) {
                // TODO: 输出错误日志
            }
        }
        // 如果没有UA，使用默认的UA
        if (!mapContainsIgnoreCase(result, Constants::UA_KEY)) {
            result[Constants::UA_KEY] = Constants::UA_DEFAULT_VALUE;
        }
        if (hasLoginHeader) {
            // TODO: Cache process
        }
        return result;
    }

    string evalJS(string jsStr) {
        auto engine = QuickJsEngine::current();
        engine.reset();
        // TODO: support more binding variable
        engine.addValue("baseUrl", getKey());
        return engine.eval(jsStr);
    }

private:
    static void parseHeaderMap(
        const std::string &jsonStr,
        std::unordered_map<std::string, std::string> &headerMap
    ) {
        using json = nlohmann::json;
        json j = json::parse(jsonStr);
        if (!j.is_object()) return;
        for (auto &[k, v]: j.items()) {
            if (v.is_string())
                headerMap[k] = v.get<std::string>();
            else {
                throw std::runtime_error("invalid header");
            }
        }
    }

    // 忽略key的大小写，查找map中是否存在指定的key
    static bool mapContainsIgnoreCase(
        const std::unordered_map<std::string, std::string>& m,
        const std::string& key)
    {
        std::string keyLower = key;
        std::ranges::transform(keyLower, keyLower.begin(), ::tolower);

        for (const auto &k: m | std::views::keys) {
            std::string kLower = k;
            std::ranges::transform(kLower, kLower.begin(), ::tolower);
            if (kLower == keyLower)
                return true;
        }
        return false;
    }
};

class BookSource final : public BaseSource {
public:
    string bookSourceUrl = "";
    string bookSourceName = "";
    optional<string> bookSourceGroup = std::nullopt;
    int bookSourceType = 0;
    optional<string> bookUrlPattern = std::nullopt;
    int customOrder = 0;
    bool enabled = true;
    bool enabledExplore = true;
    optional<string> jsLib = std::nullopt;
    optional<bool> enabledCookieJar = true;
    optional<string> concurrentRate = std::nullopt;
    optional<string> header = std::nullopt;
    optional<string> loginUrl = std::nullopt;
    optional<string> loginUi = std::nullopt;
    optional<string> loginCheckJs = std::nullopt;
    optional<string> coverDecodeJs = std::nullopt;
    optional<string> bookSourceComment = std::nullopt;
    optional<string> variableComment = std::nullopt;
    int64_t lastUpdateTime = 0;
    int64_t respondTime = 180000L;
    int weight = 0;
    optional<string> exploreUrl = std::nullopt;
    optional<string> exploreScreen = std::nullopt;
    optional<ExploreRule> ruleExplore = std::nullopt;
    optional<string> searchUrl = std::nullopt;
    optional<SearchRule> ruleSearch = std::nullopt;
    optional<BookInfoRule> ruleBookInfo = std::nullopt;
    optional<TocRule> ruleToc = std::nullopt;
    optional<ContentRule> ruleContent = std::nullopt;
    optional<ReviewRule> ruleReview = std::nullopt;

public:
    string getTag() override {
        return bookSourceName;
    }

    /**
     * 获取唯一标识key：这里使用书源url作为书源的唯一标识
     */
    string getKey() override {
        return bookSourceUrl;
    }
};
