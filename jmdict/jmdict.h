/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::JMdict struct.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <vector>
#include <QtCore/QDebug>
#include <QtCore/QString>

namespace kuu
{

/* ---------------------------------------------------------------- *
   The JM dictionary.
 * ---------------------------------------------------------------- */
struct JMdict
{    
    // Defines a kanji element. Most of the entries have a single
    // kanji element.
    struct Kanji
    {
        // A word or short phrase in Japanese.
       QString wordOrPhrase;

       // A coded information field related specifically to the
       // orthography of the word/phrase
       std::vector<QString> info;

       // Relative priorities. The value can be:
       // - news1/2: appears in the "wordfreq" file
       // - ichi1/2: appears in the "Ichimango goi bunruishuu"
       // - spec1 and spec2: a small number of words use this marker
       //   when they are detected as being common, but are not
       //   included in other lists.
       // - gai1/2: common loanwords, based on the wordfreq file.
       // - nfxx: this is an indicator of frequency-of-use ranking in
       //   the wordfreq file. "xx" is the number of the set of 500
       //   words in which the entry can be found, with "01" assigned
       //   to the first 500, "02" to the second, and so on.
       std::vector<QString> priorities;
    };

    // Defines a reading of kanji element.
    struct Reading
    {
        // Word or phrase writen in kana
        QString wordOrPhrase;

        // If not null value, indicates that the reading
        // cannot be regarded as a true reading of the kanji.
        QString noKanji;

        // ??
        QString restriction;

        // General coded information pertaining to the specific
        // reading. Typically it will be used to indicate some unusual
        // aspect of the reading.
        QString info;

        // Relative priorities.
        std::vector<QString> priorities;
    };

    // Information about the source language(s) of a loan-word /
    // gairaigo.
    struct LoadWordSource
    {
        // Source language(s). It will be coded using the three-
        // letter language code from the ISO 639-2 standard.
        QString source;

        // Fully or partially describes the source word or phrase of
        // the loanword.
        QString descFullOrPartial;

        // Indicates that the Japanese word has been constructed
        // from words in the source language, and not from an actual
        // phrase in that language. Most commonly used to indicate
        // "waseieigo".
        QString wasei;
    };

    // Records the translational equivalent of the Japanese word,
    // plus other related information.
    struct Sense
    {
        // Part-of-speech information about the entry/sense. Should
        // use appropriate entity codes. In general where there are
        // multiple senses in an entry, the part-of-speech of an
        // earlier sense will apply to later senses unless there is
        // a new part-of-speech indicated.
        std::vector<QString> partOfSpeeches;

        // Target-language words or phrases which are equivalents to
        // the Japanese word. This element would normally be present,
        // however it may be omitted in entries which are purely for
        // a cross-reference.
        std::vector<QString> glosses;

        // Loadword sources
        std::vector<LoadWordSource> loanwordSources;

        // Information about the field of application of the entry /
        // sense. When absent, general application is implied.
        // Entity coding for specific fields of application.
        std::vector<QString> fieldOfApplications;

        // This is used for other relevant information about
        // the entry/sense. As with part-of-speech, information will
        // usually apply to several senses.
        std::vector<QString> misc;

        // For words specifically associated with regional dialects
        // in Japanese, the entity code for that dialect, e.g. ksb
        // for Kansaiben.
        std::vector<QString> dialect;

        // Additional information to be recorded about a sense.
        // Typical usage would be to indicate such things as level
        // of currency of a sense, the regional variations, etc.
        std::vector<QString> infos;
    };

    // A single entry in the dictionary.
    struct Entry
    {
        // A unique numeric sequence number for each entry
        QString sequenceNumber;

        // Kanjis and its readings
        std::vector<Kanji> kanjis;
        std::vector<Reading> readings;

        // Senses
        std::vector<Sense> senses;
    };

    // Entries
    std::vector<Entry> entries;

    // Search entries containing the text.
    std::vector<Entry> searchByReading(const QString& text);
};

/* ---------------------------------------------------------------- *
   Defines streaming operators.
 * ---------------------------------------------------------------- */
QDebug operator<<(QDebug debug, const JMdict& dict);
QDebug operator<<(QDebug debug, const JMdict::Entry& dictEntry);

/* ---------------------------------------------------------------- *
   Declares a shared pointer of JMdict struct.
 * ---------------------------------------------------------------- */
using JMdictPtr = std::shared_ptr<JMdict>;

} // namespace kuu
