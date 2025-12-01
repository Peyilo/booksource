#pragma once
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <optional>
#include <regex>
#include <any>
#include <unordered_set>
#include <booksource/ruledata.h>
#include <booksource/utils.h>
#include <booksource/data.h>
#include <booksource/constants.h>

#include "rule.h"

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

    virtual BaseSource *getSource() = 0;
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

    BaseSource *getSource() override {
        return this;
    }

    std::optional<std::string> getLoginJs() const;

    std::unordered_map<std::string, std::string> getHeaderMap(bool hasLoginHeader = false);

    std::string put(std::string key, std::string value) {
        // TODO
        return value;
    }

    std::string get(std::string key) {
        return "";
    }

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

    SearchRule getSearchRule() {
        if (ruleSearch.has_value()) {
            return ruleSearch.value();
        }
        auto rule = SearchRule();
        ruleSearch = rule;
        return rule;
    }

    ExploreRule getExploreRule() {
        if (ruleExplore.has_value()) {
            return ruleExplore.value();
        }
        auto rule = ExploreRule();
        ruleExplore = rule;
        return rule;
    }
};

class BookSourceParser {
public:
    static BookSource parseBookSource(const std::string &jsonStr);

    static std::vector<BookSource> parseBookSourceList(const std::string &jsonStr);
};

class RuleAnalyzer {
private:
    std::string queue; // 被处理字符串
    size_t pos = 0; // 当前处理到的位置
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
    std::optional<std::string> body;
    std::string url;

