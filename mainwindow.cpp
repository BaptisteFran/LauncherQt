#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , process(new QProcess(this)) // Initialisation de QProcess
    , manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    // Connecter le bouton "Play" au slot onPlayButtonClicked
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);

    this->setWindowTitle("Haki Rose Online");
    this->setStyleSheet("QMainWindow { "
                        "background-image: url(:/images/background.jpg); "
                        "background-repeat: no-repeat; "
                        "background-position: center; "
                        "background-size: cover; " // Utilise contain pour garder l'image entière
                        "}");

    QFrame *frame = ui->frame;
    QWebEngineView *view = new QWebEngineView(frame);
    QVBoxLayout *layout = new QVBoxLayout(frame); // Créer un layout pour attacher la WebView au frame

    downloadRemoteVersion();


    view->setUrl(QUrl("your_url"));
    layout->addWidget(view);  // Ajouter la WebView au layout

    // Appliquer le layout au frame
    frame->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPlayButtonClicked()
{
    // Chemin du fichier Trose.exe (dans le même répertoire que le launcher)
    QString executablePath = QDir::currentPath() + "/trose.exe";

    // Arguments à passer à Trose.exe
    QStringList arguments;
    arguments << "@TRIGGER_SOFT@" << "_server" << "your_ip";

    // Lancer le fichier avec les arguments
    process->start(executablePath, arguments);

    // Vérifier si le processus s'est bien lancé
    if (!process->waitForStarted()) {
        QMessageBox::warning(this, "Error", "Could not launch the game !");
    }
}

QString MainWindow::readLocalVersion()
{
    QSettings settings("version.ini", QSettings::IniFormat);
    QString version = settings.value("Version/current", "0.0.0").toString(); // Valeur par défaut
    return version;
}

void MainWindow::downloadRemoteVersion()
{
    // URL du fichier version distant sur le serveur
    QUrl url("your_ini_url");
    QNetworkRequest request(url);

    QNetworkReply* reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QFile file(QDir::currentPath() + "version.ini"); // Crée un fichier local

            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(data);
                file.close();
                qDebug() << "Fichier version.ini téléchargé avec succès.";

                // Lire la version distante à partir des données téléchargées
                QString remoteVersion = QString::fromUtf8(data).trimmed(); // On s'assure de retirer les espaces éventuels
                compareVersions(remoteVersion); // Comparer la version locale et distante
            } else {
                qDebug() << "Erreur lors de l'ouverture du fichier.";
            }
        } else {
            qDebug() << "Erreur lors du téléchargement de la version :" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::checkVersion()
{
    QString localVersion = readLocalVersion(); // Lire la version locale
    qDebug() << "Version locale :" << localVersion;
}

void MainWindow::compareVersions(const QString &remoteVersion)
{
    QString localVersion = readLocalVersion(); // Relire la version locale
    qDebug() << "Version distante :" << remoteVersion;

    // Séparer les parties des versions
    QStringList localParts = localVersion.split('.');
    QStringList remoteParts = remoteVersion.split('.');

    // Comparer les parties
    for (int i = 0; i < qMax(localParts.size(), remoteParts.size()); ++i) {
        int localPart = (i < localParts.size()) ? localParts[i].toInt() : 0; // Défaut à 0 si la partie n'existe pas
        int remotePart = (i < remoteParts.size()) ? remoteParts[i].toInt() : 0; // Défaut à 0 si la partie n'existe pas

        if (localPart < remotePart) {
            qDebug() << "Mise à jour disponible !"; // Version distante est plus récente
            downloadAndUnzip("your_patch_file_url" + remoteVersion + ".zip", QDir::currentPath());
            return; // Pas besoin de vérifier plus loin
        } else if (localPart > remotePart) {
            qDebug() << "Le jeu est à jour."; // Version locale est plus récente ou égale
            return; // Pas besoin de vérifier plus loin
        }
    }
    qDebug() << "Le jeu est à jour."; // Les versions sont égales
}

void MainWindow::downloadAndUnzip(const QString &url, const QString &outputDir)
{
    QUrl downloadUrl(url);
    QNetworkRequest request(downloadUrl);

    QNetworkReply *reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [this, reply, outputDir]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QString zipFilePath = outputDir + "/update.zip"; // Chemin pour le fichier ZIP

            // Enregistrer le fichier ZIP
            QFile zipFile(zipFilePath);
            if (zipFile.open(QIODevice::WriteOnly)) {
                zipFile.write(data);
                zipFile.close();
                qDebug() << "Fichier ZIP téléchargé avec succès.";

                // Décompresser le fichier ZIP
                unzipFile(zipFilePath, outputDir);
            } else {
                qDebug() << "Erreur lors de l'ouverture du fichier ZIP.";
            }
        } else {
            qDebug() << "Erreur lors du téléchargement du fichier ZIP :" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::unzipFile(const QString &zipFilePath, const QString &outputDir)
{
    // Utilisation de QProcess pour appeler un outil de décompression, comme unzip
    QProcess unzipProcess;
    QStringList arguments;
    arguments << "-o" << zipFilePath << "-d" << outputDir; // Options pour décompresser

    unzipProcess.start("unzip", arguments);
    unzipProcess.waitForFinished(); // Attendre que le processus se termine

    if (unzipProcess.exitStatus() == QProcess::NormalExit && unzipProcess.exitCode() == 0) {
        qDebug() << "Décompression réussie.";
        // Supprimer le fichier ZIP après la décompression
        if (QFile::remove(zipFilePath)) {
            qDebug() << "Fichier ZIP supprimé avec succès.";
        } else {
            qDebug() << "Erreur lors de la suppression du fichier ZIP.";
        }
    } else {
        qDebug() << "Erreur lors de la décompression :" << unzipProcess.readAllStandardError();
    }
}
