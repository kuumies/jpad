/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The implementation of kuu::jpad::MainWindow class.
 * ---------------------------------------------------------------- */

#include "main_window.h"
#include "ui_main_window.h"
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTextStream>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include "about_dialog.h"
#include "dictionary_dialog.h"
#include "preferences_dialog.h"
#include "text_editor.h"

namespace kuu
{
namespace jpad
{
namespace
{

/* ---------------------------------------------------------------- *
   Definitions
 * ---------------------------------------------------------------- */
static const QString UNTITLED_FILE = "untitled";

/* ---------------------------------------------------------------- *
   Saves the text file as UTF-8 with BOM.
 * ---------------------------------------------------------------- */
void saveFile(const QString& filePath, const QString& source)
{
    QFileInfo fi(filePath);
    QDir dir = fi.absoluteDir();
    if (!dir.exists() && !dir.mkpath(dir.absolutePath()))
        throw std::runtime_error(
            QString("Failed to make path %1")
                .arg(dir.absolutePath()).toStdString());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::runtime_error(
            "Failed to create file " +
                filePath.toStdString());

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(true);
    stream<< source;
    stream.flush();

    file.close();
}

/* ---------------------------------------------------------------- *
   Loads a UTF-8 text file.
 * ---------------------------------------------------------------- */
QString loadFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error(
            "Failed to load file " +
                filePath.toStdString());

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString out = stream.readAll();

    file.close();
    return out;
}

} // anonymous namespace

/* ---------------------------------------------------------------- *
   Private data of main window.
 * ---------------------------------------------------------------- */
struct MainWindow::Impl
{
    Ui::MainWindow ui;
    QLabel* keySequenceLabel;
    TextEditor* textEditor = nullptr;
    QString currentFile = "untitled";
    SettingsPtr settings;
};

/* ---------------------------------------------------------------- *
   Constructs the main window.
 * ---------------------------------------------------------------- */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , impl(std::make_shared<Impl>())
{
    impl->ui.setupUi(this);
    impl->ui.actionSave->setEnabled(false);
    impl->ui.actionReadingToKanji->setEnabled(false);
    impl->ui.toolBar->addAction(impl->ui.actionNewFile->icon(),
                                impl->ui.actionNewFile->text());
    impl->ui.toolBar->addAction(impl->ui.actionOpen->icon(),
                                impl->ui.actionOpen->text());
    impl->ui.toolBar->addAction(impl->ui.actionSave->icon(),
                                impl->ui.actionSave->text());
    impl->ui.toolBar->addAction(impl->ui.actionSaveAs->icon(),
                                impl->ui.actionSaveAs->text());
    impl->ui.toolBar->addSeparator();
    impl->ui.toolBar->addAction(impl->ui.actionUndo->icon(),
                                impl->ui.actionUndo->text());
    impl->ui.toolBar->addAction(impl->ui.actionRedo->icon(),
                                impl->ui.actionRedo->text());
    impl->ui.toolBar->addSeparator();
    impl->ui.toolBar->addAction(impl->ui.actionCut->icon(),
                                impl->ui.actionCut->text());
    impl->ui.toolBar->addAction(impl->ui.actionCopy->icon(),
                                impl->ui.actionCopy->text());
    impl->ui.toolBar->addAction(impl->ui.actionPaste->icon(),
                                impl->ui.actionPaste->text());

    impl->ui.toolBar->addSeparator();
    impl->ui.toolBar->addAction(impl->ui.actionReadingToKanji->icon(),
                                impl->ui.actionReadingToKanji->text());
    impl->ui.toolBar->setVisible(false);
    impl->keySequenceLabel = new QLabel;
    statusBar()->addWidget(impl->keySequenceLabel);
    updateWindowTitle();

#ifdef Q_OS_MACOS
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowIcon(QIcon("://icons/jpad.png"));
}

/* ---------------------------------------------------------------- *
   Sets the text editor.
 * ---------------------------------------------------------------- */
void MainWindow::setTextEditor(TextEditor* editor)
{
    impl->textEditor = editor;
    setCentralWidget(editor);

    QObject::connect(
        editor, &TextEditor::copyAvailable,
        impl->ui.actionCopy, &QAction::setEnabled);
    QObject::connect(
        impl->ui.actionCopy, &QAction::triggered,
        editor, &TextEditor::copy);

    QObject::connect(
        editor, &TextEditor::copyAvailable,
        impl->ui.actionCut, &QAction::setEnabled);
    QObject::connect(
        impl->ui.actionCut, &QAction::triggered,
        editor, &TextEditor::cut);

    QObject::connect(
        impl->ui.actionPaste, &QAction::triggered,
        editor, &TextEditor::paste);

    QObject::connect(
        impl->ui.actionSelectAll, &QAction::triggered,
                editor, &TextEditor::selectAll);
}

/* ---------------------------------------------------------------- *
   Sets the settings.
 * ---------------------------------------------------------------- */
void MainWindow::setSettings(SettingsPtr settings)
{
    impl->settings = settings;

    if (!impl->settings->textBuffer.path.isEmpty() &&
        QFile::exists(impl->settings->textBuffer.path))
    {
        QString source = loadFile(impl->settings->textBuffer.path);
        impl->textEditor->setPlainText(source);
        impl->currentFile = UNTITLED_FILE;
        impl->ui.actionSave->setEnabled(true);
        setWindowModified(false);
        updateWindowTitle();
    }
}

/* ---------------------------------------------------------------- *
   Sets the current key sequence into status bar.
 * ---------------------------------------------------------------- */
void MainWindow::setCurrentKeySequence(const QString& keySequence)
{
    impl->keySequenceLabel->setText(keySequence);
}

/* ---------------------------------------------------------------- *
   Closes the application.
 * ---------------------------------------------------------------- */
void MainWindow::onTextEditorSelectionChanged()
{
    const QString selectedText =
        impl->textEditor->textCursor().selectedText();
    impl->ui.actionReadingToKanji->setEnabled(selectedText.size());
}

/* ---------------------------------------------------------------- *
   Closes the application.
 * ---------------------------------------------------------------- */
void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!isWindowModified())
        return;

    if (impl->settings->textBuffer.temp)
    {
        QString appDataDir =
            QStandardPaths::writableLocation(
                QStandardPaths::AppLocalDataLocation);
        QString tempFilePath  =
            QDir(appDataDir).absoluteFilePath("temp_buffer.txt");
        saveFile(tempFilePath, impl->textEditor->toPlainText());
        impl->settings->textBuffer.path = tempFilePath;
        return QMainWindow::closeEvent(event);
    }

    QMessageBox msgBox(this);
    msgBox.setIconPixmap(style()->standardPixmap(QStyle::SP_MessageBoxQuestion));
    msgBox.setWindowTitle("Save changes.");
    msgBox.setText(QString("Do you want to save '%1' before exit?")
                    .arg(QFileInfo(impl->currentFile).fileName()));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    const int answer = msgBox.exec();
    if (answer == QMessageBox::Save)
        on_actionSave_triggered();

    QMainWindow::closeEvent(event);
}

