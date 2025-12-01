#include <booksource/ruledata.h>
#include <booksource/utils.h>

using std::string, std::optional;

// 根据value的大小选择不同的保存方式
bool RuleDataInterface::putVariable(const string &key, const optional<string> &value) {
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

string RuleDataInterface::getVariable(const string &key) const {
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

void RuleData::putBigVariable(
    const std::string &key,
    const optional<std::string> &value
) {
    if (value.has_value()) {
        variableMap[key] = value.value();
    } else {
        variableMap.erase(key);
    }
}

optional<std::string> RuleData::getBigVariable(const std::string &key) const {
    if (const auto it = variableMap.find(key); it != variableMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool BaseBook::putVariable(const string &key, const optional<string> &value) {
    if (RuleData::putVariable(key, value)) {
        variable = JsonUtils::mapToJsonString(variableMap);         // map转文本
    }
    return true;
}

BookChapter::BookChapter(
        string url,
        string title,
        bool isVolume,
        string baseUrl,
        string bookUrl,
        int index,
        bool isVip,
        bool isPay,
        optional<string> resourceUrl,
        optional<string> tag,
        optional<string> wordCount,
        optional<long> start,
        optional<long> end,
        optional<string> startFragmentId,
        optional<string> endFragmentId,
        optional<string> variable
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
        variable(std::move(variable)) {}

bool BookChapter::putVariable(const string &key, const optional<string> &value) {
    if (RuleData::putVariable(key, value)) {
        variable = JsonUtils::mapToJsonString(variableMap);         // map转文本
    }
    return true;
}