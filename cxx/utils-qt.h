#pragma once

int getFontWidth(const QFontMetrics &fm, const QString &text, int len=-1);

QString wrappedText(const QString& text, QFont font, int width);

bool compareByPinYin(const QString& left, const QString& right);