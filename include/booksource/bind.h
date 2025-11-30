#pragma once

#include <booksource/rule.h>
#include <booksource/engine.h>

namespace BindingMacros {

#define BEGIN_CLASS_BINDING(T) \
void init##T##ClassInfo() { \
JsBinder<T>::setClassName(#T);
#define FIELD(T, field) JsBinder<T>::addField(#field, &T::field);
#define METHOD(T, func) JsBinder<T>::addMethod(#func, &T::func);

#define END_CLASS_BINDING() }

} // namespace Binding

// TODO: 由于BookSource引入了optional，绑定需要进行特殊处理
namespace BookSourceBinding {

    using namespace BindingMacros;

    void initBookInfoRuleClassInfo();

    void initBookListRuleClassInfo();

    void initContentRuleClassInfo();

    void initExploreRuleClassInfo();

    void initReviewRuleClassInfo();

    void initSearchRuleClassInfo();

    void initTocRuleClassInfo();

    void initBookSourceClassInfo();

    inline void initEngineClassInfo() {
        initBookInfoRuleClassInfo();
        initBookListRuleClassInfo();
        initContentRuleClassInfo();
        initExploreRuleClassInfo();
        initReviewRuleClassInfo();
        initSearchRuleClassInfo();
        initTocRuleClassInfo();
        initBookSourceClassInfo();
    }

BEGIN_CLASS_BINDING(BookInfoRule)
    FIELD(BookInfoRule, init)
    FIELD(BookInfoRule, name)
    FIELD(BookInfoRule, author)
    FIELD(BookInfoRule, intro)
    FIELD(BookInfoRule, kind)
    FIELD(BookInfoRule, lastChapter)
    FIELD(BookInfoRule, updateTime)
    FIELD(BookInfoRule, coverUrl)
    FIELD(BookInfoRule, tocUrl)
    FIELD(BookInfoRule, wordCount)
    FIELD(BookInfoRule, canReName)
    FIELD(BookInfoRule, downloadUrls)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(BookListRule)
    FIELD(BookListRule, bookList)
    FIELD(BookListRule, name)
    FIELD(BookListRule, author)
    FIELD(BookListRule, intro)
    FIELD(BookListRule, kind)
    FIELD(BookListRule, lastChapter)
    FIELD(BookListRule, updateTime)
    FIELD(BookListRule, bookUrl)
    FIELD(BookListRule, coverUrl)
    FIELD(BookListRule, wordCount)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(ContentRule)
    FIELD(ContentRule, content)
    FIELD(ContentRule, title)
    FIELD(ContentRule, nextContentUrl)
    FIELD(ContentRule, webJs)
    FIELD(ContentRule, sourceRegex)
    FIELD(ContentRule, replaceRegex)
    FIELD(ContentRule, imageStyle)
    FIELD(ContentRule, imageDecode)
    FIELD(ContentRule, payAction)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(ExploreRule)
    FIELD(ExploreRule, bookList)
    FIELD(ExploreRule, name)
    FIELD(ExploreRule, author)
    FIELD(ExploreRule, intro)
    FIELD(ExploreRule, kind)
    FIELD(ExploreRule, lastChapter)
    FIELD(ExploreRule, updateTime)
    FIELD(ExploreRule, bookUrl)
    FIELD(ExploreRule, coverUrl)
    FIELD(ExploreRule, wordCount)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(ReviewRule)
    FIELD(ReviewRule, reviewUrl)
    FIELD(ReviewRule, avatarRule)
    FIELD(ReviewRule, contentRule)
    FIELD(ReviewRule, postTimeRule)
    FIELD(ReviewRule, reviewQuoteUrl)
    FIELD(ReviewRule, voteUpUrl)
    FIELD(ReviewRule, voteDownUrl)
    FIELD(ReviewRule, postReviewUrl)
    FIELD(ReviewRule, postQuoteUrl)
    FIELD(ReviewRule, deleteUrl)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(SearchRule)
    FIELD(SearchRule, bookList)
    FIELD(SearchRule, name)
    FIELD(SearchRule, author)
    FIELD(SearchRule, intro)
    FIELD(SearchRule, kind)
    FIELD(SearchRule, lastChapter)
    FIELD(SearchRule, updateTime)
    FIELD(SearchRule, bookUrl)
    FIELD(SearchRule, coverUrl)
    FIELD(SearchRule, wordCount)
    FIELD(SearchRule, checkKeyWord)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(TocRule)
    FIELD(TocRule, preUpdateJs)
    FIELD(TocRule, chapterList)
    FIELD(TocRule, chapterName)
    FIELD(TocRule, chapterUrl)
    FIELD(TocRule, formatJs)
    FIELD(TocRule, isVolume)
    FIELD(TocRule, isVip)
    FIELD(TocRule, isPay)
    FIELD(TocRule, updateTime)
    FIELD(TocRule, nextTocUrl)
END_CLASS_BINDING()

BEGIN_CLASS_BINDING(BookSource)
    FIELD(BookSource, bookSourceUrl)
    FIELD(BookSource, bookSourceName)
    FIELD(BookSource, bookSourceGroup)
    FIELD(BookSource, bookSourceType)
    FIELD(BookSource, bookUrlPattern)
    FIELD(BookSource, customOrder)
    FIELD(BookSource, enabled)
    FIELD(BookSource, enabledExplore)
    FIELD(BookSource, jsLib)
    FIELD(BookSource, enabledCookieJar)
    FIELD(BookSource, concurrentRate)
    FIELD(BookSource, header)
    FIELD(BookSource, loginUrl)
    FIELD(BookSource, loginUi)
    FIELD(BookSource, loginCheckJs)
    FIELD(BookSource, coverDecodeJs)
    FIELD(BookSource, bookSourceComment)
    FIELD(BookSource, variableComment)
    FIELD(BookSource, lastUpdateTime)
    FIELD(BookSource, respondTime)
    FIELD(BookSource, weight)
    FIELD(BookSource, exploreUrl)
    FIELD(BookSource, exploreScreen)
    FIELD(BookSource, ruleExplore)
    FIELD(BookSource, searchUrl)
    FIELD(BookSource, ruleSearch)
    FIELD(BookSource, ruleBookInfo)
    FIELD(BookSource, ruleToc)
    FIELD(BookSource, ruleContent)
    FIELD(BookSource, ruleReview)
END_CLASS_BINDING()

} // namespace BookSourceBinding

namespace Binding {
    inline void initEngineClassInfo() {
        BookSourceBinding::initEngineClassInfo();
    }
}
