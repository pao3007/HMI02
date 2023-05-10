#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <queue>
#ifdef _WIN32
#include<windows.h>
#endif
#include<iostream>
//#include<arpa/inet.h>
//#include<unistd.h>
//#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
//#include "ckobuki.h"
//#include "rplidar.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"
#include "robot.h"

#include <QJoysticks.h>

typedef struct
{

    int x;
    int y;
    int drawX;
    int drawY;

}Index;

typedef struct
{

    bool hladaj;
    int x;
    int y;
    int drawX;
    int drawY;

}Uloha;

typedef struct
{

    float x;
    float y;
    float angle;
    int translation;
    int numberOfScans;
    //MojeLaserData Data[1000];
    bool stop;
    bool moving;
    int numOfPoints;

}MojRobot;

typedef struct
{
    queue<Index> unvisited;
    Index current;
}Alg;

namespace Ui {
class MainWindow;
}

///toto je trieda s oknom.. ktora sa spusti ked sa spusti aplikacia.. su tu vsetky gombiky a spustania...
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool useCamera1;
  //  cv::VideoCapture cap;

    int actIndex;
    //    cv::Mat frame[3];

    cv::Mat frame[3];
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int processThisLidar(LaserMeasurement laserData);

    int processThisRobot(TKobukiData robotdata);

int processThisCamera(cv::Mat cameraData);

cv::Mat detectCircle(cv::Mat image);
int pole[4], xObject, yObject;
QPoint findObject();
cv::Mat newImage;
bool search=false, convert=false;
int checkAngle(int gyroAngle);
bool finder=false;
int iter=0;
QPoint objectPoint;

private slots:
    void on_pushButton_9_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_rem_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_image_clicked();

    void on_pushButton_clicked();
    void getNewFrame();
    void ShowContextMenu(const QPoint &pos);

private:

    //--skuste tu nic nevymazat... pridavajte co chcete, ale pri odoberani by sa mohol stat nejaky drobny problem, co bude vyhadzovat chyby
    Ui::MainWindow *ui;
     void paintEvent(QPaintEvent *event);// Q_DECL_OVERRIDE;
     int updateLaserPicture;
     LaserMeasurement copyOfLaserData;
     std::string ipaddress;
     Robot robot;
     TKobukiData robotdata;
     int datacounter;
     bool pushBtnImg,start_stop, navigujem, novaTrasa, pause;
     QTimer *timer;
     std::vector<std::vector<char>> mapaOrig;
     QImage mapaImageOrig;
     QSize sizeG;
     int scale,scale2, offsetImg;
     Index suradnice, polohaRobota, zaciatok, mapSize;
     std::vector<Uloha> ulohy;
     QImage image;

     vector<Index> path;
     vector<Index> pathPoints;
     float odchylka_pol;
     float battery;


     QJoysticks *instance;
      MojRobot mojRobot;

     int offset[8][2] = {
         {1, 0},
         {0, 1},
         {-1, 0},
         {0, -1},
         {-1, -1},
         {-1, 1},
         {1, -1},
         {1, 1}
                                              };
     double forwardspeed;//mm/s
     double rotationspeed;//omega/s
public slots:
     void setUiValues(double robotX,double robotY,double robotFi);
     void chod();
     void hladaj();
signals:
     void uiValuesChanged(double newrobotX,double newrobotY,double newrobotFi); ///toto nema telo


};



#endif // MAINWINDOW_H
