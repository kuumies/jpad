/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::TextEditorKeyConverter class.
 * ---------------------------------------------------------------- */

#include "text_editor_key_converter.h"
#include <vector>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Hiragana key sequences
 * ---------------------------------------------------------------- */
struct HiraganaKeySequences
{
    // Defines aliases
    using Pair       = std::pair<QKeySequence, QString>;
    using PairVector = std::vector<Pair>;

    // Reads hiragana keys from the resource file.
    void readKeys(const QString& filePath,
                  bool shiftModifier)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream ts(&file);
        while(!ts.atEnd())
        {
            QString line = ts.readLine().trimmed();
            if (line.startsWith(";") || line.isEmpty())
                continue;

            QStringList splits;
            for (const QString& s : line.split(" "))
                if (!s.trimmed().isEmpty())
                    splits.push_back(s);

            if (splits.size() != 2)
                continue;

            const QString lineKeyCommand = splits[0];
            const QString lineHiragana = splits[1];

            QStringList hiraganaParts = lineHiragana.split(",");
            QString hiragana;
            for (const QString& hiraganaPart : hiraganaParts)
            {
                bool ok = false;
                QChar cc(hiraganaPart.toUShort(&ok, 16));
                if (ok)
                    hiragana += cc;
            }

            QString keyCommand;
            for (const QChar& c : lineKeyCommand.trimmed())
            {
                if (!keyCommand.isEmpty())
                    keyCommand += ", ";

                QString k = c.toUpper();
                if (shiftModifier)
                    k.prepend("Shift+");
                keyCommand += k;
            }

            Pair keyPair;
            keyPair.first  = QKeySequence::fromString(keyCommand);
            keyPair.second = hiragana;
            keySequences.push_back(keyPair);
        }
    }

    // Constructs the hiragana key sequences.
    HiraganaKeySequences()
    {
        readKeys(":/hiragana_keys.txt", false);
        readKeys(":/katakana_keys.txt", true);
    }

    // Returns true if the given in key sequences is a valid
    // hiragana key sequence. The reference hiragana param is then
    // set to be the matching hiragana.
    bool find(const QKeySequence& seq, QString& hiragana) const
    {
        const auto it = std::find_if(
            keySequences.begin(),
            keySequences.end(),
            [seq](const Pair& pair)
        {
            return pair.first.matches(seq) == QKeySequence::ExactMatch;
        });

        if (it != keySequences.end())
        {
            hiragana = it->second;
            return true;
        }

        return false;
    }

    // Hiragana key sequences.
    PairVector keySequences;
};

/* ---------------------------------------------------------------- *
   Accepted key sequences
 * ---------------------------------------------------------------- */
struct AcceptedKeySequences
{
    // Constructs the accepted key sequences
    AcceptedKeySequences()
    {
        keySequnces = {
            QKeySequence(Qt::Key_B),
            QKeySequence(Qt::Key_C),
            QKeySequence(Qt::Key_D),
            QKeySequence(Qt::Key_F),
            QKeySequence(Qt::Key_G),
            QKeySequence(Qt::Key_H),
            QKeySequence(Qt::Key_J),
            QKeySequence(Qt::Key_K),
            QKeySequence(Qt::Key_M),
            QKeySequence(Qt::Key_N),
            QKeySequence(Qt::Key_R),
            QKeySequence(Qt::Key_P),
            QKeySequence(Qt::Key_S),
            QKeySequence(Qt::Key_T),
            QKeySequence(Qt::Key_W),
            QKeySequence(Qt::Key_Z),
            QKeySequence(Qt::Key_A),
            QKeySequence(Qt::Key_I),
            QKeySequence(Qt::Key_E),
            QKeySequence(Qt::Key_U),
            QKeySequence(Qt::Key_O),
            QKeySequence(Qt::Key_Y),

            QKeySequence(Qt::Key_B + Qt::ShiftModifier),
            QKeySequence(Qt::Key_C + Qt::ShiftModifier),
            QKeySequence(Qt::Key_D + Qt::ShiftModifier),
            QKeySequence(Qt::Key_F + Qt::ShiftModifier),
            QKeySequence(Qt::Key_G + Qt::ShiftModifier),
            QKeySequence(Qt::Key_H + Qt::ShiftModifier),
            QKeySequence(Qt::Key_J + Qt::ShiftModifier),
            QKeySequence(Qt::Key_K + Qt::ShiftModifier),
            QKeySequence(Qt::Key_M + Qt::ShiftModifier),
            QKeySequence(Qt::Key_N + Qt::ShiftModifier),
            QKeySequence(Qt::Key_R + Qt::ShiftModifier),
            QKeySequence(Qt::Key_P + Qt::ShiftModifier),
            QKeySequence(Qt::Key_S + Qt::ShiftModifier),
            QKeySequence(Qt::Key_T + Qt::ShiftModifier),
            QKeySequence(Qt::Key_W + Qt::ShiftModifier),
            QKeySequence(Qt::Key_Z + Qt::ShiftModifier),
            QKeySequence(Qt::Key_A + Qt::ShiftModifier),
            QKeySequence(Qt::Key_I + Qt::ShiftModifier),
            QKeySequence(Qt::Key_E + Qt::ShiftModifier),
            QKeySequence(Qt::Key_U + Qt::ShiftModifier),
            QKeySequence(Qt::Key_O + Qt::ShiftModifier),
            QKeySequence(Qt::Key_Y + Qt::ShiftModifier),

            QKeySequence(Qt::Key_Period),
        };
    }

