/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::TextEditorKeyConverter class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtCore/QString>

class QKeyEvent;

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Converts the key presses in text editor into either to
   hiraganas or katakanas.
 * ---------------------------------------------------------------- */
class TextEditorKeyConverter
{
public:
    // Modes of the key converter
    enum class Mode
    {
        SystemLocale,
        HiraganaKatakana,
    };

    // Constructs the text editor key converter.
    TextEditorKeyConverter();

    // Sets and gets the mode.
    void setMode(Mode mode);
    Mode mode() const;

    // Returns the current recorded key sequence as a string.
    QString recordedKeySequence() const;

    // Clears currently recored keys. Call this to start from the
    // beginning.
    void clear();

    // Returns true if the key event is a valid key for recording.
    // Valid keys are keys that can create kanas.
    bool isValidKey(const QKeyEvent& key) const;

    // Records a key command. Returns true if the key command
    // creates text. The new text is in the given in argument.
    bool recordKey(const QKeyEvent& key, QString& textOut);

    // Undo the last recorded key.
    void undoRecordedKey();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
