/* ---------------------------------------------------------------- *
   Copyright (c) 2018 Kuu
   Antti Jumpponen <kuumies@gmail.com>

   The main entry point of D-PAD application.
 * ---------------------------------------------------------------- */

#include <iostream>
#include <QtWidgets/QApplication>
#include "jmdict/jmdict_parser.h"
#include "ui/main_window.h"
#include "ui/text_editor.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    using namespace kuu;
    using namespace kuu::jpad;
    const QString path = "/users/kuumies/Downloads/JMdict_e";
    //const QString path = "C:/Users/Antti Jumpponen/Dropbox/projects/jpad/resource/JMdict_e";
    JMdictPtr jmDict;
    try
    {
        jmDict = jmdict_parser::read(path);
    }
    catch(const std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    QApplication a(argc, argv);

    SettingsPtr settings = std::make_shared<Settings>();

    TextEditor* textEditor = new TextEditor();
    textEditor->setDictionary(jmDict);
    textEditor->setTextCharFormatFont(settings->font.toQFont());

    MainWindow mainWindow;
    mainWindow.setTextEditor(textEditor);
    mainWindow.setSettings(settings);
    mainWindow.show();

    QObject::connect(
        textEditor,
        &TextEditor::currentKeySequenceChanged,
        &mainWindow,
        &MainWindow::setCurrentKeySequence);

    QObject::connect(
        textEditor,
        &TextEditor::modificationChanged,
        &mainWindow,
        &MainWindow::setWindowModified);

    QObject::connect(
        textEditor,
        &TextEditor::selectionChanged,
        &mainWindow,
        &MainWindow::onTextEditorSelectionChanged);

    return a.exec();
}
