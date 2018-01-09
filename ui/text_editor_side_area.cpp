/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::TextEditorSideArea class.
 * ---------------------------------------------------------------- */

#include "text_editor_side_area.h"
#include <QtGui/QPainter>
#include <QtGui/QTextBlock>
#include "text_editor.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data of text editor side area.
 * ---------------------------------------------------------------- */
struct TextEditorSideArea::Impl
{
    TextEditor* editor;
};

/* ---------------------------------------------------------------- *
   Constructs the text editor side area.
 * ---------------------------------------------------------------- */
TextEditorSideArea::TextEditorSideArea(TextEditor* editor)
    : QWidget(editor)
    , impl(std::make_shared<Impl>())
{
    impl->editor = editor;
}

/* ---------------------------------------------------------------- *
   Returns the area width based on the current content on
   the text editor.
 * ---------------------------------------------------------------- */
int TextEditorSideArea::areaWidth() const
{
    int digits = 1;
    int max = qMax(1, impl->editor->blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space + 20;
}

/* ---------------------------------------------------------------- *
   Paints the  widget.
 * ---------------------------------------------------------------- */
void TextEditorSideArea::paintEvent(QPaintEvent* event)
{
    // Fill with background
    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(236, 236, 236));

    QTextBlock block = impl->editor->firstVisibleBlock();
    QRectF blockBound =
        impl->editor->blockBoundingGeometry(block);
    int blockNumber = block.blockNumber();

    int top = (int) blockBound.translated(impl->editor->contentOffset()).top();
    int bottom = top + (int) blockBound.height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        blockBound =
            impl->editor->blockBoundingGeometry(block);
        blockNumber = block.blockNumber();

        top = (int) blockBound.translated(impl->editor->contentOffset()).top();
        bottom = top + (int) blockBound.height();
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        block = block.next();
//        top = bottom;
//        bottom = top + (int) blockBound.height();
        ++blockNumber;
    }
}

} // namespace jpad
} // namespace kuu
