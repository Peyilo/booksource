#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <chrono>
#include <bitset>
#include <netdb.h>
#include <regex>
#include <booksource/engine.h>
#include <optional>
#include <unordered_set>
#include <ifaddrs.h>
#include <arpa/inet.h>

namespace JsonUtils {
    using nlohmann::json;
    using std::string, std::unordered_map;

    /**
     * 给定一个map，将其内容转为一个json字符串
     * @param map
     * @return
     */
    inline string mapToJsonString(const unordered_map<string, string> &map) {
        const json j = map;
        return j.dump();
    }

    /**
     * 将 json 字符串解析为 unordered_map<string, string>
     * @param jsonStr JSON 格式的字符串
     * @return map，如果解析失败则返回空 map
     */
    inline unordered_map<string, string> jsonStringToMap(const string &jsonStr) {
        unordered_map<string, string> result;
        try {
            json j = json::parse(jsonStr);
            if (!j.is_object()) {
                return result; // 必须是 object
            }
            for (auto &[key, value] : j.items()) {
                if (value.is_string()) {
                    result[key] = value.get<string>();
                } else {
                    // 如果不是 string，转成 string 放进去（可按需求调整）
                    result[key] = value.dump();
                }
            }
        } catch (...) {
            // 如果解析失败，返回空 map
        }
        return result;
    }
}

namespace DateUtils {
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
}


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
        ifaddrs* ifaddr = nullptr;

        if (getifaddrs(&ifaddr) == -1) return results;

        for (ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
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