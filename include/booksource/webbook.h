#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <booksource/booksource.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <regex>
#include <booksource/engine.h>
#include <optional>
#include <unordered_set>
#include <bitset>
#include <algorithm>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

using LocalDate = std::chrono::year_month_day;

inline LocalDate generateLocalDate(
    const int year, const unsigned month, const unsigned day
) {
    const auto date = LocalDate{
        std::chrono::year{year},
        std::chrono::month{month},
        std::chrono::day{day}
    };
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date");
    }
    return date;
}

inline LocalDate defaultLocalDate() {
    return generateLocalDate(1970, 1, 1);
}

// 获取当前的时间戳：毫秒ms
inline long currentTimeMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

namespace BookType {
    constexpr int text = 0b1000; // 8
    constexpr int updateError = 0b10000; // 16
    constexpr int audio = 0b100000; // 32
    constexpr int image = 0b1000000; // 64
    constexpr int webFile = 0b10000000; // 128
    constexpr int local = 0b100000000; // 256
    constexpr int archive = 0b1000000000; // 512
    constexpr int notShelf = 0b10000000000; // 1024

    constexpr int allBookType = text | image | audio | webFile;
    constexpr int allBookTypeLocal = allBookType | local;

    // 字符串常量
    constexpr auto localTag = "loc_book";
    constexpr auto webDavTag = "webDav::";
}

struct ReadConfig {
    bool reverseToc = false;
    int pageAnim = -1;
    bool reSegment = false;
    string imageStyle;
    bool useReplaceRule = false;
    long delTag = 0L;
    string ttsEngine;
    bool splitLongChapter = true;
    bool readSimulating = false;
    LocalDate startDate = defaultLocalDate();
    int startChapter = -1;
    int dailyChapters = 3;
};

struct Book {
    // 详情页Url(本地书源存储完整文件路径)
    string bookUrl;
    // 目录页Url (toc=table of Contents)
    string tocUrl;
    // 书源URL(默认BookType.localTag)
    string origin = BookType::localTag; // 需要你自己替换 BookType.localTag 对应字符串
    // 书源名称 or 本地书籍文件名
    string originName;
    // 书籍名称(书源获取)
    string name;
    // 作者名称(书源获取)
    string author;
    // 分类信息(书源获取)
    string kind;
    // 分类信息(用户修改)
    string customTag;
    // 封面Url(书源获取)
    string coverUrl;
    // 封面Url(用户修改)
    string customCoverUrl;
    // 简介内容(书源获取)
    string intro;
    // 简介内容(用户修改)
    string customIntro;
    // 自定义字符集名称(仅适用于本地书籍)
    string charset;
    // 类型
    int type = 0; // BookType.text → 你需要自己定义枚举
    // 自定义分组索引号
    long group = 0;
    // 最新章节标题
    string latestChapterTitle;
    // 最新章节标题更新时间
    long latestChapterTime = currentTimeMillis();
    // 最近一次更新书籍信息的时间
    long lastCheckTime = currentTimeMillis();
    // 最近一次发现新章节的数量
    int lastCheckCount = 0;
    // 书籍目录总数
    int totalChapterNum = 0;
    // 当前章节名称
    string durChapterTitle;
    // 当前章节索引
    int durChapterIndex = 0;
    // 当前阅读的进度(首行字符的索引位置)
    int durChapterPos = 0;
    // 最近一次阅读书籍的时间
    long durChapterTime = currentTimeMillis();
    // 字数
    string wordCount;
    // 刷新书架时更新书籍信息
    bool canUpdate = true;
    // 手动排序
    int order = 0;
    // 书源排序
    int originOrder = 0;
    // 自定义书籍变量信息
    string variable;
    // 阅读设置
    ReadConfig readConfig; // Kotlin 是 nullable，但你不想 optional → 内嵌结构体
    // 同步时间
    long syncTime = 0;

    string infoHtml;
    string tocHtml;
    std::vector<string> downloadUrls;
};

struct SearchBook {
    string bookUrl;
    // 书源
    string origin;
    string originName;
    // BookType
    int type = BookType::text;
    string name;
    string author;
    string kind;
    string coverUrl;
    string intro;
    string wordCount;
    string latestChapterTitle;

    /** 目录页Url (toc=table of Contents) */
    string tocUrl;

    long time = currentTimeMillis();
    string variable;
    int originOrder = 0;
    string chapterWordCountText;
    int chapterWordCount = -1;
    int respondTime = -1;

    string infoHtml;
    string tocHtml;
};

