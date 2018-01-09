/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::AboutDialog class.
 * ---------------------------------------------------------------- */

#pragma once

#include <memory>
#include <QtWidgets/QDialog>

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   The about dialog of J-PAD application.
 * ---------------------------------------------------------------- */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructs the about dialog.
    explicit AboutDialog(QWidget* parent = nullptr);

private slots:
    void on_finlandButton_clicked();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
