/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::Settings struct.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtGui/QFont>

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   A settings of J-PAD application.
 * ---------------------------------------------------------------- */
struct Settings
{
    // Loads the settings from file or uses the defaults if no
    // file on the disk.
    Settings();
    // Saves the settings into file.
    ~Settings();

    // Text editor font settings
    struct Font
    {
        // Constructs the default font.
        Font();

        // Returns font as Qt font.
        QFont toQFont() const;

        QString family;
        int size;
        bool antialias;
    } font;

    // Text buffer
    struct TextBuffer
    {
        // Constructs the default text buffer
        TextBuffer();

        QString path;
        bool temp;
    } textBuffer;
};

/* ---------------------------------------------------------------- *
   Declares a shared pointer of Settings struct.
 * ---------------------------------------------------------------- */
using SettingsPtr = std::shared_ptr<Settings>;

} // namespace jpad
} // namespace kuu
