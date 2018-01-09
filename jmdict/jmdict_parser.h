/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jmdict_parser namespace.
 * ---------------------------------------------------------------- */

#pragma once

#include "jmdict.h"

namespace kuu
{
namespace jmdict_parser
{

/* ---------------------------------------------------------------- *
   Reads the JM dictionary from the XML file at give in file path.
   Throw std::runtime_error if the file path is not valid.
 * ---------------------------------------------------------------- */
JMdictPtr read(const QString& filePath);

} // namespace jmdict_parser
} // namespace kuu
