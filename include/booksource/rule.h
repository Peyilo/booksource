#pragma once
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <optional>
#include <regex>
#include <any>
#include <booksource/ruledata.h>
#include <booksource/utils.h>

struct BookInfoRule {
    std::optional<std::string> init = std::nullopt;
    std::optional<std::string> name = std::nullopt;
    std::optional<std::string> author = std::nullopt;
    std::optional<std::string> intro = std::nullopt;
    std::optional<std::string> kind = std::nullopt;
    std::optional<std::string> lastChapter = std::nullopt;
    std::optional<std::string> updateTime = std::nullopt;
    std::optional<std::string> coverUrl = std::nullopt;
    std::optional<std::string> tocUrl = std::nullopt;
    std::optional<std::string> wordCount = std::nullopt;
    std::optional<std::string> canReName = std::nullopt;
    std::optional<std::string> downloadUrls = std::nullopt;
};

struct BookListRule {
    std::optional<std::string> bookList = std::nullopt;
    std::optional<std::string> name = std::nullopt;
    std::optional<std::string> author = std::nullopt;
    std::optional<std::string> intro = std::nullopt;
    std::optional<std::string> kind = std::nullopt;
    std::optional<std::string> lastChapter = std::nullopt;
    std::optional<std::string> updateTime = std::nullopt;
    std::optional<std::string> bookUrl = std::nullopt;
    std::optional<std::string> coverUrl = std::nullopt;
    std::optional<std::string> wordCount = std::nullopt;
};

struct ContentRule {
    std::optional<std::string> content = std::nullopt;
    std::optional<std::string> title = std::nullopt;
    std::optional<std::string> nextContentUrl = std::nullopt;
    std::optional<std::string> webJs = std::nullopt;
    std::optional<std::string> sourceRegex = std::nullopt;
    std::optional<std::string> replaceRegex = std::nullopt;
    std::optional<std::string> imageStyle = std::nullopt;
    std::optional<std::string> imageDecode = std::nullopt;
    std::optional<std::string> payAction = std::nullopt;
};

struct ExploreRule : BookListRule {
};

struct SearchRule : BookListRule {
    std::optional<std::string> checkKeyWord = std::nullopt; // 校验关键字
};

struct ReviewRule {
    std::optional<std::string> reviewUrl = std::nullopt;
    std::optional<std::string> avatarRule = std::nullopt;
    std::optional<std::string> contentRule = std::nullopt;
    std::optional<std::string> postTimeRule = std::nullopt;
    std::optional<std::string> reviewQuoteUrl = std::nullopt;

    std::optional<std::string> voteUpUrl = std::nullopt;
    std::optional<std::string> voteDownUrl = std::nullopt;
    std::optional<std::string> postReviewUrl = std::nullopt;
    std::optional<std::string> postQuoteUrl = std::nullopt;
    std::optional<std::string> deleteUrl = std::nullopt;
};

struct TocRule {
    std::optional<std::string> preUpdateJs = std::nullopt;
    std::optional<std::string> chapterList = std::nullopt;
    std::optional<std::string> chapterName = std::nullopt;
    std::optional<std::string> chapterUrl = std::nullopt;
    std::optional<std::string> formatJs = std::nullopt;
    std::optional<std::string> isVolume = std::nullopt;
    std::optional<std::string> isVip = std::nullopt;
    std::optional<std::string> isPay = std::nullopt;
    std::optional<std::string> updateTime = std::nullopt;
    std::optional<std::string> nextTocUrl = std::nullopt;
};

class BaseSource;

class JsExtensions {
public:
    virtual ~JsExtensions() = default;

    virtual BaseSource* getSource() = 0;
};

class BaseSource : public JsExtensions {
public:
    std::optional<std::string> concurrentRate = std::nullopt;
    std::optional<std::string> loginUrl = std::nullopt;
    std::optional<std::string> loginUi = std::nullopt;
    std::optional<std::string> header = std::nullopt;
    std::optional<bool> enabledCookieJar = std::nullopt;
    std::optional<std::string> jsLib = std::nullopt;

    virtual std::string getTag() = 0;

    virtual std::string getKey() = 0;

    BaseSource* getSource() override {
        return this;
    }

    std::optional<std::string> getLoginJs() const;

    std::unordered_map<std::string, std::string> getHeaderMap(bool hasLoginHeader = false);

    std::string evalJS(std::string jsStr);
};