class RuleDataInterface {
public:
    std::unordered_map<string, string> variableMap;

    virtual ~RuleDataInterface() = default;

    // 需要子类实现：用于保存/移除大变量（例如存入文件）
    virtual void putBigVariable(const string &key, const optional<string> &value) = 0;

    // 需要子类实现：获取大变量
    virtual optional<string> getBigVariable(const string &key) const = 0;

    // 根据value的大小选择不同的保存方式
    virtual bool putVariable(const string &key, const optional<string> &value) {
        const bool keyExist = variableMap.contains(key);

        // value == nullopt，移出所有存储：小变量和大变量，可用于释放空间
        if (!value) {
            variableMap.erase(key);
            putBigVariable(key, std::nullopt);
            return keyExist;
        }
        const std::string &val = value.value();

        // 小变量存储：value长度小于10000字符
        if (value->size() < 10000) {
            putBigVariable(key, std::nullopt);
            variableMap[key] = val;
            return true;
        }

        // 大变量存储：value长度大于等于10000字符
        variableMap.erase(key);
        putBigVariable(key, val);
        return keyExist;
    }

    // 优先小变量，再大变量，否则 ""
    string getVariable(const string &key) const {
        // 小变量
        if (const auto it = variableMap.find(key); it != variableMap.end()) {
            return it->second;
        }
        // 大变量
        if (auto big = getBigVariable(key); big.has_value()) {
            return big.value();
        }
        return "";
    }
};

// 无论大小变量均存储在哈希表中
class RuleData final : public RuleDataInterface {
public:
    ~RuleData() override = default;

    // 大变量直接存入 variableMap
    void putBigVariable(const std::string &key,
                        const optional<std::string> &value) override {
        if (value.has_value()) {
            variableMap[key] = value.value();
        } else {
            variableMap.erase(key);
        }
    }

