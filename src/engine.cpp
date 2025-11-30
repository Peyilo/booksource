#include <booksource/engine.h>
#include <iostream>
#include <mutex>

// 定义静态成员
std::atomic<int> QuickJsEngine::s_nextEngineId{1};
std::shared_mutex QuickJsEngine::s_engineRegistryMutex;
std::unordered_map<int, QuickJsEngine*> QuickJsEngine::s_engineRegistry;
thread_local QuickJsEngine QuickJsEngine::currentEngine = QuickJsEngine();

QuickJsEngine::QuickJsEngine() {
    runtime = JS_NewRuntime();
    if (!runtime) throw std::runtime_error("Failed to create JSRuntime");

    context = JS_NewContext(runtime);
    if (!context) {
        JS_FreeRuntime(runtime);
        throw std::runtime_error("Failed to create JSContext");
    }

    // 分配全局唯一的 engineID
    engineID = s_nextEngineId.fetch_add(1, std::memory_order_relaxed);

    // 注册到全局 map
    {
        std::unique_lock lock(s_engineRegistryMutex);
        s_engineRegistry[engineID] = this;
    }

    // 把 engineID 写到 context 的 opaque 里，后面回调可以拿这个 ID 再查 map
    // 注意用 uintptr_t 中转，避免未定义行为
    JS_SetContextOpaque(
        context,
        reinterpret_cast<void*>(static_cast<uintptr_t>(engineID))
    );
}

QuickJsEngine::~QuickJsEngine() {
    // 先从全局注册表里移除
    {
        std::unique_lock lock(s_engineRegistryMutex);
        if (const auto it = s_engineRegistry.find(engineID); it != s_engineRegistry.end())
            s_engineRegistry.erase(it);
    }
    // 清空全部指针
    boundPtrTable.clear();
    nameToPtrIndex.clear();
    nextPtrTableId = 0;
    if (context) JS_FreeContext(context);
    if (runtime) JS_FreeRuntime(runtime);
}

QuickJsEngine* QuickJsEngine::fromContext(JSContext* ctx) {
    if (!ctx) return nullptr;

    void* opaque = JS_GetContextOpaque(ctx);
    if (!opaque) return nullptr;

    // 还原 engineID
    const auto id = static_cast<int>(reinterpret_cast<uintptr_t>(opaque));

    std::shared_lock lock(s_engineRegistryMutex);
    const auto it = s_engineRegistry.find(id);
    if (it == s_engineRegistry.end())
        return nullptr;

    return it->second;
}

QuickJsEngine* QuickJsEngine::getEngineById(const int id) {
    std::shared_lock lock(s_engineRegistryMutex);

    const auto it = s_engineRegistry.find(id);
    if (it == s_engineRegistry.end())
        return nullptr;

    return it->second;
}

QuickJsEngine& QuickJsEngine::current() {
    return currentEngine;
}

std::string QuickJsEngine::eval(const std::string &code) const {
    const JSValue val = JS_Eval(context, code.c_str(), code.size(),
                                "<eval>", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(val)) {
        const JSValue err = JS_GetException(context);
        const char *errStr = JS_ToCString(context, err);

        const std::string message = errStr ? errStr : "Unknown JS error";
        JS_FreeCString(context, errStr);
        JS_FreeValue(context, err);
        JS_FreeValue(context, val);
        throw std::runtime_error(message);
    }

    const char *resStr = JS_ToCString(context, val);
    std::string result = resStr ? resStr : "";

    JS_FreeCString(context, resStr);
    JS_FreeValue(context, val);
    return result;
}

void QuickJsEngine::reset() {
    if (context) JS_FreeContext(context);
    context = JS_NewContext(runtime);

    if (!context) {
        throw std::runtime_error("Failed to recreate JSContext during reset()");
    }

    // 给新的Context设置engineID
    JS_SetContextOpaque(
        context,
        reinterpret_cast<void*>(static_cast<uintptr_t>(engineID))
    );
    // 清空全部指针
    boundPtrTable.clear();
    nameToPtrIndex.clear();
    nextPtrTableId = 0;
}

void QuickJsEngine::addValue(const std::string &name, const std::string &value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue str = JS_NewString(context, value.c_str());
    JS_SetPropertyStr(context, global, name.c_str(), str);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const char *value) const {
    addValue(name, std::string(value));
}

void QuickJsEngine::addValue(const std::string &name, const int value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewInt32(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const double value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue num = JS_NewFloat64(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), num);
    JS_FreeValue(context, global);
}

void QuickJsEngine::addValue(const std::string &name, const bool value) const {
    const JSValue global = JS_GetGlobalObject(context);
    const JSValue b = JS_NewBool(context, value);
    JS_SetPropertyStr(context, global, name.c_str(), b);
    JS_FreeValue(context, global);
}

// 供js中调用的屏幕输出函数
static JSValue js_print(
    JSContext* ctx,
    JSValueConst _,
    const int argc,
    JSValueConst* argv) {
    for (int i = 0; i < argc; i++) {
        const char* str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;

        std::cout << str;
        JS_FreeCString(ctx, str);

        if (i + 1 < argc)
            std::cout << " ";
    }

    std::cout << std::endl;
    return JS_UNDEFINED;
}

void QuickJsEngine::addPrintFunc(const std::string &funcName) const {
    const JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(context,global,
        funcName.c_str(),JS_NewCFunction(context, js_print, funcName.c_str(), 1)
    );
    JS_FreeValue(context, global);
}

// JS 中调用的 assert
static JSValue js_assert(JSContext *ctx,
                         JSValueConst this_val,
                         int argc,
                         JSValueConst *argv) {
    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "assert() requires at least 1 argument");
    }

    if (JS_ToBool(ctx, argv[0])) {
        return JS_UNDEFINED;   // 断言通过
    }

    // 获取错误消息
    const char* msg = nullptr;
    if (argc >= 2) {
        msg = JS_ToCString(ctx, argv[1]);
    }

    JSValue err;
    if (msg) {
        err = JS_ThrowTypeError(ctx, "Assertion failed: %s", msg);
        JS_FreeCString(ctx, msg);
    } else {
        err = JS_ThrowTypeError(ctx, "Assertion failed");
    }
    return err;
}

void QuickJsEngine::addAssertFunc(const std::string &funcName) const {
    if (!context)
        throw std::runtime_error("QuickJsEngine: context is null");

    const JSValue global = JS_GetGlobalObject(context);
    JS_SetPropertyStr(
        context,
        global,
        funcName.c_str(),
        JS_NewCFunction(context, js_assert, funcName.c_str(), 2) // 参数：cond, msg
    );
    JS_FreeValue(context, global);
}

void QuickJsEngine::deleteValue(const std::string &name) {
    if (!context)
        throw std::runtime_error("QuickJsEngine: context is null");

    const JSValue global = JS_GetGlobalObject(context);
    const JSAtom atom = JS_NewAtom(context, name.c_str());
    JS_DeleteProperty(context, global, atom, JS_PROP_THROW);
    JS_FreeAtom(context, atom);
    JS_FreeValue(context, global);

    // 只有addValueBinding()添加的变量才有指针
    if (const auto it = nameToPtrIndex.find(name); it != nameToPtrIndex.end()) {
        const int id = it->second;
        boundPtrTable.erase(id);      // 清除指针
        nameToPtrIndex.erase(it);     // 清除 name-index 映射
    }
}
