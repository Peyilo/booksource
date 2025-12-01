#pragma once
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>
#include <algorithm>

class RuleDataInterface {
protected:
    std::unordered_map<std::string, std::string> variableMap;

public:
    virtual ~RuleDataInterface() = default;

    // 需要子类实现：用于保存/移除大变量（例如存入文件）
    virtual void putBigVariable(const std::string &key, const std::optional<std::string> &value) = 0;

    // 需要子类实现：获取大变量
    virtual std::optional<std::string> getBigVariable(const std::string &key) const = 0;

    // 根据value的大小选择不同的保存方式
    virtual bool putVariable(const std::string &key, const std::optional<std::string> &value);

    // 优先小变量，再大变量，否则 ""
    std::string getVariable(const std::string &key) const;
};

// 无论大小变量均存储在哈希表中
class RuleData : public RuleDataInterface {
public:
    ~RuleData() override = default;

    // 大变量直接存入 variableMap
    void putBigVariable(const std::string &key, const std::optional<std::string> &value) override;

    // 从 variableMap 读取大变量
    std::optional<std::string> getBigVariable(const std::string &key) const override;
};

static bool isBlank(const std::string& s) {
    return std::ranges::all_of(s, [](unsigned char c) {
        return std::isspace(c);
    });
}


// 分割字符串（支持多个分隔符）
static std::vector<std::string> splitNotBlank(const std::string& str, const std::string& delims) {
    std::vector<std::string> result;
    std::string token;

    for (const char c : str) {
        if (delims.find(c) != std::string::npos) {
            // 遇到分隔符
            if (!token.empty() && !isBlank(token)) {
                result.push_back(token);
            }
            token.clear();
        } else {
            token.push_back(c);
        }
    }

    // 处理最后一个 token
    if (!token.empty() && !isBlank(token)) {
        result.push_back(token);
    }

    return result;
}

class BaseBook: RuleData {
    std::string name;
    std::string author;
    std::string bookUrl;
    std::optional<std::string> kind;
    std::optional<std::string> wordCount;
    std::optional<std::string> variable;
    std::optional<std::string> infoHtml;
    std::optional<std::string> tocHtml;
public:
    bool putVariable(const std::string &key, const std::optional<std::string> &value) override;

    void putCustomVariable(const std::optional<std::string> &value) {
        putVariable("custom", value);
    }

    std::string getCustomVariable() const {
        return getVariable("custom");
    }

    std::vector<std::string> getKindList() const {
        std::vector<std::string> kindList;
        if (wordCount.has_value() && isBlank(wordCount.value())) {
            kindList.push_back(wordCount.value());
        }
        if (kind.has_value()) {
            auto kinds = splitNotBlank(kind.value(), ",\n");
            kindList.insert(kindList.end(), kinds.begin(), kinds.end());
        }
        return kindList;
    }
};

class BookChapter final : RuleData {
public:
    std::string url = "";
    std::string title = "";
    bool isVolume = false;
    std::string baseUrl = "";
    std::string bookUrl = "";
    int index = 0;
    bool isVip = false;
    bool isPay = false;

    std::optional<std::string> resourceUrl = std::nullopt;
    std::optional<std::string> tag = std::nullopt;
    std::optional<std::string> wordCount = std::nullopt;
    std::optional<long> start = std::nullopt;
    std::optional<long> end = std::nullopt;
    std::optional<std::string> startFragmentId = std::nullopt;
    std::optional<std::string> endFragmentId = std::nullopt;

    std::optional<std::string> variable = std::nullopt;
    std::optional<std::string> titleMD5 = std::nullopt;

    explicit BookChapter(
        std::string url = "",
        std::string title = "",
        bool isVolume = false,
        std::string baseUrl = "",
        std::string bookUrl = "",
        int index = 0,
        bool isVip = false,
        bool isPay = false,
        std::optional<std::string> resourceUrl = std::nullopt,
        std::optional<std::string> tag = std::nullopt,
        std::optional<std::string> wordCount = std::nullopt,
        std::optional<long> start = std::nullopt,
        std::optional<long> end = std::nullopt,
        std::optional<std::string> startFragmentId = std::nullopt,
        std::optional<std::string> endFragmentId = std::nullopt,
        std::optional<std::string> variable = std::nullopt
    );

    bool putVariable(const std::string &key, const std::optional<std::string> &value) override;

};