    // Returns true if the key sequnce is accepted.
    bool isAccepted(const QKeySequence& seq) const
    {
        const auto it =
            std::find_if(keySequnces.begin(),
                         keySequnces.end(),
                         [seq](const QKeySequence& other)
        {
            return other.matches(seq) == QKeySequence::ExactMatch;
        });

        return it != keySequnces.end();
    }

    // A vector of key sequences that are valid.
    std::vector<QKeySequence> keySequnces;
};

/* ---------------------------------------------------------------- *
   Currently recorded key sequence
 * ---------------------------------------------------------------- */
struct RecordedKeySequence
{
    // Clears the recorded keys.
    void clear()
    {
        recordedKeys.clear();
    }

    // Adds a key.
    void addKey(const QKeyEvent& key)
    {
        recordedKeys.push_back(key);
        if (recordedKeys.size() >= 5)
            removeFirstKey();
    }

    // Removes the first key.
    void removeFirstKey()
    {
        if (recordedKeys.size())
            recordedKeys.erase(recordedKeys.begin());
    }

    // Removes the last key.
    void removeLastKey()
    {
        if (recordedKeys.size())
            recordedKeys.erase(recordedKeys.end()-1);
    }

    // Returns the current key sequence or an empty sequence if
    // the recorded keys are invalid.
    QKeySequence keySequence() const
    {
        if (recordedKeys.size() == 1)
            return QKeySequence(
                recordedKeys[0].key() +
                recordedKeys[0].modifiers());

        if (recordedKeys.size() == 2)
            return QKeySequence(
                recordedKeys[0].key() + recordedKeys[0].modifiers(),
                recordedKeys[1].key() + recordedKeys[1].modifiers());
        if (recordedKeys.size() == 3)
            return QKeySequence(
                recordedKeys[0].key() + recordedKeys[0].modifiers(),
                recordedKeys[1].key() + recordedKeys[1].modifiers(),
                recordedKeys[2].key() + recordedKeys[2].modifiers());
        if (recordedKeys.size() == 4)
            return QKeySequence(
                recordedKeys[0].key() + recordedKeys[0].modifiers(),
                recordedKeys[1].key() + recordedKeys[1].modifiers(),
                recordedKeys[2].key() + recordedKeys[2].modifiers(),
                recordedKeys[3].key() + recordedKeys[3].modifiers());
        return QKeySequence();
    }

    // Recorded keys.
    std::vector<QKeyEvent> recordedKeys;
};

/* ---------------------------------------------------------------- *
   Private data of the text editor key converter.
 * ---------------------------------------------------------------- */
struct TextEditorKeyConverter::Impl
{
    const AcceptedKeySequences acceptedKeySequences;
    const HiraganaKeySequences hiraganaKeySequences;

    Mode mode;
    RecordedKeySequence recordedKeySequence;
};

/* ---------------------------------------------------------------- *
   Constructs the text editor key converter.
 * ---------------------------------------------------------------- */
TextEditorKeyConverter::TextEditorKeyConverter()
    : impl(std::make_shared<Impl>())
{}

/* ---------------------------------------------------------------- *
   Sets the mode.
 * ---------------------------------------------------------------- */
void TextEditorKeyConverter::setMode(Mode mode)
{
    impl->mode = mode;
    clear();
}

/* ---------------------------------------------------------------- *
   Returns the mode.
 * ---------------------------------------------------------------- */
TextEditorKeyConverter::Mode TextEditorKeyConverter::mode() const
{ return impl->mode; }

/* ---------------------------------------------------------------- *
   Returns the current recorded key sequence as a string.
 * ---------------------------------------------------------------- */
QString TextEditorKeyConverter::recordedKeySequence() const
{ return impl->recordedKeySequence.keySequence().toString(); }

/* ---------------------------------------------------------------- *
   Clears the recorded keys.
 * ---------------------------------------------------------------- */
void TextEditorKeyConverter::clear()
{ impl->recordedKeySequence.clear(); }

/* ---------------------------------------------------------------- *
   Returns true if the key sequence is in the accepted
   key sequences.
 * ---------------------------------------------------------------- */
bool TextEditorKeyConverter::isValidKey(const QKeyEvent& key) const
{
    if (impl->mode == Mode::SystemLocale)
        return true;

    const QKeySequence inputSeq(
        key.key() +
        key.modifiers());
    return impl->acceptedKeySequences.isAccepted(inputSeq);
}

/* ---------------------------------------------------------------- *
   Records a key.
 * ---------------------------------------------------------------- */
bool TextEditorKeyConverter::recordKey(
        const QKeyEvent& keyEvent,
        QString& textOut)
{
    // Check if the output key sequence is valid.
    bool out = false;
    switch(impl->mode)
    {
        case Mode::SystemLocale:
            textOut = keyEvent.text();
            out = true;
            break;

        case Mode::HiraganaKatakana:
        {
            // Check that the input key sequence is valid.
            const QKeySequence inputSeq(keyEvent.key() +
                                        keyEvent.modifiers());
            if (!impl->acceptedKeySequences.isAccepted(inputSeq))
                return false;

            // Add key
            impl->recordedKeySequence.addKey(keyEvent);

            QString hiragana;
            if (impl->hiraganaKeySequences.find(
                impl->recordedKeySequence.keySequence(),
                hiragana))
            {
                textOut = hiragana;
                out = true;
                impl->recordedKeySequence.clear();
            }

            break;
        }
    }

    return out;
}

/* ---------------------------------------------------------------- *
   Undo the last recorded key.
 * ---------------------------------------------------------------- */
void TextEditorKeyConverter::undoRecordedKey()
{ impl->recordedKeySequence.removeLastKey(); }

} // namespace jpad
} // namespace kuu
