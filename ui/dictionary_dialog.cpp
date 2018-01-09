/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::DictionaryDialog class.
 * ---------------------------------------------------------------- */

#include "dictionary_dialog.h"
#include "ui_dictionary_dialog.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data of DictionaryDialog
 * ---------------------------------------------------------------- */
struct DictionaryDialog::Impl
{
    Ui::DictionaryDialog ui;
    JMdictPtr dictionary;
};

/* ---------------------------------------------------------------- *
   Constructs the dictionary dialog.
 * ---------------------------------------------------------------- */
DictionaryDialog::DictionaryDialog(QWidget* parent)
    : QDialog(parent)
    , impl(std::make_shared<Impl>())
{
    impl->ui.setupUi(this);
    QFont font = impl->ui.textEdit->font();
    font.setPointSize(16);
    impl->ui.textEdit->setFont(font);
}

/* ---------------------------------------------------------------- *
   Sets the dictionary.
 * ---------------------------------------------------------------- */
void DictionaryDialog::setDictionary(JMdictPtr dictionary)
{ impl->dictionary = dictionary; }

/* ---------------------------------------------------------------- *
   Search for an entry from dictionary.
 * ---------------------------------------------------------------- */
void DictionaryDialog::on_searchButton_clicked()
{
    const QString text = impl->ui.searchLineEdit->text();
    const bool startsWith = impl->ui.startsWithCheckBox->isChecked();
    const bool endsWith = impl->ui.endsWithCheckBox->isChecked();

    std::vector<JMdict::Entry> results;
    for (const JMdict::Entry& entry : impl->dictionary->entries)
    {
        for (const JMdict::Sense& sense : entry.senses)
        {
            for (const QString& gloss : sense.glosses)
            {
                if (startsWith)
                    if (!gloss.startsWith(text))
                        continue;
                if (endsWith)
                    if (!gloss.endsWith(text))
                        continue;

                if (!startsWith && !endsWith)
                    if (gloss != text)
                        continue;

                results.push_back(entry);
                break;
            }
        }
    }

    QString str;
    QDebug dbg(&str);
    dbg.noquote();
    for (const JMdict::Entry& entry : results)
    {
        qDebug() << entry;

        QString main = entry.kanjis.size()
            ? entry.kanjis[0].wordOrPhrase
            : entry.readings.size()
                ? entry.readings[0].wordOrPhrase
                : "";
        if (main.isEmpty())
            continue;

        QString secondary = entry.readings.size()
            ? entry.readings[0].wordOrPhrase
            : "";
        if (secondary == main)
            secondary.clear();

        QString gloss = entry.senses.size()
            ? entry.senses[0].glosses.size()
                ? entry.senses[0].glosses[0]
                : ""
            : "";
        if (gloss.isEmpty())
            continue;

        dbg << main;
        if (secondary.size())
            dbg << QString(QChar(0x3010))
                << secondary
                << QString(QChar(0x3011));

        dbg << "\n    " << gloss << "\n";


    }
    impl->ui.textEdit->setPlainText(str);
    impl->ui.textEdit->setEnabled(true);
}

} // namespace jpad
} // namespace kuu
