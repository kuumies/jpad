/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::Settings struct.
 * ---------------------------------------------------------------- */

#include "settings.h"
#include <QtCore/QDebug>
#include <QtCore/QSettings>

namespace kuu
{
namespace jpad
{
namespace
{

/* ---------------------------------------------------------------- *
   Definitions
 * ---------------------------------------------------------------- */
struct Strings
{
    static const QString Organization;
    static const QString Application;
};

const QString Strings::Organization = "kuu";
const QString Strings::Application  = "dpad";

} // anonymous namespace

/* ---------------------------------------------------------------- *
   Loads the settings from file or uses the defaults if no
   file on the disk.
 * ---------------------------------------------------------------- */
Settings::Settings()
{
    QSettings settings(Strings::Organization, Strings::Application);
    if (settings.allKeys().isEmpty())
        return;

    font.family    = settings.value("font/family",
                                    font.family).toString();
    font.size      = settings.value("font/size",
                                    font.size).toInt();
    font.antialias = settings.value("font/antialias",
                                    font.antialias).toBool();

    textBuffer.path = settings.value("text_buffer/path",
                                     textBuffer.path).toString();
    textBuffer.temp = settings.value("text_buffer/temp",
                                     textBuffer.temp).toBool();
}

/* ---------------------------------------------------------------- *
   Saves the settings into file.
 * ---------------------------------------------------------------- */
Settings::~Settings()
{
    QSettings settings(Strings::Organization, Strings::Application);
    settings.setValue("font/family",      font.family);
    settings.setValue("font/size",        font.size);
    settings.setValue("font/antialias",   font.antialias);
    settings.setValue("text_buffer/path", textBuffer.path);
    settings.setValue("text_buffer/temp", textBuffer.temp);
}

/* ---------------------------------------------------------------- *
   Constructs the default font.
 * ---------------------------------------------------------------- */
Settings::Font::Font()
    : family("Arial")
    , size(14)
    , antialias(true)
{}

/* ---------------------------------------------------------------- *
   Returns font as Qt font.
 * ---------------------------------------------------------------- */
QFont Settings::Font::toQFont() const
{
    return QFont(family, size);
}

/* ---------------------------------------------------------------- *
   Constructs the default text buffer.
 * ---------------------------------------------------------------- */
Settings::TextBuffer::TextBuffer()
    : temp(true)
{}

} // namespace jpad
} // namespace kuu