    explicit StrResponse(
        std::string _url,
        std::optional<std::string> _body
    ) : body(std::move(_body)), url(std::move(_url)) {
    }
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
                    ? std::optional(std::stoi(*value))
                    : std::nullopt;
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
            if (str.empty()) return false; // 空字符串返回false
            if (str == "false") return false;
        } else if (webView->type() == typeid(bool)) {
            const auto b = std::any_cast<bool>(*webView);
            return b; // webView为false时返回false
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

public:
    std::string ruleUrl;
    std::string url;
    std::optional<std::string> type = std::nullopt;
    std::unordered_map<std::string, std::string> headerMap;

private:
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
    std::regex paramPattern{R"(\s*,\s*(?=\{))"}; // 匹配 http://xx.xx/xxxx , { "method": "POST" } 中的逗号
    std::regex pagePattern{R"(<.*?>)"}; // <a, b, c>

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

class AnalyzeByXPath {
};

class AnalyzeByJSoup {
};

class AnalyzeByJSonPath {
};

enum Mode {
    XPath, Json, Default, Js, Regex
};

class AnalyzeRule;

class SourceRule {
    AnalyzeRule& outer;
    Mode mode;
    std::string rule;
    std::string replaceRegex = "";
    std::string replacement = "";
    bool replaceFirst = false;
    std::unordered_map<std::string, std::string> putMap;
    std::vector<std::string> ruleParam;

public:
    explicit SourceRule(
        AnalyzeRule &_outer,
        const std::string &ruleStr,
        Mode mode = Default);
};

class AnalyzeRule : JsExtensions {
private:
    friend class SourceRule;
    RuleDataInterface *ruleData = nullptr;
    BaseSource *source = nullptr;
    bool preUpdateJs = false;

    BaseBook *book = nullptr;
    BookChapter *chapter = nullptr;
    std::optional<std::string> nextChapterUrl = std::nullopt;
    std::optional<std::any> content = std::nullopt;
    std::optional<std::string> baseUrl = std::nullopt;
    std::optional<std::string> redirectUrl = std::nullopt;
    bool isJSON = false;
    bool isRegex = false;

    std::optional<AnalyzeByXPath> analyzeByXPath = std::nullopt;
    std::optional<AnalyzeByJSoup> analyzeByJSoup = std::nullopt;
    std::optional<AnalyzeByJSonPath> analyzeByJSonPath = std::nullopt;

    std::unordered_map<std::string, std::vector<SourceRule> > stringRuleCache{};
    std::unordered_map<std::string, std::regex> regexCache{};
    // private val scriptCache = hashMapOf<String, CompiledScript>()
    // private var topScopeRef: WeakReference<Scriptable>? = null
    int evalJSCallCount = 0;
    bool loggedNonStandardJSON = false;

public:
    explicit AnalyzeRule(
        RuleDataInterface *_ruleData = nullptr,
        BaseSource *_source = nullptr,
        const bool _preUpdateJs = false
    ) : ruleData(_ruleData), source(_source), preUpdateJs(_preUpdateJs) {
    }

    AnalyzeRule &setContent(
        std::optional<std::any> _content,
        const std::optional<std::string> &_baseUrl = std::nullopt
    ) {
        if (!_content.has_value()) {
            throw std::runtime_error("Content cannot be null");
        }
        content = _content.value();
        // TODO: 根据content初始化isJSON变量
        setBaseUrl(_baseUrl);
        analyzeByJSonPath = std::nullopt;
        analyzeByXPath = std::nullopt;
        analyzeByJSoup = std::nullopt;
        return *this;
    }

    AnalyzeRule &setBaseUrl(std::optional<std::string> _baseUrl) {
        if (_baseUrl.has_value()) {
            this->baseUrl = _baseUrl.value();
        }
        return *this;
    }

    std::optional<std::string> setRedirectUrl(std::string &url) {
        if (NetworkUtils::isDataUrl(url)) {
            return redirectUrl;
        }
        redirectUrl = url;
        return redirectUrl;
    }

    std::vector<std::any> getElements(std::string ruleStr) {
        std::vector<std::any> elements;
        std::optional<std::any> result = std::nullopt;
        auto content = this->content;
        auto ruleList = splitSourceRule(ruleStr, true);
        if (content.has_value() && !ruleList.empty()) {
            result = content;
            for (const auto &sourceRule : ruleList) {

            }
        }
        return elements;
    }

    void putRule(std::unordered_map<std::string, std::string> &map) {
        for (const auto &[key, value] : map) {
            put(key , getString(value));
        }
    }

    std::string getString(std::string ruleStr) {
        // TODO
        return ruleStr;
    }

    std::string put(const std::string &key, const std::string &value) const {
        if (chapter != nullptr) {
            chapter->putVariable(key, value);
        } else if (book != nullptr) {
            book->putVariable(key, value);
        } else if (ruleData != nullptr) {
            ruleData->putVariable(key, value);
        } else if (source != nullptr) {
            source->put(key, value);
        }
        return value;
    }

    std::vector<SourceRule> splitSourceRule(const std::optional<std::string> &ruleStr,
        const bool allInOne = false) {
        std::vector<SourceRule> ruleList;
        if (StringUtils::isNullOrEmpty(ruleStr)) {
            return ruleList;
        }
        auto mMode = Default;
        int start = 0;
        if (allInOne && ruleStr->starts_with(":")) {
            mMode = Regex;
            isRegex = true;
            start = 1;
        } else if (isRegex) {
            mMode = Regex;
        }
        std::string tmp;
        std::smatch match;

        auto searchBegin = ruleStr->cbegin();

        while (std::regex_search(searchBegin, ruleStr->cend(), match, Constants::JS_PATTERN)) {
            const size_t matchStart = match.position() + std::distance(ruleStr->cbegin(), searchBegin);
            const size_t matchEnd   = matchStart + match.length();

            // 如果中间有普通规则
            if (matchStart > start) {
                tmp = ruleStr->substr(start, matchStart - start);
                StringUtils::trim(tmp);
                if (!tmp.empty()) {
                    ruleList.emplace_back(*this, tmp, mMode);
                }
            }

            // JS 部分，对应 group(2) ?: group(1)
            std::string jsCode =
                match[2].matched ? match[2].str() : match[1].str();

            ruleList.emplace_back(*this, jsCode, Js);

            start = matchEnd;
            searchBegin = ruleStr->cbegin() + matchEnd;
        }

        // 最后一段
        if (ruleStr->size() > start) {
            tmp = ruleStr->substr(start);
            StringUtils::trim(tmp);
            if (!tmp.empty()) {
                ruleList.emplace_back(*this, tmp, mMode);
            }
        }
        return ruleList;
    }

    BaseSource *getSource() override {
        return source;
    }
};

namespace BookList {
    using BookFilter = std::function<bool(const std::string &name, const std::string &author)>;

    using BreakCondition = std::function<bool(int size)>;


    static std::optional<SearchBook> getSearchItem() {
        return std::nullopt;
    }

    /**
    * 解析书籍列表，有两个用途：解析搜索结果的书籍列表和解析发现（分类/标签/排行等）结果的书籍列表
    * @param bookSource 书源
    * @param ruleData 规则数据
    * @param analyzeUrl 解析地址对象
    * @param baseUrl 给定关键字搜索或分类/标签/排行等的url链接，如：https://www.example.com/so/?searchkey=斗罗
    * @param body 实际上就是给定关键字搜索或分类/标签/排行等地址返回的网页内容
    * @param isSearch 是否为搜索
    * @param isRedirect 是否为重定向后的地址
    * @param filter 过滤器，返回 false 则过滤掉该书籍
    * @param shouldBreak 当解析到的书籍数量满足中断条件时则中断，输入的size表示当前已解析到的书籍数量，返回值则表示是否中断
    * @return 返回搜索到的书籍列表
    */
    std::vector<SearchBook> analyzeBookList(
        BookSource &bookSource,
        RuleData &ruleData,
        AnalyzeUrl &analyzeUrl,
        std::string &baseUrl,
        const std::optional<std::string> &body,
        bool isSearch = true,
        bool isRedirect = false,
        const std::optional<BookFilter> &filter = std::nullopt,
        const std::optional<BreakCondition> &shouldBreak = std::nullopt
    ) {
        if (!body.has_value()) {
            throw std::runtime_error("Failed to access website: " + analyzeUrl.ruleUrl);
        }
        std::vector<SearchBook> bookList;
        auto analyzeRule = AnalyzeRule(&ruleData, &bookSource);
        analyzeRule.setContent(body).setBaseUrl(baseUrl);
        analyzeRule.setRedirectUrl(baseUrl);
        if (isSearch) {
            // TODO
        }
        auto reverse = false;
        BookListRule bookListRule;
        if (isSearch) {
            bookListRule = static_cast<BookListRule>(bookSource.getSearchRule());
        } else {
            if (StringUtils::isNullOrEmpty(bookSource.getExploreRule().bookList)) {
                bookListRule = static_cast<BookListRule>(bookSource.getSearchRule());
            } else {
                bookListRule = bookSource.getExploreRule();
            }
        }
        std::string ruleList = bookListRule.bookList ? bookListRule.bookList.value() : ""; // 获取书籍列表规则
        // 处理正反序: 如果书籍列表规则前有 - 号表示反序, + 号表示正序，默认为正序
        if (!ruleList.empty() && ruleList[0] == '-') {
            reverse = true;
            ruleList = ruleList.substr(1);
        } else if (!ruleList.empty() && ruleList[0] == '+') {
            ruleList = ruleList.substr(1);
        }
        // 解析书籍列表对应的全部Elements，其中每个Element都对应于一本书籍
        std::vector<std::any> collections = analyzeRule.getElements(ruleList);
        if (!collections.empty() && StringUtils::isNullOrEmpty(bookSource.bookUrlPattern)) {
            // TODO
        } else {
            auto ruleName = analyzeRule.splitSourceRule(bookListRule.name);
            auto ruleBookUrl = analyzeRule.splitSourceRule(bookListRule.bookUrl);
            auto ruleAuthor = analyzeRule.splitSourceRule(bookListRule.author);
            auto ruleCoverUrl = analyzeRule.splitSourceRule(bookListRule.coverUrl);
            auto ruleIntro = analyzeRule.splitSourceRule(bookListRule.intro);
            auto ruleKind = analyzeRule.splitSourceRule(bookListRule.kind);
            auto ruleLastChapter = analyzeRule.splitSourceRule(bookListRule.lastChapter);
            auto ruleWordCount = analyzeRule.splitSourceRule(bookListRule.wordCount);
            // 遍历全部书籍Elements，对每个书籍Element根据给定的书籍基本信息规则解析获得SearchBook
            for (auto index = 0; index < collections.size(); index++) {
                auto item = collections[index];
                auto searchBook = getSearchItem();
                if (searchBook.has_value()) {
                    if (baseUrl == searchBook->bookUrl) {
                        searchBook->infoHtml = body;
                    }
                    bookList.push_back(searchBook.value());
                }
                // 当解析到的书籍数量满足中断条件时则中断
                if (shouldBreak.has_value() && shouldBreak.value()(bookList.size())) {
                    break;
                }
            }
            // 去重的同时保证顺序不变
            std::unordered_set<SearchBook> seen;
            std::vector<SearchBook> lh;
            for (const auto& item : bookList) {
                if (seen.insert(item).second) {
                    lh.push_back(item);
                }
            }
            bookList = lh;
            // 处理顺序反转
            if (reverse) {
                std::ranges::reverse(bookList);
            }
        }
        return bookList;
    }

};
