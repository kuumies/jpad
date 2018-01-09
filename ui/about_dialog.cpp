/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::AboutDialog class.
 * ---------------------------------------------------------------- */

#include "about_dialog.h"
#include "ui_about_dialog.h"
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data.
 * ---------------------------------------------------------------- */
struct AboutDialog::Impl
{
    Ui::AboutDialog ui;
};

/* ---------------------------------------------------------------- *
   Constructs the about dialog.
 * ---------------------------------------------------------------- */
AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , impl(std::make_shared<Impl>())
{
    impl->ui.setupUi(this);
}

/* ---------------------------------------------------------------- *
   Open up a *special* URL
 * ---------------------------------------------------------------- */
void AboutDialog::on_finlandButton_clicked()
{
    QDesktopServices::openUrl(
        QUrl("http://www.visitfinland.com/"));
}

} // namespace jpad
} // namespace kuu
