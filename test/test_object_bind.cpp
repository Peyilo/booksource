#include <iostream>
#include <string>
#include <booksource/engine.h>
#include <booksource/bind.h>
#include <booksource/rule.h>
#include <cassert>
#include <fstream>
#include "test_utils.h"

struct Metainfo {};

struct Book {
    std::string name;
    std::string author;
    Metainfo metainfo;

    std::string info() const {
        return name + " / " + author;
    }

    int test(int param1, int param2, Book param3) const {
        std::cout << "param1: " << param1 << std::endl;
        std::cout << "param2: " << param2 << std::endl;
        std::cout << "param3: " << param3.name << "; " << param3.author << std::endl;
        return param1 + param2;
    }

};

void initBookClassInfo() {
    JsBinder<Book>::setClassName("Book");
    JsBinder<Book>::addField("name", &Book::name);
    JsBinder<Book>::addField("author", &Book::author);
    JsBinder<Book>::addField("metainfo", &Book::metainfo);
    JsBinder<Book>::addMethod("info", &Book::info);
    JsBinder<Book>::addMethod("test", &Book::test);
}

void exeJs(const QuickJsEngine &engine, const std::string &code) {
    if (const std::string res = engine.eval(code); res != "undefined") {
    std::cout << "[Result] " << res << std::endl;
    }
}

int main() {
    QuickJsEngine engine;
    Binding::initEngineClassInfo();
    engine.addPrintFunc("print");
    engine.addAssertFunc("assert");
    initBookClassInfo();

    Book book{"first book", "first author"};
    Book book2{"second book", "second author"};
    engine.addObjectBinding("book", &book);
    engine.addObjectBinding("book2", &book2);

    // 验证绑定后，能否在js中访问字段和方法
    exeJs(engine, "book.name + ' / ' + book.author");
    exeJs(engine, "book.info()");
    exeJs(engine, "book.test(1, 2, book2)");

    // 验证在js中修改了绑定对象的值以后，cpp中相应的变量是否会修改
    exeJs(engine, "book.name = \"first book (edited)\";");
    exeJs(engine, "book.author = \"first author (edited)\";");
    exeJs(engine, "book.name + ' / ' + book.author");
    assert("first book (edited)" == book.name);
    assert("first author (edited)" == book.author);

    // 验证在cpp中修改了绑定对象的值以后，js中相应的变量是否会修改
    book2.author = "Tolkien";
    exeJs(engine, "book2.info()");
    exeJs(engine, "book2.test(5, 6, book2)");

    // 检查基础类型的绑定是否为双向的
    int num = 123;
    engine.addValueBinding("num", &num);
    exeJs(engine, "num = 456;");
    std::cout << "num: " << num << std::endl;
    num = 789;
    engine.deleteValue("num");
    try {
        exeJs(engine, "num;");
    } catch (const std::exception &e) {}

    auto json = getResourceText("bs1.json");
    auto bookSources = BookSourceParser::parseBookSourceList(json);
    auto source = bookSources[0];
    engine.addObjectBinding("source", &source);
    exeJs(engine, "source.bookSourceName");
    exeJs(engine, "source.bookSourceUrl");

    // 测试在js中修改，会不会影响cpp中的变量
    exeJs(engine, "source.bookSourceUrl = \"booksource url\";");
    assert(source.bookSourceUrl == "booksource url");

    exeJs(engine, "source.bookSourceGroup");
    exeJs(engine, "source.bookSourceType");
    exeJs(engine, "source.enabledCookieJar");
    exeJs(engine, "source.ruleToc");
    exeJs(engine, "source.ruleToc.chapterList");

    // 测试在cpp修改以后，会不会影响js中的变量
    source.ruleToc.value().chapterList = "this is rule after changed";
    exeJs(engine, "assert(source.ruleToc.chapterList == \"this is rule after changed\")");
    return 0;
}