/* ---------------------------------------------------------------- *
   Create a new file.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionNewFile_triggered()
{
    if (isWindowModified())
        askChangesSave();

    impl->settings->textBuffer.path = "";
    impl->settings->textBuffer.temp = true;
    impl->currentFile = UNTITLED_FILE;
    impl->textEditor->setPlainText("");
    impl->ui.actionSave->setEnabled(false);
    setWindowModified(false);
    updateWindowTitle();
}

/* ---------------------------------------------------------------- *
   Open an existing file.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionOpen_triggered()
{
    QString currentDir = QDir::currentPath();
    if (!impl->currentFile.isEmpty())
        currentDir = QFileInfo(impl->currentFile)
            .absoluteDir()
            .absolutePath();

    const QString& fileName =
        QFileDialog::getOpenFileName(
            this,
            "Open",
            currentDir);
    if (fileName.isEmpty())
        return;

    if (isWindowModified())
        askChangesSave();

    try
    {
        QString source = loadFile(fileName);
        impl->textEditor->setPlainText(source);
        impl->currentFile = fileName;
        impl->ui.actionSave->setEnabled(true);
        impl->settings->textBuffer.path = fileName;
        impl->settings->textBuffer.temp = false;
        setWindowModified(false);
        updateWindowTitle();
    }
    catch(const std::runtime_error& ex)
    {
        QMessageBox::critical(
            this,
            "Save failed",
            ex.what());
    }
}

/* ---------------------------------------------------------------- *
   Save the current file.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionSave_triggered()
{
    try
    {
        saveFile(impl->currentFile, impl->textEditor->toPlainText());
        impl->ui.actionSave->setEnabled(true);
        setWindowModified(false);
        updateWindowTitle();
    }
    catch(const std::runtime_error& ex)
    {
        QMessageBox::critical(
            this,
            "Save failed",
            ex.what());
    }
}

/* ---------------------------------------------------------------- *
   Save the current file as a new file.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionSaveAs_triggered()
{
    QString currentDir = QDir::currentPath();
    if (!impl->currentFile.isEmpty())
        currentDir = QFileInfo(impl->currentFile)
            .absoluteDir()
            .absolutePath();

    const QString& fileName =
        QFileDialog::getSaveFileName(
            this,
            "Save As",
            currentDir);
    if (fileName.isEmpty())
        return;

    try
    {
        saveFile(fileName, impl->textEditor->toPlainText());
        impl->currentFile = fileName;
        impl->ui.actionSave->setEnabled(true);
        impl->settings->textBuffer.path = fileName;
        impl->settings->textBuffer.temp = false;
        setWindowModified(false);
        updateWindowTitle();
    }
    catch(const std::runtime_error& ex)
    {
        QMessageBox::critical(
            this,
            "Save failed",
            ex.what());
    }
}

/* ---------------------------------------------------------------- *
   Print the document.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    QPrintDialog dlg(&printer, this);
    const int result = dlg.exec();
    if (result == QDialog::Rejected)
        return;

    impl->textEditor->print(&printer);
}

/* ---------------------------------------------------------------- *
   Start converting the selected text from reading into kanjis.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionReadingToKanji_triggered()
{
    impl->textEditor->readingToKanji();
}

/* ---------------------------------------------------------------- *
   Use hiragana/katakana input.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionHiragana_toggled(bool checked)
{
    if (!checked)
    {
        if (!impl->ui.actionSystem->isChecked())
            impl->ui.actionSystem->setChecked(true);
        return;
    }

    impl->ui.actionSystem->setChecked(false);
    impl->textEditor->setInputMode(
        TextEditorKeyConverter::Mode::HiraganaKatakana);
}

/* ---------------------------------------------------------------- *
   Use system locale input.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionSystem_toggled(bool checked)
{
    if (!checked)
    {
        if (!impl->ui.actionHiragana->isChecked())
            impl->ui.actionHiragana->setChecked(true);
        return;
    }

    impl->ui.actionHiragana->setChecked(false);
    impl->textEditor->setInputMode(
        TextEditorKeyConverter::Mode::SystemLocale);
}

/* ---------------------------------------------------------------- *
   Show the diactionary dialog.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionDictionary_triggered()
{
    DictionaryDialog dictionaryDlg(this);
    dictionaryDlg.setDictionary(impl->textEditor->dictionary());
    dictionaryDlg.exec();
}

/* ---------------------------------------------------------------- *
   Show preferences dialog.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog preferencesDlg(this);

    connect(&preferencesDlg, &PreferencesDialog::settingsChanged,
            [&]()
    {
        impl->textEditor->setTextCharFormatFont(
            impl->settings->font.toQFont());
    });

    preferencesDlg.setSettings(impl->settings);
    preferencesDlg.exec();
}

/* ---------------------------------------------------------------- *
   Show about dialog.
 * ---------------------------------------------------------------- */
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDlg(this);
    aboutDlg.exec();
}

/* ---------------------------------------------------------------- *
   Updates the window title to reflect the current file name.
 * ---------------------------------------------------------------- */
void MainWindow::updateWindowTitle()
{
    QString title = "J-PAD";
    title += " - ";
    title += QFileInfo(impl->currentFile).fileName();
    title += "[*]";
    setWindowTitle(title);
}

/* ---------------------------------------------------------------- *
   Ask user whether to save the changes and then save them is
   that is what user wants.
 * ---------------------------------------------------------------- */
void MainWindow::askChangesSave()
{
    QMessageBox msgBox(this);
    msgBox.setIconPixmap(style()->standardPixmap(QStyle::SP_MessageBoxQuestion));
    msgBox.setWindowTitle("Save the changes?");
    msgBox.setText("Do you want to save the canges?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int answer = msgBox.exec();
    if (answer == QMessageBox::Yes)
    {
        if (impl->currentFile.isEmpty() ||
            impl->currentFile == UNTITLED_FILE)
        {
            on_actionSaveAs_triggered();
        }
        else
            on_actionSave_triggered();
    }
}

} // namespace jpad
} // namespace kuu