class BookSource final : public BaseSource {
public:
    std::string bookSourceUrl;
    std::string bookSourceName;
    std::optional<std::string> bookSourceGroup = std::nullopt;
    int bookSourceType = 0;
    std::optional<std::string> bookUrlPattern = std::nullopt;
    int customOrder = 0;
    bool enabled = true;
    bool enabledExplore = true;
    std::optional<std::string> jsLib = std::nullopt;
    std::optional<bool> enabledCookieJar = true;
    std::optional<std::string> concurrentRate = std::nullopt;
    std::optional<std::string> header = std::nullopt;
    std::optional<std::string> loginUrl = std::nullopt;
    std::optional<std::string> loginUi = std::nullopt;
    std::optional<std::string> loginCheckJs = std::nullopt;
    std::optional<std::string> coverDecodeJs = std::nullopt;
    std::optional<std::string> bookSourceComment = std::nullopt;
    std::optional<std::string> variableComment = std::nullopt;
    int64_t lastUpdateTime = 0;
    int64_t respondTime = 180000L;
    int weight = 0;
    std::optional<std::string> exploreUrl = std::nullopt;
    std::optional<std::string> exploreScreen = std::nullopt;
    std::optional<ExploreRule> ruleExplore = std::nullopt;
    std::optional<std::string> searchUrl = std::nullopt;
    std::optional<SearchRule> ruleSearch = std::nullopt;
    std::optional<BookInfoRule> ruleBookInfo = std::nullopt;
    std::optional<TocRule> ruleToc = std::nullopt;
    std::optional<ContentRule> ruleContent = std::nullopt;
    std::optional<ReviewRule> ruleReview = std::nullopt;

public:
    std::string getTag() override;

    /**
     * 获取唯一标识key：这里使用书源url作为书源的唯一标识
     */
    std::string getKey() override;
};

class BookSourceParser {
public:
    static BookSource parseBookSource(const std::string& jsonStr);

    static std::vector<BookSource> parseBookSourceList(const std::string& jsonStr);
};

class RuleAnalyzer {
private:
    std::string queue;      // 被处理字符串
    size_t pos = 0;         // 当前处理到的位置
    size_t start = 0;
    size_t startX = 0;

    int step = 0;
    std::vector<std::string> rule;
    bool codeMode = false;

public:
    std::string elementsType;

    explicit RuleAnalyzer(std::string data, const bool code = false)
        : queue(std::move(data)), codeMode(code) {
    }

    void reSetPos() {
        pos = 0;
        startX = 0;
    }

private:
    // consumeTo — 查找 seq
    bool consumeTo(const std::string &seq);

    // consumeToAny — 查找多个 seq
    bool consumeToAny(const std::vector<std::string> &seq);

    // 查找任一字符
    int findToAny(const std::vector<char> &chars) const;

    // 平衡检查（用于 {...} / (...) 等）
    bool chompBalanced(char open, char close);

public:
    // ===========================
    //  innerRule("[[", "]]", handler)
    // ===========================
    std::string innerRule(
        const std::string &startStr,
        const std::string &endStr,
        const std::function<std::string(const std::string &)> &handler
    );

    // ===================================
    // 等价于 Kotlin innerRule("{", 1, 1)
    // ===================================
    std::string innerRuleCode(
        const std::string &symbol,
        int startStep,
        int endStep,
        const std::function<std::string(const std::string &)> &handler
    );

    // ===========================
    // splitRule("&&", "||") 等
    // ===========================
    std::vector<std::string> splitRule(const std::vector<std::string> &split);

private:
    // 递归切分
    void splitRuleRec(const std::vector<std::string> &split);
};


class StrResponse {
public:
};

enum RequestMethod {
    GET, POST
};

class UrlOption {
private:
    std::optional<std::string> method = std::nullopt;
    std::optional<std::string> charset = std::nullopt;
    std::optional<std::any> headers = std::nullopt;
    std::optional<std::any> body = std::nullopt;

    std::optional<std::string> origin = std::nullopt;

    std::optional<int> retry = std::nullopt;
    std::optional<std::string> type = std::nullopt;
    std::optional<std::any> webView = std::nullopt;
    std::optional<std::string> webJs = std::nullopt;
    std::optional<std::string> js = std::nullopt;
    std::optional<long> serverID = std::nullopt;
    std::optional<long> webViewDelayTime = std::nullopt;
public:
    void setMethod(std::optional<std::string> value) {
        method = value.has_value() && !value->empty() ? value : std::nullopt;
    }

    std::optional<std::string> getMethod() {
        return method;
    }

    void setCharset(std::optional<std::string> value) {
        charset = value.has_value() && !value->empty() ? value : std::nullopt;
    }

