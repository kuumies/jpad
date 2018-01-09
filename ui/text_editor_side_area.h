/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::TextEditorSideArea class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtWidgets/QWidget>

namespace kuu
{
namespace jpad
{

class TextEditor;

/* ---------------------------------------------------------------- *
   The side area of text editor of J-PAD application.
 * ---------------------------------------------------------------- */
class TextEditorSideArea : public QWidget
{
    Q_OBJECT

public:
    // Constructs the text editor side area.
    TextEditorSideArea(TextEditor* textEditor);

    // Returns the area width based on the current content on
    // the text editor.
    int areaWidth() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
