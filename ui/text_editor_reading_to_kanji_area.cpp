/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::TextEditorReadingToKanjiArea
   class.
 * ---------------------------------------------------------------- */

#include "text_editor_reading_to_kanji_area.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data of text editor's reading-to-kanji area.
 * ---------------------------------------------------------------- */
struct TextEditorReadingToKanjiArea::Impl
{
    std::vector<int> entryPositions;
    int currentEntryPosition = 0;
};

/* ---------------------------------------------------------------- *
   Constructs the text editor's reading-to-kanji area.
 * ---------------------------------------------------------------- */
TextEditorReadingToKanjiArea::TextEditorReadingToKanjiArea(
        QWidget* parent)
    : QPlainTextEdit(parent)
    , impl(std::make_shared<Impl>())
{
    setTextInteractionFlags(Qt::NoTextInteraction);
}

/* ---------------------------------------------------------------- *
   Adjusts the size of the area based on the text editor and
   side area sizes.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::adjustSizeToTextEditor(
    const QSize& textEditorSize,
    const QSize& sideAreaSize,
    const QRect& cursorRect)
{
    const int extraSpace = 5;
    const int x = sideAreaSize.width() + extraSpace;
    const int w = textEditorSize.width() - x - extraSpace;
    const int h = 25;

    // Move the area atop of the current text cursor if it would go
    // out-of-bounds.
    int y = cursorRect.bottom() + extraSpace;
    if (y + h >= textEditorSize.height())
        y = cursorRect.top() - cursorRect.height() -
                extraSpace - extraSpace;

    resize(w, h);
    move(x, y);

}

/* ---------------------------------------------------------------- *
   Sets the entries.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::setEntries(
        const std::vector<JMdict::Entry>& entries,
        const QString& reading)
{
    setPlainText("");
    impl->entryPositions.clear();

    QTextCursor tc = textCursor();
    tc.setPosition(0);

    for (const JMdict::Entry& entry : entries)
    {
        for (const JMdict::Kanji& kanji : entry.kanjis)
        {
            impl->entryPositions.push_back(tc.position());
            tc.insertText(kanji.wordOrPhrase);
            tc.insertText("       ");
        }
    }

    impl->entryPositions.push_back(tc.position());
    tc.insertText(reading);

    select(0);
}

/* ---------------------------------------------------------------- *
   Sets the next entry to be selected.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::selectNext()
{
    int entryPosition = impl->currentEntryPosition + 1;
    if (entryPosition >= int(impl->entryPositions.size()))
        entryPosition = 0;

    select(entryPosition);
}

/* ---------------------------------------------------------------- *
   Sets the previous entry to be selected.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::selectPrevious()
{
    int entryPosition = impl->currentEntryPosition - 1;
    if (entryPosition < 0)
        entryPosition = int(impl->entryPositions.size()) - 1;

    select(entryPosition);
}

/* ---------------------------------------------------------------- *
   Sets the next entry to be selected.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::select(int entryPosition)
{
    if (entryPosition < 0 ||
        entryPosition >= int(impl->entryPositions.size()))
    {
        return;
    }

    QTextCursor tc = textCursor();
    tc.setPosition(impl->entryPositions[entryPosition]);
    tc.select(QTextCursor::WordUnderCursor);
    setTextCursor(tc);
    impl->currentEntryPosition = entryPosition;
    emit selectedKanjisChanged(textCursor().selectedText());
}

/* ---------------------------------------------------------------- *
   User can press left or right arrow keys to move the selection
   left or right.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case Qt::Key_Left:
            selectPrevious();
            event->accept();
            return;

        case Qt::Key_Right:
            selectNext();
            event->accept();
            return;

        case Qt::Key_Escape:
        case Qt::Key_Return:
            close();
            return;

        default:
            break;
    }

    event->accept();
}

/* ---------------------------------------------------------------- *
   Notify listeners that user has finished converting readings
   into kanji strings.
 * ---------------------------------------------------------------- */
void TextEditorReadingToKanjiArea::closeEvent(QCloseEvent* event)
{
    emit selectionFinished();
    QPlainTextEdit::closeEvent(event);
}

} // namespace jpad
} // namespace kuu