    std::optional<std::string> getCharset() {
        return charset;
    }

    void setOrigin(std::optional<std::string> value) {
        origin = value.has_value() && !value->empty() ? value : std::nullopt;
    }

    std::optional<std::string> getOrigin() {
        return origin;
    }

    void setRetry(const std::optional<std::string> &value) {
        retry = value.has_value() && !value->empty()
        ? std::optional(std::stoi(*value)) : std::nullopt;
    }

    int getRetry() const {
       return retry ? *retry : 0;
    }

    void setType(std::optional<std::string> value) {
        type = value.has_value() && !value->empty() ? value : std::nullopt;
    }

    std::optional<std::string> getType() {
        return type;
    }

    bool useWebView() const {
        if (!webView) return false;
        if (webView->type() == typeid(std::string)) {
            const auto str = std::any_cast<std::string>(*webView);
            if (str.empty()) return false;      // 空字符串返回false
            if (str == "false") return false;
        } else if (webView->type() == typeid(bool)) {
            const auto b = std::any_cast<bool>(*webView);
            return b;           // webView为false时返回false
        }
        return true;
    }

    void useWebView(const bool value) {
        webView = value ? std::optional(true) : std::nullopt;
    }

    void setHeaders(std::optional<std::string> value) {
        if (!value || value->empty()) {
            // 若value为null或者空字符串
            headers = std::nullopt;
        } else {
            auto map = JsonUtils::jsonStringToMap(*value);
            this->headers = map;
        }
    }

};

class AnalyzeUrl final : JsExtensions {
private:
    std::string mUrl;
    std::optional<std::string> key = std::nullopt;
    std::optional<int> page = std::nullopt;
    std::optional<std::string> speakText = std::nullopt;
    std::optional<int> speakSpeed = std::nullopt;
    std::string baseUrl;
    BaseSource *source = nullptr;
    RuleDataInterface *ruleData = nullptr;;
    BookChapter *chapter = nullptr;
    std::optional<long> readTimeout = std::nullopt;
    std::optional<long> callTimeout = std::nullopt;
    std::optional<std::unordered_map<std::string, std::string> > headerMapF = std::nullopt;
    bool hasLoginHeader = true;

    std::string ruleUrl;
    std::string url;
    std::optional<std::string> type = std::nullopt;
    std::unordered_map<std::string, std::string> headerMap;

    std::optional<std::string> body = std::nullopt;
    std::string urlNoQuery;
    std::optional<std::string> encodedForm = std::nullopt;
    std::optional<std::string> encodedQuery = std::nullopt;
    std::optional<std::string> charset = std::nullopt;
    RequestMethod method = GET;
    std::optional<std::string> proxy = std::nullopt;
    int retry = 0;
    bool useWebView = false;
    std::optional<std::string> webJs = std::nullopt;
    bool enabledCookieJar = false;
    std::string domain;
    long webViewDelayTime = 0;

    std::optional<long> serverID = std::nullopt;

public:
    explicit AnalyzeUrl(
        std::string _mUrl,
        std::optional<std::string> _key = std::nullopt,
        std::optional<int> _page = std::nullopt,
        std::optional<std::string> _speakText = std::nullopt,
        std::optional<int> _speakSpeed = std::nullopt,
        std::string _baseUrl = "",
        BaseSource *_source = nullptr,
        RuleDataInterface *_ruleData = nullptr,
        BookChapter *_chapter = nullptr,
        std::optional<long> _readTimeout = std::nullopt,
        std::optional<long> _callTimeout = std::nullopt,
        std::optional<std::unordered_map<std::string, std::string> > _headerMapF = std::nullopt,
        bool _hasLoginHeader = true
    );

    std::string evalJS(const std::string &jsStr,
        const std::optional<std::string> &result = std::nullopt);


public:
    StrResponse getStrResponse(
        std::string *jsStr = nullptr,
        std::string *sourceRegex = nullptr,
        bool useWebView = false
    );

    BaseSource *getSource() override {
        return source;
    }

private:
    std::regex paramPattern{R"(\s*,\s*(?=\{))"};    // 匹配 http://xx.xx/xxxx , { "method": "POST" } 中的逗号
    std::regex pagePattern{R"(<.*?>)"};             // <a, b, c>

    void initUrl();

    /**
    * 执行@js,<js></js>
    */
    void analyzeJs();

    /**
     * 替换url中关于key、page的规则为实际的字符串
     */
    void replaceKeyPageJs();

    void analyzeUrl();
};

