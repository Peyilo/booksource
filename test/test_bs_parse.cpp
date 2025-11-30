//
// Created by 夏莳 on 2025/11/26.
//

#include <iostream>
#include <booksource/rule.h>
#include "test_utils.h"
#include <curl/curl.h>

// 写入回调：把收到的数据 append 到 std::string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    const size_t totalSize = size * nmemb;
    auto* buffer = static_cast<std::string *>(userp);
    buffer->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

void test_curl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl init failed\n";
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, "https://httpbin.org/get");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: "
                  << curl_easy_strerror(res) << "\n";
    } else {
        std::cout << "Response:\n" << response << "\n";
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

int main() {
    const auto bss1 = BookSourceParser::parseBookSourceList(getResourceText("bs1.json"));
    std::cout << "bss1 size: " << bss1.size() << std::endl;
    const auto bss2 = BookSourceParser::parseBookSourceList(getResourceText("bs2.json"));
    std::cout << "bss2 size: " << bss2.size() << std::endl;

    test_curl();
    return 0;
}