#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <booksource/rule.h>
#include <booksource/engine.h>
#include <booksource/utils.h>
#include <booksource/constants.h>

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
    const std::unordered_map<std::string, std::string> &m,
    const std::string &key) {
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

static std::string replaceAll(
    const std::string &src,
    const std::string &from,
    const std::string &to) {
    if (from.empty()) return src;
    std::string result = src;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.size(), to);
        pos += to.size();
    }
    return result;
}

// 对给定的字符串s按照逗号','进行分割，将分割结果保存至out中
static void splitByComma(const std::string &s, std::vector<std::string> &out) {
    std::string token;
    std::stringstream ss(s);
    while (std::getline(ss, token, ',')) {
        out.push_back(token);
    }
}

static std::string trimCopy(std::string s) {
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::ranges::find_if(s, notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

std::optional<std::string> BaseSource::getLoginJs() const {
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

std::unordered_map<std::string, std::string> BaseSource::getHeaderMap(bool hasLoginHeader) {
    std::unordered_map<std::string, std::string> result;
    if (header.has_value()) {
        try {
            std::string json;
            const std::string &headerStr = header.value();
            if (headerStr.rfind("@js:", 0) == 0) {
                // startsWith("@js:")
                json = evalJS(headerStr.substr(4));
            } else if (headerStr.rfind("<js>", 0) == 0) {
                // startsWith("<js>")
                const size_t endPos = headerStr.find_last_of('<');
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

std::string BaseSource::evalJS(std::string jsStr) {
    auto engine = QuickJsEngine::current();
    engine.reset();
    // TODO: support more binding variable
    engine.addValue("baseUrl", getKey());
    return engine.eval(jsStr);
}

std::string BookSource::getTag() {
    return bookSourceName;
}

std::string BookSource::getKey() {
    return bookSourceUrl;
}


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

// consumeTo — 查找 seq
bool RuleAnalyzer::consumeTo(const std::string &seq) {
    start = pos;
    const size_t offset = queue.find(seq, pos);
    if (offset == std::string::npos) return false;
    pos = offset;
    return true;
}

// consumeToAny — 查找多个 seq
bool RuleAnalyzer::consumeToAny(const std::vector<std::string> &seq) {
    size_t p = pos;

    while (p < queue.size()) {
        for (const auto &s: seq) {
            if (queue.compare(p, s.size(), s) == 0) {
                step = static_cast<int>(s.size());
                pos = p;
                return true;
            }
        }

        p++;
    }
    return false;
}

// 查找任一字符
int RuleAnalyzer::findToAny(const std::vector<char> &chars) const {
    size_t p = pos;
    while (p < queue.size()) {
        for (const char c: chars) {
            if (queue[p] == c) return static_cast<int>(p);
        }
        p++;
    }
    return -1;
}

// 平衡检查（用于 {...} / (...) 等）
bool RuleAnalyzer::chompBalanced(char open, char close) {
    int depth = 0;
    bool inSingle = false;
    bool inDouble = false;
    size_t p = pos;

    while (p < queue.size()) {
        const char c = queue[p];
        p++;

        if (c == '\\') {
            p++; // 跳过被转义的字符
            continue;
        }

        if (c == '\'' && !inDouble) {
            inSingle = !inSingle;
            continue;
        }
        if (c == '"' && !inSingle) {
            inDouble = !inDouble;
            continue;
        }

        if (inSingle || inDouble) continue;

        if (c == open) depth++;
        else if (c == close) depth--;

        if (depth == 0) {
            pos = p;
            return true;
        }
    }

    return false;
}

std::string RuleAnalyzer::innerRule(
    const std::string &startStr,
    const std::string &endStr,
    const std::function<std::string(const std::string &)> &handler
) {
    std::string out;

    while (consumeTo(startStr)) {
        const size_t startPos = pos;
        pos += startStr.size();
        const size_t innerStart = pos;

        if (!consumeTo(endStr)) break;
        const size_t innerEnd = pos;

        std::string inner = queue.substr(innerStart, innerEnd - innerStart);
        const std::string replaced = handler(inner);

        out += queue.substr(startX, startPos - startX);
        out += replaced;

        pos += endStr.size();
        startX = pos;
    }

    out += queue.substr(startX);
    return out;
}

// ===================================
// 等价于 Kotlin innerRule("{", 1, 1)
// ===================================
std::string RuleAnalyzer::innerRuleCode(
    const std::string &symbol,
    const int startStep,
    const int endStep,
    const std::function<std::string(const std::string &)> &handler
) {
    std::string out;

    while (consumeTo(symbol)) {
        const size_t matchPos = pos;

        if (chompBalanced('{', '}')) {
            const size_t codeStart = matchPos + startStep;
            const size_t codeEnd = pos - endStep;

            std::string inner = queue.substr(codeStart, codeEnd - codeStart);
            const std::string replaced = handler(inner);

            out += queue.substr(startX, matchPos - startX);
            out += replaced;

            startX = pos;
            continue;
        }

        pos += symbol.size();
    }

    out += queue.substr(startX);
    return out;
}

// ===========================
// splitRule("&&", "||") 等
// ===========================
std::vector<std::string> RuleAnalyzer::splitRule(const std::vector<std::string> &split) {
    elementsType = split[0];

    if (!consumeTo(elementsType)) {
        rule.push_back(queue.substr(startX));
        return rule;
    }

    step = static_cast<int>(elementsType.size());
    splitRuleRec(split);
    return rule;
}

void RuleAnalyzer::splitRuleRec(const std::vector<std::string> &split) {
    while (consumeTo(elementsType)) {
        const size_t end = pos;

        std::string part = queue.substr(startX, end - startX);
        rule.push_back(part);

        pos += step;
        startX = pos;
    }

    rule.push_back(queue.substr(startX));
}

SourceRule::SourceRule(
        AnalyzeRule &_outer,
        const std::string &ruleStr,
        const Mode mode): outer(_outer), mode(mode) {
    if (mode == Js || mode == Regex) {
        rule = ruleStr;
    } else if (StringUtils::startsWithIgnoreCase(ruleStr, "@CSS")) {
        this->mode = Default;
        rule = ruleStr;
    } else if (ruleStr.starts_with("@@")) {
        this->mode = Default;
        rule = ruleStr.substr(2);
    } else if (StringUtils::startsWithIgnoreCase(ruleStr, "@XPath")) {
        this->mode = XPath;
        rule = ruleStr.substr(7);
    } else if (StringUtils::startsWithIgnoreCase(ruleStr, "@Json")) {
        this->mode = Json;
        rule = ruleStr.substr(6);
    } else if (outer.isJSON || ruleStr.starts_with("$.") || ruleStr.starts_with("$[")) {
        this->mode = Json;
        rule = ruleStr;
    } else if (ruleStr.starts_with("/")) {      // XPath特征很明显,无需配置单独的识别标头
        this->mode = XPath;
        rule = ruleStr;
    } else {
        rule = ruleStr;
    }
}

AnalyzeUrl::AnalyzeUrl(
    std::string _mUrl,
    std::optional<std::string> _key,
    std::optional<int> _page,
    std::optional<std::string> _speakText,
    std::optional<int> _speakSpeed,
    std::string _baseUrl,
    BaseSource *_source,
    RuleDataInterface *_ruleData,
    BookChapter *_chapter,
    std::optional<long> _readTimeout,
    std::optional<long> _callTimeout,
    std::optional<std::unordered_map<std::string, std::string> > _headerMapF,
    bool _hasLoginHeader
) : mUrl(std::move(_mUrl)),
    key(std::move(_key)),
    page(_page),
    speakText(std::move(_speakText)),
    speakSpeed(_speakSpeed),
    baseUrl(std::move(_baseUrl)),
    source(std::move(_source)),
    ruleData(std::move(_ruleData)),
    chapter(std::move(_chapter)),
    readTimeout(_readTimeout),
    callTimeout(_callTimeout),
    headerMapF(std::move(_headerMapF)), hasLoginHeader(_hasLoginHeader) {
    std::smatch urlMatcher;
    if (std::regex_search(baseUrl, urlMatcher, paramPattern)) {
        baseUrl = baseUrl.substr(0, urlMatcher.position());
    }

    // 先得到最终的 header map（可能来自 headerMapF，也可能来自 source）
    std::unordered_map<std::string, std::string> finalHeaderMap;
    if (headerMapF.has_value()) {
        finalHeaderMap = *headerMapF; // 使用传入的 headerMapF
    } else if (this->source) {
        finalHeaderMap = source->getHeaderMap(hasLoginHeader);
    }
    if (!finalHeaderMap.empty()) {
        // copy into headerMap
        for (const auto &[fst, snd]: finalHeaderMap) {
            headerMap[fst] = snd;
        }
        // 处理 proxy
        if (auto it = finalHeaderMap.find("proxy"); it != finalHeaderMap.end()) {
            proxy = it->second; // 保存 proxy
            headerMap.erase("proxy"); // 删除 headerMap 中的 proxy
        }
    }
    initUrl();
}

std::string AnalyzeUrl::evalJS(const std::string &jsStr, const std::optional<std::string> &result) {
    auto &engine = QuickJsEngine::current();
    engine.reset();
    engine.addObjectBinding("java", this);
    engine.addValue("baseUrl", baseUrl);
    if (page.has_value()) {
        engine.addValue("page", *page);
    }
    if (key.has_value()) {
        engine.addValue("key", *key);
    }
    if (speakText.has_value()) {
        engine.addValue("speakText", *speakText);
    }
    if (speakSpeed.has_value()) {
        engine.addValue("speakSpeed", *speakSpeed);
    }
    engine.addObjectBinding("book", ruleData);
    engine.addObjectBinding("source", source);
    if (result.has_value()) {
        engine.addValue("result", *result);
    }
    return engine.eval(jsStr);
}

void AnalyzeUrl::initUrl() {
    ruleUrl = mUrl;
    analyzeJs();
    replaceKeyPageJs();
    analyzeUrl();
}

/**
* 执行@js,<js></js>
*/
void AnalyzeUrl::analyzeJs() {
    size_t start = 0;
    std::smatch match;
    std::string result = ruleUrl;
    std::string text = ruleUrl;
    // 按顺序查找所有 <js>...</js> 或 @js:...
    while (std::regex_search(text, match, Constants::JS_PATTERN)) {
        size_t matchStart = start + match.position();
        size_t matchEnd = start + match.position() + match.length();
        // 处理匹配前的普通文字
        if (matchStart > start) {
            std::string before = ruleUrl.substr(start, matchStart - start);
            StringUtils::trim(before);
            if (!before.empty()) {
                result = replaceAll(before, "@result", result);
            }
        }
        // 获取 JS 代码
        std::string jsCode;
        if (match[2].matched) {
            jsCode = match[2].str(); // @js:xxxx
        } else {
            jsCode = match[1].str(); // <js>xxxx</js>
        }
        // 执行 JS
        std::string jsResult = evalJS(jsCode, result);
        result = jsResult;
        start = matchEnd;
        text = ruleUrl.substr(start);
    }
    if (ruleUrl.length() > start) {
        std::string tail = ruleUrl.substr(start);
        StringUtils::trim(tail);
        if (!tail.empty()) {
            result = replaceAll(tail, "@result", result);
        }
    }
    ruleUrl = result;
}

/**
 * 替换url中关于key、page的规则为实际的字符串
 */
void AnalyzeUrl::replaceKeyPageJs() {
    // 这里会将ruleUrl中的全部{{ js }}中的内嵌规则替换掉实际的值
    if (ruleUrl.find("{{") != std::string::npos &&
        ruleUrl.find("}}") != std::string::npos) {
        RuleAnalyzer analyzer(ruleUrl);
        std::string processedUrl = analyzer.innerRule(
            "{{", "}}",
            [&](const std::string &jsCode) -> std::string {
                std::string jsEval = evalJS(jsCode);
                // 先尝试将jsEval转为double，接着将double转为整数字符串
                // 如果转换失败，则直接使用jsEval作为处理结果
                try {
                    if (const double d = std::stod(jsEval);
                        std::fabs(d - std::round(d)) < 1e-9) {
                        const long long intVal = std::llround(d);
                        jsEval = std::to_string(intVal);
                    }
                } catch (...) {
                } // 不是数字，则保持原样
                return jsEval;
            }
        );
        if (!processedUrl.empty()) {
            ruleUrl = processedUrl;
        }
    }
    // 处理分页规则："<a,b,c>"，如搜索结果的分页
    if (page.has_value()) {
        std::string newRule = ruleUrl;
        std::smatch match;
        std::string text = ruleUrl;
        // 非贪婪匹配: pagePattern = <.*?>，并且只处理匹配成功的第一个子串
        while (std::regex_search(text, match, pagePattern)) {
            std::string group = match[0].str(); // "<a,b,c>"
            std::string inner = group.substr(1, group.size() - 2); // 去掉 < >
            // 按逗号分割
            std::vector<std::string> pages;
            splitByComma(inner, pages);

            int p = page.value();
            std::string replacement;

            // 对于越界的page约束到"<a,b,c>"中最后一个
            if (p <= static_cast<int>(pages.size())) {
                replacement = trimCopy(pages[p - 1]);
            } else {
                replacement = trimCopy(pages.back());
            }

            if (size_t pos = newRule.find(group); pos != std::string::npos) {
                newRule.replace(pos, group.size(), replacement);
            }
            text = match.suffix();
        }
        ruleUrl = newRule;
    }
}

void AnalyzeUrl::analyzeUrl() {
    // 在之前的处理中，已经替换掉了额外的内容，接下来要处理的是形如：https://www.qidian.com/so/斗破.html,{"webView": true} 的字符串
    std::smatch m;
    std::string urlNoOption; // 实际上就是取逗号之前的部分作为没有参数的url，如：https://www.qidian.com/so/斗破.html
    if (std::regex_search(ruleUrl, m, paramPattern)) {
        urlNoOption = ruleUrl.substr(0, m.position());
    } else {
        urlNoOption = ruleUrl;
    }
    url = NetworkUtils::getAbsoluteURL(baseUrl, urlNoOption);
    const auto curBaseUrl = NetworkUtils::getBaseUrl(url);
    if (curBaseUrl.has_value()) {
        baseUrl = curBaseUrl.value();
    }
    // 前后长度发生变化，说明可能存在额外参数
    if (urlNoOption.size() != ruleUrl.size()) {
        std::string urlOptionStr = ruleUrl.substr(m.position() + 1);
        // TODO: 继续处理
    }
}

#include <curl/curl.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    auto* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// 封装函数：输入 URL，返回响应内容
std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // 如需忽略 HTTPS 证书错误（可选）
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        // 如果失败，返回空字符串，也可以抛异常或返回错误信息
        return "";
    }

    return response;
}

StrResponse AnalyzeUrl::getStrResponse(
        std::string *jsStr ,
        std::string *sourceRegex,
        bool useWebView
    ) {
    auto res = httpGet(url);
    return StrResponse(url, res);
}