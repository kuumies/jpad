/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The definition of kuu::jpad::MainWindow class.
 * ---------------------------------------------------------------- */

#pragma once

#include <QtWidgets/QMainWindow>
#include "settings.h"

namespace kuu
{
namespace jpad
{

class TextEditor;

/* ---------------------------------------------------------------- *
   The main window of J-PAD application.
 * ---------------------------------------------------------------- */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructs the main window.
    explicit MainWindow(QWidget* parent = nullptr);

    // Sets the text editor.
    void setTextEditor(TextEditor* editor);
    // Sets the settings.
    void setSettings(SettingsPtr settings);

public slots:
    // Sets the current key sequence into status bar.
    void setCurrentKeySequence(const QString& keySequence);

    // User has selected a text in the editor.
    void onTextEditorSelectionChanged();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void on_actionNewFile_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionPrint_triggered();
    void on_actionReadingToKanji_triggered();
    void on_actionHiragana_toggled(bool checked);
    void on_actionSystem_toggled(bool checked);
    void on_actionDictionary_triggered();
    void on_actionPreferences_triggered();
    void on_actionAbout_triggered();

private:
    void updateWindowTitle();
    void askChangesSave();

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

} // namespace jpad
} // namespace kuu
