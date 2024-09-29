#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayButtonClicked();  // Slot pour le bouton Play

private:
    Ui::MainWindow *ui;
    QProcess *process;
    QNetworkAccessManager *manager; // Gestionnaire de requêtes réseau

    QString readLocalVersion(); // Lire la version locale
    void downloadRemoteVersion(); // Télécharger la version distante
    void checkVersion(); // Vérifier la version
    void compareVersions(const QString &remoteVersion); // Comparer la version locale et distante
    void downloadAndUnzip(const QString &url, const QString &outputDir); // Télécharge
    void unzipFile(const QString &zipFilePath, const QString &outputDir); // Unzip

};
#endif // MAINWINDOW_H
