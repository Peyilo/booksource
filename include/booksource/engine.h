#pragma once

#include <shared_mutex>
#include <quickjs/quickjs.h>
#include <string>
#include <functional>
#include <vector>
#include <stdexcept>
#include <atomic>

template<typename T>
struct JSConverter;

template<typename T>
struct MethodTraits;

template<typename T>
struct FieldTraits;

template<typename T>
class JsBinder;

class QuickJsEngine {
public:
    QuickJsEngine();

    ~QuickJsEngine();

    /// 执行脚本并返回字符串结果
    std::string eval(const std::string &code) const;

    /// 重新创建上下文（清空状态）
    void reset();

    // addValue()只是往js环境中注入了一个变量，并没有与cpp变量完成绑定
    void addValue(const std::string &name, const std::string &value) const;

    void addValue(const std::string &name, const char *value) const;

    void addValue(const std::string &name, int value) const;

    void addValue(const std::string &name, double value) const;

    void addValue(const std::string &name, bool value) const;

    // 这个函数用于进行基本类型的双向绑定，QuickJsEngine内部会保存对应变量的指针
    // 如果该变量已经销毁了，请尽早调用deleteValue()移出该变量（会删除QuickJsEngine内部的指针）
    template<typename T>
    void addValueBinding(const std::string &name, T *ptr) {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        const JSValue global = JS_GetGlobalObject(context);

        // 1. 每个引擎获得属于自己的 index
        const int id = this->registerBoundPtr(ptr);
        nameToPtrIndex[name] = id; // 记录 name → id

        JS_DefinePropertyGetSet(
            context,
            global,
            JS_NewAtom(context, name.c_str()),

            // getter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val, int, JSValueConst *, int magic) {
                    const QuickJsEngine *engine = fromContext(ctx);
                    if (!engine)
                        return JS_ThrowInternalError(ctx, "Engine not found");

                    T *p = static_cast<T *>(engine->getBoundPtr(magic));
                    return JSConverter<T>::toJS(ctx, *p);
                },
                name.c_str(),
                0,
                JS_CFUNC_generic_magic,
                id),

            // setter
            JS_NewCFunctionMagic(
                context,
                [](JSContext *ctx, JSValueConst this_val, int, JSValueConst *argv, int magic) {
                    const QuickJsEngine *engine = fromContext(ctx);
                    if (!engine)
                        return JS_ThrowInternalError(ctx, "Engine not found");

                    T *p = static_cast<T *>(engine->getBoundPtr(magic));
                    *p = JSConverter<T>::fromJS(ctx, argv[0]);
                    return JS_UNDEFINED;
                },
                name.c_str(),
                1,
                JS_CFUNC_generic_magic,
                id),

            JS_PROP_ENUMERABLE | JS_PROP_CONFIGURABLE
        );

        JS_FreeValue(context, global);
    }

    /// 添加全局变量绑定
    /// 注意：如果需要给对象绑定字段以及方法，请在调用改函数之前保证字段表、函数表已经完成了初始化
    template<typename T>
    void addObjectBinding(const std::string &name, T *instance) const {
        if (!context)
            throw std::runtime_error("QuickJsEngine: context is null");

        const JSValue global = JS_GetGlobalObject(context);
        const JSValue obj = JsBinder<T>::bind(context, instance);

        if (JS_IsException(obj)) {
            JS_FreeValue(context, global);
            throw std::runtime_error("QuickJsEngine: failed to wrap object");
        }

        JS_SetPropertyStr(context, global, name.c_str(), obj);
        JS_FreeValue(context, global);
    }

    // 从js当前环境中删除指定变量
    void deleteValue(const std::string &name);

    // 添加一个全局函数：用于屏幕打印输出
    void addPrintFunc(const std::string &funcName) const;

    void addAssertFunc(const std::string &funcName) const;

    JSContext *getContext() const { return context; }

    int getEngineId() const { return engineID; }

public:
    // 通过 JSContext 反查对应的 QuickJsEngine*
    static QuickJsEngine *fromContext(JSContext *ctx);

    static QuickJsEngine *getEngineById(int id);

    // 获取当前线程的引擎实例
    static QuickJsEngine& current();

private:
    JSRuntime *runtime = nullptr;
    JSContext *context = nullptr;
    int engineID = 0;

    std::unordered_map<int, void *> boundPtrTable;
    int nextPtrTableId = 0;
    std::unordered_map<std::string, int> nameToPtrIndex;

    // 注册一个指针，返回索引
    int registerBoundPtr(void *p) {
        const auto id = nextPtrTableId++;
        boundPtrTable[id] = p;
        return id;
    }

    // 通过索引拿指针
    void *getBoundPtr(const int id) const {
        const auto it = boundPtrTable.find(id);
        if (it == boundPtrTable.end())
            return nullptr;
        return it->second;
    }

