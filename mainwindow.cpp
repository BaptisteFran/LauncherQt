#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFrame *frame = ui->frame;

    QWebEngineView *view = new QWebEngineView(frame);

    view->setUrl(QUrl("http://jibestudio.net/rose_online_patches_notes"));

    // Créer un layout pour attacher la WebView au frame
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->addWidget(view);  // Ajouter la WebView au layout

    // Appliquer le layout au frame
    frame->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}
