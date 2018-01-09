/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::TextEditor class.
 * ---------------------------------------------------------------- */

#include "text_editor.h"
#include <QtGui/QKeyEvent>
#include <QtGui/QTextBlock>
#include "text_editor_key_converter.h"
#include "text_editor_reading_to_kanji_area.h"
#include "text_editor_side_area.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data of TextEditor
 * ---------------------------------------------------------------- */
struct TextEditor::Impl
{
    // Constructs the private data.
    Impl(TextEditor* self)
        : sideArea(self)
        , readingToKanjiArea(self)
    {}

    TextEditorKeyConverter keyConverter;
    TextEditorSideArea sideArea;
    TextEditorReadingToKanjiArea readingToKanjiArea;

    JMdictPtr dictionary;
    std::vector<JMdict::Entry> readingSearchResults;
};

/* ---------------------------------------------------------------- *
   Constructs the text editor.
 * ---------------------------------------------------------------- */
TextEditor::TextEditor()
    : impl(std::make_shared<Impl>(this))
{
    impl->keyConverter.setMode(TextEditorKeyConverter::Mode::HiraganaKatakana);
    impl->readingToKanjiArea.hide();

    connect(this, &TextEditor::blockCountChanged,
            this, &TextEditor::onBlockCountChanged);

    connect(this, &TextEditor::updateRequest,
            this, &TextEditor::onUpdateRequest);

    connect(&impl->readingToKanjiArea,
            &TextEditorReadingToKanjiArea::selectedKanjisChanged,
            this,
            &TextEditor::onSelectedKanjisChanged);

    connect(&impl->readingToKanjiArea,
            &TextEditorReadingToKanjiArea::selectionFinished,
            this,
            &TextEditor::onSelectionFinished);

    onBlockCountChanged(0);

    setTextCharFormatFont(QFont("Arial", 14));
}

/* ---------------------------------------------------------------- *
   Sets the dictionary.
 * ---------------------------------------------------------------- */
void TextEditor::setDictionary(JMdictPtr dictionary)
{ impl->dictionary = dictionary; }

/* ---------------------------------------------------------------- *
   Returns the dictionary.
 * ---------------------------------------------------------------- */
JMdictPtr TextEditor::dictionary() const
{ return impl->dictionary; }

/* ---------------------------------------------------------------- *
   Sets the text char format font.
 * ---------------------------------------------------------------- */
void TextEditor::setTextCharFormatFont(const QFont& font)
{
    QTextCharFormat fmt;
    fmt.setFont(font);
    setCurrentCharFormat(fmt);

    QTextBlock block =  document()->findBlockByNumber(0);
    if(!block.isValid())
        return;
    QTextCursor cursor(block);

    // Set font to all of the blocks.
    for (QTextBlock::iterator it = cursor.block().begin(); !(it.atEnd()); ++it)
    {
        QTextCursor tempCursor = cursor;
        tempCursor.setPosition(it.fragment().position());
        tempCursor.setPosition(it.fragment().position()
                                + it.fragment().length(),
                               QTextCursor::KeepAnchor);
        tempCursor.setCharFormat(fmt);
    }
}

/* ---------------------------------------------------------------- *
   Sets the input mode.
 * ---------------------------------------------------------------- */
void TextEditor::setInputMode(TextEditorKeyConverter::Mode mode)
{
    impl->keyConverter.setMode(mode);
}

/* ---------------------------------------------------------------- *
   Convert the currently selected reading into kanjis. If the
   area is already visible the select the next kanji.
 * ---------------------------------------------------------------- */
void TextEditor::readingToKanji()
{
    if (impl->readingToKanjiArea.isVisible())
        return;

    const QTextCursor tc = textCursor();
    const QString searchText = tc.selectedText();
    impl->readingSearchResults =
        impl->dictionary->searchByReading(searchText);

    impl->readingToKanjiArea.setEntries(
        impl->readingSearchResults,
        searchText);
    impl->readingToKanjiArea.adjustSizeToTextEditor(
        size(),
        impl->sideArea.size(),
         cursorRect(tc));
    impl->readingToKanjiArea.setFocus();
    impl->readingToKanjiArea.show();
}