private:
    // 下一个js引擎的ID（全局唯一）
    static std::atomic<int> s_nextEngineId;
    static std::shared_mutex s_engineRegistryMutex;
    static std::unordered_map<int, QuickJsEngine *> s_engineRegistry;
    thread_local static QuickJsEngine currentEngine;
};

// int
template<>
struct JSConverter<int> {
    static int fromJS(JSContext *ctx, JSValueConst v) {
        int32_t out;
        JS_ToInt32(ctx, &out, v);
        return out;
    }

    static JSValue toJS(JSContext *ctx, int v) { return JS_NewInt32(ctx, v); }
};

template<>
struct JSConverter<int64_t> {
    static int64_t fromJS(JSContext *ctx, JSValueConst v) {
        int64_t out;
        JS_ToInt64(ctx, &out, v);
        return out;
    }

    static JSValue toJS(JSContext *ctx, int64_t v) { return JS_NewInt64(ctx, v); }
};

// double
template<>
struct JSConverter<double> {
    static double fromJS(JSContext *ctx, JSValueConst v) {
        double out;
        JS_ToFloat64(ctx, &out, v);
        return out;
    }

    static JSValue toJS(JSContext *ctx, double v) { return JS_NewFloat64(ctx, v); }
};

// bool
template<>
struct JSConverter<bool> {
    static bool fromJS(JSContext *ctx, JSValueConst v) {
        return JS_ToBool(ctx, v) == 1;
    }

    static JSValue toJS(JSContext *ctx, bool v) { return JS_NewBool(ctx, v); }
};

// std::string
template<>
struct JSConverter<std::string> {
    static std::string fromJS(JSContext *ctx, JSValueConst v) {
        const char *s = JS_ToCString(ctx, v);
        std::string out = s ? s : "";
        JS_FreeCString(ctx, s);
        return out;
    }

    static JSValue toJS(JSContext *ctx, const std::string &v) {
        return JS_NewString(ctx, v.c_str());
    }
};

template<typename U>
struct JSConverter<U*> {
    static U* fromJS(JSContext *ctx, JSValueConst v) {
        return static_cast<U*>(JS_GetOpaque2(ctx, v, JsBinder<U>::getClassID()));
    }

    static JSValue toJS(JSContext *ctx, U* ptr) {
        return JsBinder<U>::bind(ctx, ptr);
    }
};

template<typename U>
struct JSConverter {
    static U fromJS(JSContext *ctx, JSValueConst v) {
        return *static_cast<U*>(JS_GetOpaque2(ctx, v, JsBinder<U>::getClassID()));
    }

    static JSValue toJS(JSContext *ctx, U &v) {
        return JsBinder<U>::bind(ctx, const_cast<U*>(&v));
    }
};

// 支持 const T& / T& / const T
template<typename T>
struct JSConverter<const T &> : JSConverter<T> {
};

template<typename T>
struct JSConverter<T &> : JSConverter<T> {
};

template<typename T>
struct JSConverter<const T> : JSConverter<T> {
};

// 非 const
template<typename C, typename Ret, typename... Args>
struct MethodTraits<Ret (C::*)(Args...)> {
    using Class = C;
    using Return = Ret;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr bool isConst = false;
};

// const 版本
template<typename C, typename Ret, typename... Args>
struct MethodTraits<Ret (C::*)(Args...) const> {
    using Class = C;
    using Return = Ret;
    using ArgsTuple = std::tuple<Args...>;
    static constexpr bool isConst = true;
};

template<typename C, typename FieldType>
struct FieldTraits<FieldType C::*> {
    using Class = C;
    using Type = FieldType;
};

template<typename T>
class JsBinder {
public:
    template<typename FieldPtr>
    static void addField(const std::string &name, FieldPtr ptr) {
        using Traits = FieldTraits<FieldPtr>;
        using FieldType = Traits::Type;

        FieldBase base;
        base.name = name;

        // getter
        base.getter = [ptr](JSContext *ctx, T *obj) -> JSValue {
            return JSConverter<FieldType>::toJS(ctx, obj->*ptr);
        };

        // setter
        base.setter = [ptr](JSContext *ctx, T *obj, JSValueConst jsVal) {
            obj->*ptr = JSConverter<FieldType>::fromJS(ctx, jsVal);
        };

        s_fields.push_back(std::move(base));
    }

    template<typename Method>
    static void addMethod(const std::string &name, Method m) {
        using Traits = MethodTraits<Method>;
        using Return = Traits::Return;
        using ArgsTuple = Traits::ArgsTuple;

        MethodBase base;
        base.name = name;

        base.invoker = [m](JSContext *ctx, T *obj, int argc, JSValueConst *argv) -> JSValue {
            return callMethod<Return>(ctx, obj, m, argv, std::make_index_sequence<std::tuple_size_v<ArgsTuple> >{});
        };

        s_methods.push_back(std::move(base));
    }

