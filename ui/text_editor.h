/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::TextEditor class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtWidgets/QPlainTextEdit>
#include "../jmdict/jmdict.h"
#include "text_editor_key_converter.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   A text editor of J-PAD application.
 * ---------------------------------------------------------------- */
class TextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    // Constructs the text editor.
    TextEditor();

    // Sets the dictionary.
    void setDictionary(JMdictPtr dictionary);
    // Returns the dictionary.
    JMdictPtr dictionary() const;

    // Sets the text char format font
    void setTextCharFormatFont(const QFont& font);

    // Sets the input mode.
    void setInputMode(TextEditorKeyConverter::Mode mode);

public slots:
    void readingToKanji();

signals:
    // Current key sequence with is used to create a kana character
    // has changed.
    void currentKeySequenceChanged(const QString& keySequence);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onBlockCountChanged(int blockCount);
    void onUpdateRequest(const QRect& rect, int dy);
    void onSelectedKanjisChanged(const QString& kanjis);
    void onSelectionFinished();


private:
    void clearEdit();
    void replaceSelectedText(const QString& txt);

    bool recordKey(const QKeyEvent& keyEvent);
    bool recordKeyUndo();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;

    friend class TextEditorSideArea;
};

} // namespace jpad
} // namespace kuu
