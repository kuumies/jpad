/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::TextEditorReadingToKanjiArea class.
 * ---------------------------------------------------------------- */

#pragma once

#include <QtWidgets/QPlainTextEdit>
#include "../jmdict/jmdict.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Text editor's reading-to-kanji area.
 * ---------------------------------------------------------------- */
class TextEditorReadingToKanjiArea : public QPlainTextEdit
{
    Q_OBJECT

public:
    // Constructs the text editor reading-to-kanji area.
    explicit TextEditorReadingToKanjiArea(QWidget* parent = nullptr);

    // Adjusts the size of the area based on the text editor and
    // side area sizes.
    void adjustSizeToTextEditor(
        const QSize& textEditorSize,
        const QSize& sideAreaSize,
        const QRect& cursorRect);

    // Sets the entries.
    void setEntries(const std::vector<JMdict::Entry>& entries,
                    const QString& reading);

    // Sets the next entry to be selected.
    void selectNext();
    // Sets the previous entry to be selected.
    void selectPrevious();
    // Select the entry.
    void select(int entryPosition);

signals:
    void selectedKanjisChanged(const QString& kanjis);
    void selectionFinished();

protected:
    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