    /// 把原生对象包装成 JS 对象（**不负责释放 Class* 的生命周期**）
    static JSValue bind(JSContext *ctx, T *instance) {
        ensureClassInit(ctx);

        const JSValue obj = JS_NewObjectClass(ctx, s_classId);
        if (JS_IsException(obj)) {
            return obj;
        }

        // 将instance指针注入到该obj对象中，只保存指针，不负责 delete
        JS_SetOpaque(obj, instance);

        return obj;
    }

    /// 设置类名，仅用于调试/错误信息
    static void setClassName(const std::string &name) {
        s_className = name;
    }

    static JSClassID getClassID() {
        return s_classId;
    }

private:
    struct FieldBase {
        std::string name;
        std::function<JSValue(JSContext *, T *)> getter;
        std::function<void(JSContext *, T *, JSValueConst)> setter;
    };

    struct MethodBase {
        std::string name;
        std::function<JSValue(JSContext *, T *, int, JSValueConst *)> invoker;
    };

    // 确保Class已经被注册了
    static void ensureClassInit(JSContext *ctx) {
        JSRuntime *rt = JS_GetRuntime(ctx);
        if (s_inited)
            return;

        JS_NewClassID(&s_classId); // 申请一个新的class_id，保证唯一，用于区分不同的class

        JSClassDef def{};
        def.class_name = s_className.c_str();

        if (JS_NewClass(rt, s_classId, &def) < 0) {
            // 这里抛异常比静默失败要好
            throw std::runtime_error("JS_NewClass failed for " + s_className);
        }

        build(ctx);

        s_inited = true;
    }

    template<typename Return, typename Method, size_t... I>
    static JSValue callMethod(JSContext *ctx, T *obj,
                              Method m,
                              JSValueConst *argv,
                              std::index_sequence<I...>) {
        using Traits = MethodTraits<Method>;
        using ArgsTuple = Traits::ArgsTuple;

        // 参数类型解包
        auto nativeRet = (obj->*m)(
            JSConverter<std::tuple_element_t<I, ArgsTuple> >::fromJS(ctx, argv[I])...
        );

        if constexpr (std::is_same_v<Return, void>) {
            return JS_UNDEFINED;
        } else {
            return JSConverter<Return>::toJS(ctx, nativeRet);
        }
    }

    static JSValue methodDispatcher(JSContext *ctx,
                                    JSValueConst this_val, int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        return s_methods[magic].invoker(ctx, obj, argc, argv);
    }

    static void build(JSContext *ctx) {
        const JSValue proto = JS_NewObject(ctx);

        // 绑定 field
        for (int i = 0; i < s_fields.size(); i++) {
            const auto &f = s_fields[i];

            JSAtom atom = JS_NewAtom(ctx, f.name.c_str());

            JS_DefinePropertyGetSet(
                ctx,
                proto,
                atom,
                JS_NewCFunctionMagic(ctx, getterDispatcher, f.name.c_str(), 0, JS_CFUNC_generic_magic, i),
                JS_NewCFunctionMagic(ctx, setterDispatcher, f.name.c_str(), 1, JS_CFUNC_generic_magic, i),
                JS_PROP_ENUMERABLE | JS_PROP_CONFIGURABLE
            );

            JS_FreeAtom(ctx, atom);
        }

        // 方法绑定（无需 magic index 变更）
        for (int i = 0; i < s_methods.size(); i++) {
            JS_SetPropertyStr(
                ctx,
                proto,
                s_methods[i].name.c_str(),
                JS_NewCFunctionMagic(ctx, &methodDispatcher, s_methods[i].name.c_str(),
                                     0, JS_CFUNC_generic_magic, i)
            );
        }

        JS_SetClassProto(ctx, s_classId, proto);
    }

    static JSValue getterDispatcher(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        return s_fields[magic].getter(ctx, obj);
    }

    static JSValue setterDispatcher(JSContext *ctx, JSValueConst this_val,
                                    int argc, JSValueConst *argv, int magic) {
        auto *obj = static_cast<T *>(JS_GetOpaque2(ctx, this_val, s_classId));
        if (!obj) return JS_ThrowTypeError(ctx, "Invalid native object");
        s_fields[magic].setter(ctx, obj, argv[0]);
        return JS_UNDEFINED;
    }

private:
    static inline JSClassID s_classId{0};
    static inline bool s_inited{false};
    static inline std::string s_className{"NativeObject"};
    static inline std::vector<FieldBase> s_fields{};
    static inline std::vector<MethodBase> s_methods{};
};
