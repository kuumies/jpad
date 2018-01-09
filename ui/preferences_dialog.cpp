/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::PreferencesDialog class.
 * ---------------------------------------------------------------- */

#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   Private data.
 * ---------------------------------------------------------------- */
struct PreferencesDialog::Impl
{
    void updateUiFromSettings()
    {
        ui.fontComboBox->setCurrentFont(settings->font.toQFont());
        ui.fontSizeSpinBox->setValue(settings->font.size);
        ui.antialiasCheckBox->setChecked(settings->font.antialias);
    }

    void updateSettingsFromUi()
    {
        settings->font.family    = ui.fontComboBox->font().family();
        settings->font.size      = ui.fontSizeSpinBox->value();
        settings->font.antialias = ui.antialiasCheckBox->isChecked();
    }

    Ui::PreferencesDialog ui;
    SettingsPtr settings;
};

/* ---------------------------------------------------------------- *
   Constructs the preferences dialog.
 * ---------------------------------------------------------------- */
PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
    , impl(std::make_shared<Impl>())
{
    impl->ui.setupUi(this);
    impl->ui.listWidget->setCurrentRow(0);
}

/* ---------------------------------------------------------------- *
   Sets the settings.
 * ---------------------------------------------------------------- */
void PreferencesDialog::setSettings(SettingsPtr settings)
{
    impl->settings = settings;
    impl->updateUiFromSettings();
}

/* ---------------------------------------------------------------- *
   Apply settings and close the dialog.
 * ---------------------------------------------------------------- */
void PreferencesDialog::on_okButton_clicked()
{
    on_applyButton_clicked();
    accept();
}

/* ---------------------------------------------------------------- *
   Close the dialog.
 * ---------------------------------------------------------------- */
void PreferencesDialog::on_cancelButton_clicked()
{
    reject();
}

/* ---------------------------------------------------------------- *
   Apply changes into settings.
 * ---------------------------------------------------------------- */
void PreferencesDialog::on_applyButton_clicked()
{
    impl->updateSettingsFromUi();
    emit settingsChanged();
}

} // namespace jpad
} // namespace kuu
