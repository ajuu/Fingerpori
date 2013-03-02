#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QByteArray>
#include<QString>
#include <QtCore/QCoreApplication>
#include<QPixmap>
#include<imageloader.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    currentpixmap=NULL;
    imgloader = new ImageLoader();
    connect(imgloader,SIGNAL(first_image_received()),this,SLOT(show_first_image()));


    anim = new QTimeLine(500,this);
    anim->setCurveShape(QTimeLine::EaseOutCurve);
    connect(anim,SIGNAL(frameChanged(int)),this,SLOT(timeline_change(int)));
    connect(anim,SIGNAL(finished()),this,SLOT(timeline_finished()));
}

MainWindow::~MainWindow()
{
    delete imgloader;
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}



void MainWindow::on_pushButton_previous_clicked()
{
    QPixmap kuva;

    kuva = imgloader->getPreviousImage();

    qDebug() << "on_pushButton_previous_clicked addpixmap";
    nextpixmap = scene->addPixmap(kuva.scaled(ui->graphicsView->size().width(),ui->graphicsView->size().height(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
    nextpixmap->setPos(-640,0);
    anim->stop();
    anim->setFrameRange(-640,0);
    anim->start();
}

void MainWindow::timeline_change(int value)
{
    nextpixmap->setPos(value,0);
}
void MainWindow::timeline_finished()
{
    scene->removeItem(currentpixmap);
    currentpixmap = nextpixmap;
    qDebug() << "timeline_finished nof items" << scene->items().count();
}

void MainWindow::on_pushButton_next_clicked()
{

    QPixmap kuva;
    kuva = imgloader->getNextImage();// return;
    qDebug() << "on_pushButton_next_clicked addpixmap";

    nextpixmap = scene->addPixmap(kuva.scaled(ui->graphicsView->size().width(),ui->graphicsView->size().height(),Qt::IgnoreAspectRatio,Qt::FastTransformation));
    nextpixmap->setPos(640,0);
    anim->stop();
    anim->setFrameRange(640,0);
    anim->start();
}

void MainWindow::on_pushButton_exit_clicked()
{
     QCoreApplication::quit();
}
void MainWindow::show_first_image()
{

    qDebug() << "show_first_image addpixmap";
    scene->clear();
    currentpixmap = scene->addPixmap(imgloader->getNextImage().scaled(ui->graphicsView->size().width(),ui->graphicsView->size().height(),Qt::IgnoreAspectRatio,Qt::FastTransformation));;
}