    // 从 variableMap 读取大变量
    optional<std::string> getBigVariable(const std::string &key) const override {
        if (const auto it = variableMap.find(key); it != variableMap.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};


class BookChapter final : RuleDataInterface {
public:
    string url = "";
    string title = "";
    bool isVolume = false;
    string baseUrl = "";
    string bookUrl = "";
    int index = 0;
    bool isVip = false;
    bool isPay = false;

    optional<string> resourceUrl = std::nullopt;
    optional<string> tag = std::nullopt;
    optional<string> wordCount = std::nullopt;
    optional<long> start = std::nullopt;
    optional<long> end = std::nullopt;
    optional<string> startFragmentId = std::nullopt;
    optional<string> endFragmentId = std::nullopt;

    optional<string> variable = std::nullopt;
    optional<string> titleMD5 = std::nullopt;

    explicit BookChapter(
        string url = "",
        string title = "",
        bool isVolume = false,
        string baseUrl = "",
        string bookUrl = "",
        int index = 0,
        bool isVip = false,
        bool isPay = false,
        optional<string> resourceUrl = std::nullopt,
        optional<string> tag = std::nullopt,
        optional<string> wordCount = std::nullopt,
        optional<long> start = std::nullopt,
        optional<long> end = std::nullopt,
        optional<string> startFragmentId = std::nullopt,
        optional<string> endFragmentId = std::nullopt,
        optional<string> variable = std::nullopt
    ) : url(std::move(url)),
        title(std::move(title)),
        isVolume(isVolume),
        baseUrl(std::move(baseUrl)),
        bookUrl(std::move(bookUrl)),
        index(index),
        isVip(isVip),
        isPay(isPay),
        resourceUrl(std::move(resourceUrl)),
        tag(std::move(tag)),
        wordCount(std::move(wordCount)),
        start(start),
        end(end),
        startFragmentId(std::move(startFragmentId)),
        endFragmentId(std::move(endFragmentId)),
        variable(std::move(variable)) {
    }

    bool putVariable(const string &key, const optional<string> &value) override {
        if (RuleDataInterface::putVariable(key, std::move(value))) {
            variable = toJson(variableMap); // map转文本
        }
        return true;
    }

    void putBigVariable(const string &key, const optional<string> &value) override {
        // TODO: 未实现
    }

    optional<string> getBigVariable(const string &key) const override {
        // TODO: 未实现
        return std::nullopt;
    }

private:
    static std::string toJson(const std::unordered_map<std::string, std::string> &map) {
        const nlohmann::json j = map;
        return j.dump(); // 生成 JSON 字符串
    }
};


class StrResponse {
public:
};

enum RequestMethod {
    GET, POST
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

    RuleAnalyzer(const std::string &data, bool code = false)
        : queue(data), codeMode(code) {
    }

    void reSetPos() {
        pos = 0;
        startX = 0;
    }

private:
    // consumeTo — 查找 seq
    bool consumeTo(const std::string &seq) {
        start = pos;
        size_t offset = queue.find(seq, pos);
        if (offset == std::string::npos) return false;
        pos = offset;
        return true;
    }

    // consumeToAny — 查找多个 seq
    bool consumeToAny(const std::vector<std::string> &seq) {
        size_t p = pos;

        while (p < queue.size()) {
            for (const auto &s: seq) {
                if (queue.compare(p, s.size(), s) == 0) {
                    step = (int) s.size();
                    pos = p;
                    return true;
                }
            }

            p++;
        }
        return false;
    }

    // 查找任一字符
    int findToAny(const std::vector<char> &chars) {
        size_t p = pos;
        while (p < queue.size()) {
            for (char c: chars) {
                if (queue[p] == c) return (int) p;
            }
            p++;
        }
        return -1;
    }

    // 平衡检查（用于 {...} / (...) 等）
    bool chompBalanced(char open, char close) {
        int depth = 0;
        bool inSingle = false;
        bool inDouble = false;
        size_t p = pos;

        while (p < queue.size()) {
            char c = queue[p];
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

public:
    // ===========================
    //  innerRule("[[", "]]", handler)
    // ===========================
    std::string innerRule(
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
    std::string innerRuleCode(
        const std::string &symbol,
        const int startStep,
        const int endStep,
        const std::function<std::string(const std::string &)> &handler
    ) {
        std::string out;

        while (consumeTo(symbol)) {
            const size_t matchPos = pos;

            if (chompBalanced('{', '}')) {
                size_t codeStart = matchPos + startStep;
                size_t codeEnd = pos - endStep;

                std::string inner = queue.substr(codeStart, codeEnd - codeStart);
                std::string replaced = handler(inner);

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
    std::vector<std::string> splitRule(const std::vector<std::string> &split) {
        elementsType = split[0];

        if (!consumeTo(elementsType)) {
            rule.push_back(queue.substr(startX));
            return rule;
        }

        step = (int) elementsType.size();
        splitRuleRec(split);
        return rule;
    }

private:
    // 递归切分
    void splitRuleRec(const std::vector<std::string> &split) {
        while (consumeTo(elementsType)) {
            size_t end = pos;

            std::string part = queue.substr(startX, end - startX);
            rule.push_back(part);

            pos += step;
            startX = pos;
        }

        rule.push_back(queue.substr(startX));
    }
};

namespace NetworkUtils {
    using std::string;

    // ------------------------------
    // Helper: 判断是否是十六进制字符
    // ------------------------------
    inline bool isDigit16Char(char c) {
        return (c >= '0' && c <= '9') ||
               (c >= 'A' && c <= 'F') ||
               (c >= 'a' && c <= 'f');
    }

    // ---------------------------------------
    // Kotlin 中的 BitSet → C++ 转成 std::bitset
    // ---------------------------------------
    inline std::bitset<256> buildAllowedQuery() {
        std::bitset<256> bs;
        for (char c = 'a'; c <= 'z'; c++) bs.set(static_cast<unsigned char>(c));
        for (char c = 'A'; c <= 'Z'; c++) bs.set(static_cast<unsigned char>(c));
        for (char c = '0'; c <= '9'; c++) bs.set(static_cast<unsigned char>(c));

        auto extra = "!$&()*+,-./:;=?@[\\]^_`{|}~";
        while (*extra) {
            bs.set(static_cast<unsigned char>(*extra));
            ++extra;
        }
        return bs;
    }

    inline std::bitset<256> buildAllowedForm() {
        std::bitset<256> bs;
        for (char c = 'a'; c <= 'z'; c++) bs.set(static_cast<unsigned char>(c));
        for (char c = 'A'; c <= 'Z'; c++) bs.set(static_cast<unsigned char>(c));
        for (char c = '0'; c <= '9'; c++) bs.set(static_cast<unsigned char>(c));

        auto extra = "*-._";
        while (*extra) {
            bs.set(static_cast<unsigned char>(*extra));
            ++extra;
        }
        return bs;
    }

    static const std::bitset<256> notNeedEncodingQuery = buildAllowedQuery();
    static const std::bitset<256> notNeedEncodingForm  = buildAllowedForm();

    // ----------------------------------------------------
    // 判断 Query 是否已编码 (encodedQuery)
    // ----------------------------------------------------
    inline bool encodedQuery(const string& str) {
        size_t i = 0;
        while (i < str.size()) {
            char c = str[i];
            if (notNeedEncodingQuery[static_cast<unsigned char>(c)]) {
                i++;
                continue;
            }
            if (c == '%' && i + 2 < str.size()) {
                const char c1 = str[++i];
                const char c2 = str[++i];
                if (isDigit16Char(c1) && isDigit16Char(c2)) {
                    i++;
                    continue;
                }
            }
            return false;  // 不合法，需要 encode
        }
        return true;
    }

    // ------------------------------------------------
    // 判断 form 是否已编码 (encodedForm)
    // ------------------------------------------------
    inline bool encodedForm(const string& str) {
        size_t i = 0;
        while (i < str.size()) {
            char c = str[i];
            if (notNeedEncodingForm[static_cast<unsigned char>(c)]) {
                i++;
                continue;
            }
            if (c == '%' && i + 2 < str.size()) {
                char c1 = str[++i];
                char c2 = str[++i];
                if (isDigit16Char(c1) && isDigit16Char(c2)) {
                    i++;
                    continue;
                }
            }
            return false;
        }
        return true;
    }

    // ---------------------------------------------------
    // 判断是否为绝对 URL
    // ---------------------------------------------------
    inline bool isAbsUrl(const string& url) {
        return url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0;
    }

    inline bool isDataUrl(const string& url) {
        return url.rfind("data:", 0) == 0;
    }

    // ---------------------------------------------------
    // 获取 Base URL（例: https://abc.com → https://abc.com）
    // ---------------------------------------------------
    inline std::optional<string> getBaseUrl(const string& url) {
        if (!isAbsUrl(url)) return std::nullopt;
        const size_t pos = url.find('/', 9);
        if (pos == string::npos) return url;
        return url.substr(0, pos);
    }

    // --------------------------------------------------------
    // 判断是否是 IP 地址
    // --------------------------------------------------------
    inline bool isIPv4(const string& s) {
        struct sockaddr_in sa{};
        return inet_pton(AF_INET, s.c_str(), &(sa.sin_addr)) == 1;
    }

    inline bool isIPv6(const string& s) {
        struct sockaddr_in6 sa{};
        return inet_pton(AF_INET6, s.c_str(), &(sa.sin6_addr)) == 1;
    }

    inline bool isIPAddress(const string& s) {
        return isIPv4(s) || isIPv6(s);
    }

    // --------------------------------------------------------
    // 获取主域名 getSubDomain（简化，不含 PublicSuffix）
    // --------------------------------------------------------
    inline string getDomain(const string& url) {
        const auto base = getBaseUrl(url);
        if (!base) return url;

        const std::regex hostRegex(R"(https?://([^/]+))");
        std::smatch m;
        if (!std::regex_search(base.value(), m, hostRegex)) return url;

        return m[1].str();
    }

    inline string getSubDomain(const string& url) {
        string host = getDomain(url);

        if (isIPAddress(host)) return host;

        // 简化的 “有效顶级域名 + 一级域名”
        // 例如 www.a.b.c.com → c.com
        const auto pos = host.rfind('.');
        if (pos == string::npos) return host;

        const auto pos2 = host.rfind('.', pos - 1);
        if (pos2 == string::npos) return host;

        return host.substr(pos2 + 1);
    }

    // --------------------------------------------------------
    // 获取绝对地址 getAbsoluteURL
    // --------------------------------------------------------
    inline string getAbsoluteURL(const string& baseUrl, const string& relative) {
        string rel = relative;
        rel.erase(0, rel.find_first_not_of(" \t\r\n"));

        if (baseUrl.empty()) return rel;
        if (isAbsUrl(rel)) return rel;
        if (isDataUrl(rel)) return rel;
        if (rel.rfind("javascript", 0) == 0) return "";

        const auto base = getBaseUrl(baseUrl);
        if (!base) return rel;

        // 构造 URL (简单处理)
        if (rel[0] == '/') {
            return base.value() + rel;
        }
        return base.value() + "/" + rel;
    }

    // --------------------------------------------------------
    // 获取本机 IPv4 地址
    // --------------------------------------------------------
    inline std::vector<string> getLocalIPAddress() {
        std::vector<string> results;
        struct ifaddrs* ifaddr = nullptr;

        if (getifaddrs(&ifaddr) == -1) return results;

        for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) continue;

            if (ifa->ifa_addr->sa_family == AF_INET) {
                char buf[INET_ADDRSTRLEN];
                const auto sin = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);

                if (!inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf)))
                    continue;

                // 跳过 127.0.0.1
                if (string ip = buf; ip != "127.0.0.1")
                    results.push_back(ip);
            }
        }

        freeifaddrs(ifaddr);
        return results;
    }

    // 始终返回 true
    inline bool isAvailable() { return true; }
}

class AnalyzeUrl : JsExtensions {
private:
    string mUrl;
    optional<string> key = std::nullopt;
    optional<int> page = std::nullopt;
    optional<string> speakText = std::nullopt;
    optional<int> speakSpeed = std::nullopt;
    string baseUrl;
    BaseSource *source = nullptr;
    RuleDataInterface *ruleData = nullptr;;
    BookChapter *chapter = nullptr;
    optional<long> readTimeout = std::nullopt;
    optional<long> callTimeout = std::nullopt;
    optional<std::unordered_map<string, string> > headerMapF = std::nullopt;
    bool hasLoginHeader = true;

    string ruleUrl;
    string url;
    optional<string> type = std::nullopt;
    std::unordered_map<string, string> headerMap;

    optional<string> body = std::nullopt;
    string urlNoQuery;
    optional<string> encodedForm = std::nullopt;
    optional<string> encodedQuery = std::nullopt;
    optional<string> charset = std::nullopt;
    RequestMethod method = GET;
    optional<string> proxy = std::nullopt;
    int retry = 0;
    bool useWebView = false;
    optional<string> webJs = std::nullopt;
    bool enabledCookieJar = false;
    string domain;
    long webViewDelayTime = 0;

    optional<long> serverID = std::nullopt;

private:
    std::regex paramPattern{R"(\s*,\s*(?=\{))"};    // 匹配 http://xx.xx/xxxx , { "method": "POST" } 中的逗号
    std::regex pagePattern{R"(<.*?>)"};             // <a, b, c>

public:
    explicit AnalyzeUrl(
        string _mUrl,
        optional<string> _key = std::nullopt,
        optional<int> _page = std::nullopt,
        optional<string> _speakText = std::nullopt,
        optional<int> _speakSpeed = std::nullopt,
        string _baseUrl = "",
        BaseSource *_source = nullptr,
        RuleDataInterface *_ruleData = nullptr,
        BookChapter *_chapter = nullptr,
        optional<long> _readTimeout = std::nullopt,
        optional<long> _callTimeout = std::nullopt,
        optional<std::unordered_map<string, string> > _headerMapF = std::nullopt,
        bool _hasLoginHeader = true
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

    std::string evalJS(const std::string &jsStr, const optional<string> &result = std::nullopt) {
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

public:
    StrResponse getStrResponse(
        string *jsStr = nullptr,
        string *sourceRegex = nullptr,
        bool useWebView = false
    ) {
        return StrResponse();
    }

    BaseSource *getSource() override {
        return source;
    }

private:
    void initUrl() {
        ruleUrl = mUrl;
        analyzeJs();
        replaceKeyPageJs();
        analyzeUrl();
    }

    /**
    * 执行@js,<js></js>
    */
    void analyzeJs() {
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
                trim(before);
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
            trim(tail);
            if (!tail.empty()) {
                result = replaceAll(tail, "@result", result);
            }
        }
        ruleUrl = result;
    }

    /**
     * 替换url中关于key、page的规则为实际的字符串
     */
    void replaceKeyPageJs() {
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

    void analyzeUrl() {
        // 在之前的处理中，已经替换掉了额外的内容，接下来要处理的是形如：https://www.qidian.com/so/斗破.html,{"webView": true} 的字符串
        std::smatch m;
        std::string urlNoOption;  // 实际上就是取逗号之前的部分作为没有参数的url，如：https://www.qidian.com/so/斗破.html
        if (std::regex_search(ruleUrl, m, paramPattern)) {
            urlNoOption = ruleUrl.substr(0, m.position());
        } else {
            urlNoOption = ruleUrl;
        }
        url = NetworkUtils::getAbsoluteURL(baseUrl, urlNoOption);

    }

    static void trim(std::string &s) {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::ranges::find_if(s, notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
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
};

namespace WebBook {
    using BookFilter = std::function<bool(const string &name, const string &author)>;

    using BreakCondition = std::function<bool(int size)>;

    std::vector<SearchBook> searchBook(
        BookSource &bookSource,
        const string &key,
        optional<int> page = 1,
        const optional<BookFilter> &filter = std::nullopt,
        const optional<BreakCondition> &shouldBreak = std::nullopt
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
        return std::vector<SearchBook>{};
    }
}
