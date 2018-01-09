/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jmdict_parser namespace.

   See:
   http://www.edrdg.org/jmdict/jmdict_dtd_h.html
   http://www.edrdg.org/jmdict/jmdict_sample.html
 * ---------------------------------------------------------------- */

#include "jmdict_parser.h"

#include <exception>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QXmlStreamReader>

namespace kuu
{
namespace jmdict_parser
{
namespace
{

/* ---------------------------------------------------------------- *
   Declares XML tag names.
 * ---------------------------------------------------------------- */
const QString TAG_ENTRY                = "entry";
const QString TAG_SEQUENCE_NUMBER      = "ent_seq";
const QString TAG_KANJI_ELEMENT        = "k_ele";
const QString TAG_KANJI_PHRASE         = "keb";
const QString TAG_KANJI_INFO           = "ke_inf";
const QString TAG_KANJI_PRIORITY       = "ke_pri";
const QString TAG_READING_ELEMENT      = "r_ele";
const QString TAG_READING_PHRASE       = "reb";
const QString TAG_READING_PRIORITY     = "re_pri";
const QString TAG_SENSE                = "sense";
const QString TAG_PART_OF_SPEECH       = "pos";
const QString TAG_GLOSS                = "gloss";
const QString TAG_LOANWORD_SOURCE      = "lsource";
const QString TAG_FIELD_OF_APPLICATION = "field";
const QString TAG_MISC                 = "misc";
const QString TAG_DIALECT              = "dial";
const QString TAG_INFO                 = "s_inf";

/* ---------------------------------------------------------------- *
   Declares XML tag attributes
 * ---------------------------------------------------------------- */
const QString TAB_ATTRIBUTE_TYPE       = "ls_type";
const QString TAB_ATTRIBUTE_WASEIEIGO  = "ls_wasei";

/* ---------------------------------------------------------------- *
   Reads a sense from the stream.
 * ---------------------------------------------------------------- */
JMdict::Sense readSense(QXmlStreamReader& r)
{
    JMdict::Sense out;
    for (;;)
    {
        const QXmlStreamReader::TokenType token = r.readNext();
        if (token == QXmlStreamReader::EndElement &&
            r.name() == TAG_SENSE)
        {
            break;
        }

        if (r.name() == TAG_PART_OF_SPEECH)
            out.partOfSpeeches.push_back(r.readElementText());

        if (r.name() == TAG_GLOSS)
            out.glosses.push_back(r.readElementText());

        if (r.name() == TAG_LOANWORD_SOURCE)
        {
            JMdict::LoadWordSource src;
            src.source = "full";
            const QXmlStreamAttributes attributes = r.attributes();
            if (attributes.size())
            {
                if (attributes.hasAttribute(TAB_ATTRIBUTE_TYPE))
                    src.source = attributes.value(TAB_ATTRIBUTE_TYPE).toString();

                if (attributes.hasAttribute(TAB_ATTRIBUTE_WASEIEIGO))
                    src.wasei = attributes.value(TAB_ATTRIBUTE_WASEIEIGO).toString();
            }

            src.source = r.readElementText();
            out.loanwordSources.push_back(src);
        }

        if (r.name() == TAG_FIELD_OF_APPLICATION)
            out.fieldOfApplications.push_back(r.readElementText());

        if (r.name() == TAG_MISC)
            out.misc.push_back(r.readElementText());

        if (r.name() == TAG_DIALECT)
            out.dialect.push_back(r.readElementText());

        if (r.name() == TAG_INFO)
            out.infos.push_back(r.readElementText());
    }

    return out;
}

/* ---------------------------------------------------------------- *
   Reads a reading element from the stream.
 * ---------------------------------------------------------------- */
JMdict::Reading readReadingElement(QXmlStreamReader& r)
{
    JMdict::Reading out;
    for (;;)
    {
        const QXmlStreamReader::TokenType token = r.readNext();
        if (token == QXmlStreamReader::EndElement &&
            r.name() == TAG_READING_ELEMENT)
        {
            break;
        }

        if (r.name() == TAG_READING_PHRASE)
            out.wordOrPhrase = r.readElementText();

        if (r.name() == TAG_READING_PRIORITY)
            out.priorities.push_back(r.readElementText());

    }
    return out;
}

/* ---------------------------------------------------------------- *
   Reads a kanji element from the stream.
 * ---------------------------------------------------------------- */
JMdict::Kanji readKanjiElement(QXmlStreamReader& r)
{
    JMdict::Kanji out;
    for (;;)
    {
        const QXmlStreamReader::TokenType token = r.readNext();
        if (token == QXmlStreamReader::EndElement &&
            r.name() == TAG_KANJI_ELEMENT)
        {
            break;
        }

        if (r.name() == TAG_KANJI_PHRASE)
            out.wordOrPhrase = r.readElementText();

        if (r.name() == TAG_KANJI_PRIORITY)
            out.priorities.push_back(r.readElementText());

        if (r.name() == TAG_KANJI_INFO)
            out.info.push_back(r.readElementText());
    }
    return out;
}

/* ---------------------------------------------------------------- *
   Reads an entry from the stream.
 * ---------------------------------------------------------------- */
JMdict::Entry readEntry(QXmlStreamReader& r)
{
    JMdict::Entry e;

    for(;;)
    {
        QXmlStreamReader::TokenType token = r.readNext();
        if (token == QXmlStreamReader::EndElement &&
            r.name() == TAG_ENTRY)
        {
            break;
        }

        if (r.name() == TAG_SEQUENCE_NUMBER) // entry sequence number
            e.sequenceNumber = r.readElementText();

        if (r.name() == TAG_KANJI_ELEMENT) // kanji element
        {
            const JMdict::Kanji kanji = readKanjiElement(r);
            e.kanjis.push_back(kanji);
        }

        if (r.name() == TAG_READING_ELEMENT) // reading element
        {
            const JMdict::Reading reading = readReadingElement(r);
            e.readings.push_back(reading);
        }

        if (r.name() == TAG_SENSE) // sense
        {
            const JMdict::Sense sense = readSense(r);
            e.senses.push_back(sense);
        }
    }

    return e;
}

} // anonymous namespace

/* ---------------------------------------------------------------- *
   Reads dictionary from the XML file.
 * ---------------------------------------------------------------- */
JMdictPtr read(const QString& filePath)
{
    JMdictPtr out = std::make_shared<JMdict>();

    if (!QFile::exists(filePath))
        throw std::runtime_error("File does not exits");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
        throw std::runtime_error("Failed to open file");

    QXmlStreamReader r(&file);
    while (!r.atEnd() && !r.hasError())
    {
        QXmlStreamReader::TokenType token = r.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if (r.name() == TAG_ENTRY)
            {
                const JMdict::Entry e = readEntry(r);
                out->entries.push_back(e);
            }
        }
    }

    if (r.hasError())
        throw std::runtime_error(r.errorString().toStdString());

    return out;
}

} // namespace jmdict_parser
} // namespace kuu
