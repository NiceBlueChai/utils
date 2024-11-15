#include "utils/utils-qt.h"

int getFontWidth(const QFontMetrics &fm, const QString &text, int len) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int width = fm.horizontalAdvance(text, len);
#else
    int width = fm.width(text, len);
#endif
    return width;
}

/**
 * @brief 自动换行文本
 * 
 * 该函数用于将给定的文本按照指定字体和宽度进行自动换行处理。如果文本的宽度超过了指定的宽度，
 * 则在适当的位置进行换行，以确保文本能够在指定的宽度内显示完整。
 * 
 * @param text 待处理的原始文本
 * @param font 文本的字体信息，用于计算文本宽度
 * @param width 指定的最大文本宽度
 * @return QString 处理后的文本，其中包含了按照指定宽度进行的换行
 */
QString wrappedText(const QString& text, QFont font, int width){
    // 使用给定的字体创建字体度量对象，用于计算文本宽度
    QFontMetrics fm(font);
    // 用于临时存储文本片段的字符串
    QString strTemp;
    // 最终返回的经过换行处理的字符串
    QString retStr;
    // 计算原始文本的宽度（以像素为单位）
    int textWidthInPxs = getFontWidth(fm, text);
    // 原始文本的长度
    const int rear = text.length();
    // 初始化前指针位置
    int pre = 0;
    // 初始化后指针位置
    int vernier = 1;
    // 记录当前挑选的文本宽度（以像素为单位）
    int pickUpWidthPxs = 0;
    // 当前文本片段的长度
    int curLen = 0;

    // 如果文本宽度小于等于指定宽度，或者文本中包含换行符，则直接返回原始文本
    if (textWidthInPxs <= width || text.contains("\n")) {
        return text;
    }

    // 遍历文本，进行自动换行处理
    while (vernier <= rear) {
        // 计算当前文本片段的长度
        curLen = vernier - pre;
        // 从文本中提取当前文本片段
        strTemp = text.mid(pre, curLen);
        // 计算当前文本片段的宽度（以像素为单位）
        pickUpWidthPxs = getFontWidth(fm, strTemp);
        // 如果当前文本片段的宽度大于指定宽度，则在适当位置进行换行
        if (pickUpWidthPxs > width) {
            // 将当前文本片段添加到结果字符串中，并在末尾添加换行符
            retStr += strTemp + (vernier + 1 <= rear ? "\n" : "");
            // 更新前指针位置
            pre = vernier;
            // 重置当前挑选的文本宽度
            pickUpWidthPxs = 0;
            // 清空临时字符串
            strTemp.clear();
        }
        // 后指针向前移动
        vernier++;
    }
    // 如果剩余的文本宽度小于指定宽度且临时字符串非空，则将剩余的文本添加到结果字符串中
    if (pickUpWidthPxs < width && !strTemp.isEmpty()) {
        retStr += strTemp;
    }
    // 返回处理后的文本
    return retStr;
}

static const QLocale cn(QLocale::Chinese, QLocale::China);
static QCollator collator(cn);
static std::once_flag init_collator_flag;

static bool startWithChinse(const QString& str) {
    if (str.isEmpty()) {
        return false;
    }
    auto first = str.at(0).unicode();
    return (first >= 0x4e00 && first <= 0x9fa5);
}

bool compareByPinYin(const QString& left, const QString& right) {
    std::call_once(init_collator_flag, []() {
        collator.setNumericMode(true);
    });
    if (startWithChinse(left) && !startWithChinse(right)){
        return false;
    } else if (!startWithChinse(left) && startWithChinse(right)){
        return true;
    } else {
        return collator.compare(left, right) < 0;
    }
}