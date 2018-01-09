/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::PreferencesDialog class.
 * ---------------------------------------------------------------- */

#pragma once

#include <QtWidgets/QDialog>
#include "../settings.h"

namespace kuu
{
namespace jpad
{

/* ---------------------------------------------------------------- *
   The preferences dialog of J-PAD application.
 * ---------------------------------------------------------------- */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructs the preferences dialog.
    explicit PreferencesDialog(QWidget* parent = nullptr);

    // Sets the settings.
    void setSettings(SettingsPtr settings);

signals:
    void settingsChanged();

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void on_applyButton_clicked();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
