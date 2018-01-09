/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::DictionaryDialog class.
 * ---------------------------------------------------------------- */

#pragma once

#include <QtWidgets/QDialog>
#include "../jmdict/jmdict.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   A dictionary dialog of J-PAD application.
 * ---------------------------------------------------------------- */
class DictionaryDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructs the dictionary dialog.
    explicit DictionaryDialog(QWidget* parent = nullptr);

    // Sets the dictionary.
    void setDictionary(JMdictPtr dictionary);

private slots:
    void on_searchButton_clicked();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
