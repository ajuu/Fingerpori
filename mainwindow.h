#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include<QString>
#include<QtNetwork/QNetworkAccessManager>
#include<QUrl>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QGraphicsScene>
#include<QGraphicsPixmapItem>
#include<QPropertyAnimation>
#include<imageloader.h>
#include<QTimeLine>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private:
    Ui::MainWindow *ui;
    ImageLoader *imgloader;

    QGraphicsScene *scene;
    QGraphicsPixmapItem *currentpixmap;
    QGraphicsPixmapItem *nextpixmap;

    QTimeLine *anim;


private slots:
   void on_pushButton_previous_clicked();
   void on_pushButton_next_clicked();
   void on_pushButton_exit_clicked();
   void show_first_image();
   void timeline_change(int);
   void timeline_finished(void);
};

#endif // MAINWINDOW_H