/* ---------------------------------------------------------------- *
   Resizes the text editor - mainly settings the correction
   geometry into line number area.
 * ---------------------------------------------------------------- */
void TextEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    // Update line area geometry
    QRect cr = contentsRect();
    impl->sideArea.setGeometry(
        QRect(cr.left(), cr.top(),
        impl->sideArea.areaWidth(),
        cr.height()));
}

/* ---------------------------------------------------------------- *
   Handles a key press
 * ---------------------------------------------------------------- */
void TextEditor::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_F2:
            readingToKanji();
            return;

        case Qt::Key_Escape:
            clearEdit();
            return;

        case Qt::Key_Backspace:
            if (recordKeyUndo())
                return;
            break;

        default:
            if (recordKey(*event))
            {
                event->ignore();
                return;
            }
            break;
    }

    QPlainTextEdit::keyPressEvent(event);
}

/* ---------------------------------------------------------------- *
   Add margins to left to fit the line number area.
 * ---------------------------------------------------------------- */
void TextEditor::onBlockCountChanged(int /*blockCount*/)
{
    setViewportMargins(impl->sideArea.areaWidth(), 0, 0, 0);
}

/* ---------------------------------------------------------------- *
   Change the side area.
 * ---------------------------------------------------------------- */
void TextEditor::onUpdateRequest(const QRect& rect, int dy)
{
    if (dy) // scroll
        impl->sideArea.scroll(0, dy);
    else
        impl->sideArea.update(
            0, rect.y(),
            impl->sideArea.width(), rect.height());

    if (rect.contains(viewport()->rect()))
        onBlockCountChanged(0);
}

/* ---------------------------------------------------------------- *
   Replace the currently selected text with the user selected
   kanjis string.
 * ---------------------------------------------------------------- */
void TextEditor::onSelectedKanjisChanged(const QString& kanjis)
{
    replaceSelectedText(kanjis);
}

/* ---------------------------------------------------------------- *
   User has finished changing a reading to kanji. Clear the
   selection on the editor.
 * ---------------------------------------------------------------- */
void TextEditor::onSelectionFinished()
{
    QTextCursor tc = textCursor();
    QString txt = tc.selectedText();
    tc.clearSelection();
    tc.setPosition(tc.position() + txt.size());
    setTextCursor(tc);
    setFocus();
}

/* ---------------------------------------------------------------- *
   Clears the edit data.
 * ---------------------------------------------------------------- */
void TextEditor::clearEdit()
{
    impl->readingSearchResults.clear();
    impl->readingToKanjiArea.hide();
    impl->keyConverter.clear();
    emit currentKeySequenceChanged(
        impl->keyConverter.recordedKeySequence());
}

/* ---------------------------------------------------------------- *
   Replaces the currently selected text with the give in text.
   The new text is selected.
 * ---------------------------------------------------------------- */
void TextEditor::replaceSelectedText(const QString& txt)
{
    QTextCursor tc = textCursor();
    tc.removeSelectedText();
    tc.insertText(txt);
    tc.setPosition(tc.position()- txt.size(),
                   QTextCursor::KeepAnchor);
    setTextCursor(tc);
}

/* ---------------------------------------------------------------- *
   Records a key.
 * ---------------------------------------------------------------- */
bool TextEditor::recordKey(const QKeyEvent& keyEvent)
{
    if (impl->keyConverter.mode() ==
        TextEditorKeyConverter::Mode::HiraganaKatakana)
    {
        if (impl->keyConverter.isValidKey(keyEvent))
        {
            QString text;
            if (impl->keyConverter.recordKey(keyEvent, text))
            {
                insertPlainText(text);
            }

            emit currentKeySequenceChanged(
                impl->keyConverter.recordedKeySequence());
            return true;
        }

        return false;
    }
    return false;
}

/* ---------------------------------------------------------------- *
   Undo recorded key if available.
 * ---------------------------------------------------------------- */
bool TextEditor::recordKeyUndo()
{
    if (impl->keyConverter.recordedKeySequence().size())
    {
        impl->keyConverter.undoRecordedKey();
        emit currentKeySequenceChanged(
            impl->keyConverter.recordedKeySequence());
        return true;
    }

    return false;
}

} // namespace jpad
} // namespace kuu
