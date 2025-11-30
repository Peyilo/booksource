#pragma once
#include <unordered_map>
#include <string>
#include <optional>

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
class RuleData final : public RuleDataInterface {
public:
    ~RuleData() override = default;

    // 大变量直接存入 variableMap
    void putBigVariable(const std::string &key, const std::optional<std::string> &value) override;

    // 从 variableMap 读取大变量
    std::optional<std::string> getBigVariable(const std::string &key) const override;
};

class BookChapter final : RuleDataInterface {
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

    void putBigVariable(const std::string &key, const std::optional<std::string> &value) override {
        // TODO: 未实现
    }

    std::optional<std::string> getBigVariable(const std::string &key) const override {
        // TODO: 未实现
        return std::nullopt;
    }
};