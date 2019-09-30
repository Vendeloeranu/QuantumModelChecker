#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bbvAdd\\b"), QStringLiteral("\\bbvMul\\b"), QStringLiteral("\\bbvSplit\\b"), QStringLiteral("\\bbvAssign\\b")
        , QStringLiteral("\\bbvAdc\\b"), QStringLiteral("\\bbvAddC\\b"), QStringLiteral("\\bbvShl\\b"), QStringLiteral("\\bbvShr\\b")
        , QStringLiteral("\\bbvAdcC\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    assertFormat.setFontItalic(true);
    assertFormat.setForeground(Qt::blue);
    const QString assertPatterns[] = {
        QStringLiteral("\\bbvAssert\\b"), QStringLiteral("\\bbvAssume\\b")
    };
    for (const QString &pattern : assertPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = assertFormat;
        highlightingRules.append(rule);
    }

    compareFormat.setFontWeight(QFont::Bold);
    compareFormat.setForeground(Qt::green);
    const QString comparePatterns[] = {
        QStringLiteral("\\bbveands\\b"), QStringLiteral("\\bbveEqMod\\b"), QStringLiteral("\\bbvrands\\b"), QStringLiteral("\\bbveEq\\b"), QStringLiteral("\\bbvrEq\\b")
        , QStringLiteral("\\bbveTrue\\b"), QStringLiteral("\\bbvrTrue\\b"), QStringLiteral("\\bbvTrue\\b")
    };
    for (const QString &pattern : comparePatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = compareFormat;
        highlightingRules.append(rule);
    }

    otherFormat.setForeground(Qt::darkMagenta);
    const QString otherPatterns[] = {
        QStringLiteral("limbs"), QStringLiteral("\\bbvconst\\b"), QStringLiteral("\\bbvvar\\b"), QStringLiteral("proc"), QStringLiteral("\\bbvVar\\b")
    };
    for (const QString &pattern : otherPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = otherFormat;
        highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(Qt::red);
    commentStartExpression = QRegularExpression(QStringLiteral("\\(\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*\\)"));
}


void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
