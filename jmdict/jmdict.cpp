/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::JMdict struct.
 * ---------------------------------------------------------------- */

#include "jmdict.h"

namespace kuu
{

/* ---------------------------------------------------------------- *
   Implementation of JMdict streaming operator..
 * ---------------------------------------------------------------- */
QDebug operator<<(QDebug debug, const JMdict& dict)
{
    QDebugStateSaver saver(debug);
    debug << "JMdict";
    for (const JMdict::Entry& entry : dict.entries)
        debug << entry;

    return debug;
}

/* ---------------------------------------------------------------- *
   Implementation of JMdict::Entry streaming operator.
 * ---------------------------------------------------------------- */
QDebug operator<<(QDebug debug, const JMdict::Entry& dictEntry)
{
    QDebugStateSaver saver(debug);
    debug << "\nJMdict::Entry";
    debug << "\n\tSequence number:" << dictEntry.sequenceNumber;

    debug << "\n\tKanjis";
    for (const JMdict::Kanji& kanji : dictEntry.kanjis)
    {
        debug << "\n\t\tWord or phrase:" << kanji.wordOrPhrase;
        for (const QString& info : kanji.info)
            debug << "\n\t\t\tInfo:" << info;
        for (const QString& priority : kanji.priorities)
            debug << "\n\t\t\tPriority:" << priority;
    }

    debug << "\n\tReadings";
    for (const JMdict::Reading& reading : dictEntry.readings)
    {
        debug << "\n\t\tWord or phrase:" << reading.wordOrPhrase;
        for (const QString& info : reading.info)
            debug << "\n\t\t\tInfo:" << info;
        for (const QString& priority : reading.priorities)
            debug << "\n\t\t\tPriority:" << priority;
    }

    debug << "\n\tSenses";
    for (const JMdict::Sense& sense : dictEntry.senses)
    {
        for (const QString& partOfSpeech : sense.partOfSpeeches)
            debug << "\n\t\tPart-of-spheech:" << partOfSpeech;
        for (const QString& gloss : sense.glosses)
            debug << "\n\t\tGloss:" << gloss;
        for (const JMdict::LoadWordSource& loadWordSource :
                sense.loanwordSources)
        {
            debug << "\n\t\tLoadword:";
            debug << "\n\t\t\tSource:" << loadWordSource.source;
            debug << "\n\t\t\tDesc full or partial:"
                  << loadWordSource.descFullOrPartial;
            debug << "\n\t\t\tWasei:" << loadWordSource.wasei;
        }
        for (const QString& fieldOfApplication
             : sense.fieldOfApplications)
        {
            debug << "\n\t\tField Of Application:"
                  << fieldOfApplication;
        }
        for (const QString& misc : sense.misc)
            debug << "\n\t\tMisc:" << misc;
        for (const QString& dialect : sense.dialect)
            debug << "\n\t\tDialect:" << dialect;
        for (const QString& info : sense.infos)
            debug << "\n\t\tInfo:" << info;
    }

    return debug;
}

/* ---------------------------------------------------------------- *
   Search entries containing the text.
 * ---------------------------------------------------------------- */
std::vector<JMdict::Entry> JMdict::searchByReading(
    const QString& text)
{
    std::vector<Entry> matches;
    std::copy_if(
        entries.begin(),
        entries.end(),
        std::back_inserter(matches),
        [text](const Entry& e)
    {
        for(const Reading& reading : e.readings)
            if (reading.wordOrPhrase == text)
                return true;
        return false;
    });

    std::sort(
        matches.begin(),
        matches.end(),
        [](const Entry& e1, const Entry& e2)
    {
        if (e1.kanjis.size() == 0)
            return false;
        if (e2.kanjis.size() == 0)
            return true;
        return e2.readings[0].priorities.size() <
               e1.readings[0].priorities.size();
    });

    return matches;
}

} // namespace kuu
