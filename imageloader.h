#ifndef IMAGELOADER_H
#define IMAGELOADER_H
#include<QThread>
#include<QList>
#include<QPixmap>
#include<QNetworkReply>
#include<QNetworkAccessManager>
#include<QTimer>

//#define WRITE_TO_FILE
#define IMAGEBUFFER_SIZE 5

class ImageLoader : public QObject // : public QThread
{
    Q_OBJECT
public:
    ImageLoader();
    QPixmap getPreviousImage(void);
    QPixmap getNextImage(void);
protected:
     void run();

private:
QTimer *checktimer;
void downloadImage(QString address);
void downloadHtmlData(QString address);

int imageindex;
QList<QPixmap> imagelist;

QNetworkAccessManager *nManager;
QNetworkAccessManager *nManagerhtml;

QString previousimage;
QString nextimage;
QString currentimage;
QByteArray bytes;
int filename_index;

private slots:
 void download_finished(QNetworkReply*);
 void downloadhtml_finished(QNetworkReply* reply);
 void checktimer_timeout(void);

signals:
 void first_image_received(void);

};

#endif // IMAGELOADER_H
