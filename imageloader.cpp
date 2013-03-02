#include "imageloader.h"
#include<QDebug>
#include<QNetworkReply>
#include<QNetworkAccessManager>

#ifdef WRITE_TO_FILE
#include<QImageWriter>
#endif

#define CURRENT_IMAGE_LINK "snstatic.fi/webkuva/sarjis/560"
#define NEXT_IMAGE_LINK "/m/fingerpori/"
#define PREVIOUS_IMAGE_LINK "/m/fingerpori/"
#define IMAGELINK "snstatic.fi/webkuva/sarjis/560/"
//#define PAGELINK "http://www.hs.fi/fingerpori/"
#define PAGELINK "http://www.hs.fi/m/fingerpori/"

ImageLoader::ImageLoader()
{
    qDebug() << "ImageLoader::ImageLoader";
    nManager = new QNetworkAccessManager();
    nManagerhtml = new QNetworkAccessManager();

    connect(nManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(download_finished(QNetworkReply*)));
    connect(nManagerhtml,SIGNAL(finished(QNetworkReply*)),this,SLOT(downloadhtml_finished(QNetworkReply*)));
    imageindex=0;


    checktimer = new QTimer(this);
    checktimer->setSingleShot(true);
    checktimer->setInterval(1000);
    connect(checktimer,SIGNAL(timeout()),this,SLOT(checktimer_timeout()));

    checktimer->start();
    filename_index=0;

    }
QPixmap ImageLoader::getNextImage()
{
   if(imageindex>0) imageindex--;
   qDebug() << "ImageLoader::getNextImage" << imageindex;
   return imagelist.at(imageindex);
}
QPixmap ImageLoader::getPreviousImage()
{
   if(imageindex==imagelist.count()-1) return imagelist.at(imageindex);
   imageindex++;
   qDebug() << "ImageLoader::getPreviousImage" << imageindex;
   checktimer->start();
   return imagelist.at(imageindex);

}
void ImageLoader::checktimer_timeout()
{
    if(imagelist.count()==0) {downloadHtmlData(PAGELINK);return; }
    if((imagelist.count()-imageindex)<IMAGEBUFFER_SIZE) downloadHtmlData(QString(PAGELINK).append(previousimage));
}

void ImageLoader::downloadImage(QString address)
{
    qDebug() << "downloadImage " << address;
    QUrl url(address);
 nManager->get(QNetworkRequest(url));
    ;
}
void ImageLoader::downloadHtmlData(QString address)
{
    qDebug() << "downloadHtmlData " << address;
    QUrl url(address);
 nManagerhtml->get(QNetworkRequest(url));
}

void ImageLoader::download_finished(QNetworkReply* reply)
{
    qDebug() << "download_finished" << reply->error();

    if (reply->error() == QNetworkReply::NoError)
     {
     // read data from QNetworkReply here

     bytes = reply->readAll();
     qDebug() << "download_finished bytes " <<  bytes.count();

     QPixmap kuva;
     if(kuva.loadFromData(bytes,"",Qt::AutoColor)==true)
         qDebug("load ok");
     else {qDebug("load nok");return;}
     //kuva.scaledToHeight(271);
    // kuva.scaled(640,271,Qt::AspectRatioMode,Qt::FastTransformation);
    //if(currentpixmap) {scene->removeItem(currentpixmap);}
    //currentpixmap = scene->addPixmap(kuva->scaled(ui->graphicsView->size().width(),ui->graphicsView->size().height(),Qt::IgnoreAspectRatio,Qt::FastTransformation));


    imagelist.append(kuva);
    if(imagelist.count()==1) emit first_image_received();
    qDebug() << "imagecount" << imagelist.count();
    if((imagelist.count()-imageindex)<IMAGEBUFFER_SIZE) downloadHtmlData(QString(PAGELINK).append(previousimage));
#ifdef WRITE_TO_FILE
    QString filename;
    filename = "fingerpori_";
    filename.append(QString::number(filename_index,'f',0).rightJustified(4,'0'));//  ::number(filename_index,'f',0));
    filename.append(".jpg");
    qDebug() << filename;
    filename_index++;
    QImageWriter imagefile(filename,"jpg");
    imagefile.write(kuva.toImage());
#endif
    }
}

void ImageLoader::downloadhtml_finished(QNetworkReply *reply)
{
  qDebug() << "downloadhtml_finished" << reply->error();
  QString currentimage_prefix;
  if (reply->error() == QNetworkReply::NoError)
   {
   // read data from QNetworkReply here

   //QByteArray bytes = reply->readAll();
   QString htmldata = reply->readAll();
   qDebug() << "download_finished bytes " <<  htmldata.size();

   //"http://www.hs.fi/kuvat/iso_webkuva"
   int pos;
   pos = htmldata.indexOf(CURRENT_IMAGE_LINK,0,Qt::CaseSensitive);
   currentimage = htmldata.mid(pos+QString(CURRENT_IMAGE_LINK).size()+1,20);
   currentimage_prefix = htmldata.mid(pos-12,12);
   pos = htmldata.indexOf("previous",0,Qt::CaseSensitive);
   if(pos!=-1)
       {
       pos = htmldata.indexOf(PREVIOUS_IMAGE_LINK,pos,Qt::CaseSensitive);
       previousimage = htmldata.mid(pos+QString(PREVIOUS_IMAGE_LINK).size(),14);
       }
   else previousimage.clear();

   pos = htmldata.indexOf("next",0,Qt::CaseSensitive);
   if(pos!=-1)
       {
       pos = htmldata.indexOf(NEXT_IMAGE_LINK,pos,Qt::CaseSensitive);
       nextimage = htmldata.mid(pos+QString(NEXT_IMAGE_LINK).size(),14);
       }
   else nextimage.clear();

   if(!nextimage.compare(previousimage)) nextimage.clear();
  }
  qDebug() << previousimage << " " << currentimage << " " << nextimage;
  downloadImage(currentimage_prefix.append(IMAGELINK).append(currentimage));
}

