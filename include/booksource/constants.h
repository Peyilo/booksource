#pragma once
#include <string>
#include <regex>

namespace Constants {

    const std::string UA_KEY = "User-Agent";
    const std::string UA_DEFAULT_VALUE = "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36"
        "(KHTML, like Gecko) Chrome/142.0.0.0 Mobile Safari/537.36";

    const std::regex JS_PATTERN{R"(<js>([\w\W]*?)</js>|@js:([\w\W]*))", std::regex::icase};


}

