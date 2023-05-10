#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <math.h>
#include <queue>
///TOTO JE DEMO PROGRAM...AK SI HO NASIEL NA PC V LABAKU NEPREPISUJ NIC,ALE SKOPIRUJ SI MA NIEKAM DO INEHO FOLDERA
/// AK HO MAS Z GITU A ROBIS NA LABAKOVOM PC, TAK SI HO VLOZ DO FOLDERA KTORY JE JASNE ODLISITELNY OD TVOJICH KOLEGOV
/// NASLEDNE V POLOZKE Projects SKONTROLUJ CI JE VYPNUTY shadow build...
/// POTOM MIESTO TYCHTO PAR RIADKOV NAPIS SVOJE MENO ALEBO NEJAKY INY LUKRATIVNY IDENTIFIKATOR
/// KED SA NAJBLIZSIE PUSTIS DO PRACE, SKONTROLUJ CI JE MIESTO TOHTO TEXTU TVOJ IDENTIFIKATOR
/// AZ POTOM ZACNI ROBIT... AK TO NESPRAVIS, POJDU BODY DOLE... A NIE JEDEN,ALEBO DVA ALE BUDES RAD
/// AK SA DOSTANES NA SKUSKU


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //tu je napevno nastavena ip. treba zmenit na to co ste si zadali do text boxu alebo nejaku inu pevnu. co bude spravna
    ipaddress="127.0.0.1";
    //ipaddress="192.168.1.14";//192.168.1.11toto je na niektory realny robot.. na lokal budete davat "127.0.0.1"
  //  cap.open("http://192.168.1.11:8000/stream.mjpg");
    ui->setupUi(this);
    datacounter=0;
  //  timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(getNewFrame()));
    actIndex=-1;
    useCamera1=false;



    ui->pushButton_rem->setToolTip("Removes last added checkpoint");
    ui->pushButton_rem->setToolTipDuration(10000);
    datacounter=0;

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(ShowContextMenu(QPoint)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

cv::Mat MainWindow::detectCircle(cv::Mat image)
{
    cout << "zavolala sa funkcia na detekciu";
    cv::Mat gray;
    cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    medianBlur(gray, gray, 5);
    vector<cv::Vec3f> circles;
    HoughCircles(gray, circles, cv::HOUGH_GRADIENT_ALT, 1, gray.rows/16, 90, 0.9, 20, 100);

    for( size_t i = 0; i < circles.size(); i++ ){
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);

        // circle center
        cv::circle( image, center, 1,cv::Scalar(0,100,100), 3, cv::LINE_AA);
        // circle outline
        int radius = c[2];
        cv::circle( image, center, radius, cv::Scalar(0,0,255), 5, cv::LINE_AA);
        std::cout << c[0] << c[1] << c[2] <<"mamkruh\n\n";
        pole[0]=c[0];//x
        pole[1]=c[1];//y
        pole[2] = 1;
    }
return image;
}

QPoint MainWindow::findObject(){
    cout<<"zavolala sa funkcia na detekciu ";
    QPoint point2;
    convert=true;
    detectCircle(newImage);
    if(pole[2]==1){

        int pointX = pole[0];
        int highX = pointX+10;
        int lowX = pointX-10;
        int highAngle = highX*0.07;
        int lowAngle = lowX*0.07;
        if(highAngle>426*0.07){
            highAngle=360-(highAngle-426*0.07);
        }
        if(lowAngle>426*0.07){
            lowAngle=360-(lowAngle-426*0.07);
        }

        int angle;
        //int point = 853/64;
        for(int k=0;k<copyOfLaserData.numberOfScans/*360*/;k++){
            angle = copyOfLaserData.Data[k].scanAngle;
            if(angle<highAngle && angle>lowAngle){
                float angle2 = angle;
                float dist = copyOfLaserData.Data[k].scanDistance + 90;

                xObject = dist*cos(angle2);
                yObject = dist*sin(angle2);
                cout<<"Vzdialenost v mm = "<<dist<<" x = "<<xObject<<" y = "<<yObject<<endl;
            }
        }
        point2.setX(xObject);
        point2.setY(yObject);
    }else{
        point2.isNull();
        cout<<"nenasiel objekt";
    }

    return point2;
}

int MainWindow::checkAngle(int gyroAngle){
    int checkAngle;
    if(gyroAngle<0){
        checkAngle = gyroAngle+36000;
    }else{
        checkAngle = gyroAngle;
    }
    return checkAngle/100;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QSize size = this->size();
    static int previousHeight = 0;
    static int previousWidth = 0;
    ///prekreslujem obrazovku len vtedy, ked viem ze mam nove data. paintevent sa
    /// moze pochopitelne zavolat aj z inych dovodov, napriklad zmena velkosti okna
    ///
    ///
    ///
    ///

    //kopirovanie cvmat do qimage



    if(actIndex>-1)
    {
        image = QImage((uchar*)frame[actIndex].data, frame[actIndex].cols, frame[actIndex].rows, frame[actIndex].step, QImage::Format_RGB888  );//kopirovanie cvmat do qimage
            QImage im2 = image;
            if(convert){
            cv::Mat ni(image.height(), image.width(), CV_8UC3, (cv::Scalar*)image.scanLine(0));
            newImage = ni;
            }


    }



    painter.setBrush(Qt::black);//cierna farba pozadia(pouziva sa ako fill pre napriklad funkciu drawRect)
    QPen pero;
    pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
    pero.setWidth(3);//hrubka pera -3pixely
    pero.setColor(Qt::green);//farba je zelena
    QRect rect;

    rect= ui->frame->geometry();//ziskate porametre stvorca,do ktoreho chcete kreslit
    rect.translate(0,15);
    painter.drawRect(rect);

    QRect rectMini(15,ui->frame->pos().y()+20,ui->frame->geometry().width()/4,ui->frame->geometry().height()/4);
    ui->pushButton_image->setGeometry(3,7,ui->frame->geometry().width()/4,ui->frame->geometry().height()/4);
    ui->pushButton_image->setStyleSheet("background-color: rgba(255, 255, 255, 0);");



    static QColor rectColor(255, 0, 0, 75);
    painter.fillRect(rectMini,rectColor);


    float pomer = (float)size.width() / (float)size.height();

    if((previousHeight-size.height()) < 0 || (previousHeight-size.width()) < 0){
        if(!((1.75 > pomer) && (pomer > 1.79))){
            if(pomer < 1.75){
                int width = size.height()*1.77;
                resize(width,size.height());
            }
            if(pomer > 1.79){
                int height = size.width()/1.77;
                resize(size.width(),height);
            }
        }
         ui->pushButton_9->setGeometry((size.width()/2)-100,size.height()-90,201,31);
         ui->pushButton_rem->setGeometry(70,size.height()-90,100,31);
         ui->label->setGeometry(180,size.height()-90,300,31);
    }else if((previousHeight-size.height()) > 0 || (previousHeight-size.width()) > 0){
        if(!((1.75 > pomer) && (pomer > 1.79))){
            if(pomer < 1.75){
                int height = size.width()/1.77;
                resize(size.width(),height);
            }
            if(pomer > 1.79){
                int width = size.height()*1.77;
                resize(width,size.height());
            }
        }
         ui->pushButton_9->setGeometry((size.width()/2)-100,size.height()-90,201,31);
         ui->pushButton_rem->setGeometry(70,size.height()-90,100,31);
         ui->label->setGeometry(180,size.height()-90,300,31);

    }




    previousHeight = size.height();
    previousWidth = size.width();
    sizeG = ui->frame->size();





    if(!mapaImageOrig.isNull()){
        QImage mapaImage = mapaImageOrig;
        QPen pero2;
        pero2.setStyle(Qt::SolidLine);//styl pera - plna ciara
        pero2.setWidth(5);//hrubka pera -3pixely
        pero2.setColor(Qt::white);//farba je zelena

        QPainter painterMap(&mapaImage);
        painterMap.setPen(pero2);

        for(int i = 0;i < ulohy.size();i++){
            ///painterMap.drawEllipse(ulohy[i].drawX,ulohy[i].drawY,5,3);
            ///
            if(ulohy[i].hladaj){
                QString s = "H";
                s.append(char(i+'/0'));
                painterMap.drawText(ulohy[i].drawX-5,ulohy[i].drawY+5,s);
            }else{
                QString s = "C";
                s.append(char(i+'/0'));
                painterMap.drawText(ulohy[i].drawX-5,ulohy[i].drawY+5,s);
            }


            ///cout << ulohy[i].drawX << " " << ulohy[i].drawY << endl;
        }

        pero2.setStyle(Qt::SolidLine);//styl pera - plna ciara
        pero2.setWidth(2);//hrubka pera -3pixely
        pero2.setColor(Qt::red);//farba je zelena
        painterMap.setPen(pero2);
        int sizeX = 24;
        int sizeY = 17;
        painterMap.drawEllipse((polohaRobota.x * scale - sizeX/2),(polohaRobota.y * scale2 + offsetImg - sizeY/2),sizeX,sizeY);
        int px1 = (polohaRobota.x * scale);
        int py1 = (polohaRobota.y * scale2 + offsetImg);

        int px2 = px1 + (sizeX/2)*cos(mojRobot.angle);
        int py2 = py1 + (sizeY/2)*sin(mojRobot.angle);
        painterMap.drawLine(px1,py1,px2,py2);


        QColor color0 = Qt::black;


         for(int k=0;k<copyOfLaserData.numberOfScans;k++){
             if(((copyOfLaserData.Data[k].scanDistance/1000) < 0.50) && (copyOfLaserData.Data[k].scanDistance/1000) != 0 && (k < 35 || k > 240)){
                 float scanDist = copyOfLaserData.Data[k].scanDistance / 1000;
                 float pi1 = 3.14159;
                 float lidarAngle = copyOfLaserData.Data[k].scanAngle * (pi1/180.0);
                 float calAngle = mojRobot.angle - lidarAngle;
                 if(calAngle > 6.283185) calAngle -= 6.283185;


                 int px1l = ((polohaRobota.x + (int)((mojRobot.x + scanDist*cos(calAngle))/0.05)) * scale);
                 int py1l = ((polohaRobota.y + (int)((mojRobot.y + scanDist*sin(calAngle))/0.05)) * scale2 + offsetImg);

                 pero.setWidth(2);//hrubka pera -3pixely
                 pero.setColor(Qt::cyan);//farba je zelena
                 painterMap.setPen(pero);

                 painterMap.drawPoint(px1l,py1l);
             }



         }

         if(!objectPoint.isNull()){
             int px1l = ((polohaRobota.x + objectPoint.x()/50) * scale);
             int py1l = ((polohaRobota.y + objectPoint.y()/50) * scale2 + offsetImg);
             pero.setWidth(2);//hrubka pera -3pixely
             pero.setColor(Qt::magenta);//farba je zelena
             painterMap.setPen(pero);

             int sizeXc = 12;
             int sizeYc = 8;

             painterMap.drawEllipse(px1l-(sizeXc/2),py1l-(sizeYc/2),sizeXc,sizeYc);
         }



        if(pushBtnImg){
            ///cout << "Mapa" << endl;
            color0 = Qt::white;
            painter.drawImage(rect,mapaImage);
            painter.drawImage(rectMini,image.rgbSwapped());
            ui->label->setStyleSheet("QLabel { font-weight: bold; color : white; }");


        }else
        {
            ///cout << "Kamera" << endl;
            color0 = Qt::black;
            painter.drawImage(rect,image.rgbSwapped());
            painter.drawImage(rectMini,mapaImage);
            ui->label->setStyleSheet("QLabel { font-weight: bold; color : blue; }");

        }
        //15,ui->frame->pos().y()+20,ui->frame->geometry().width()/4,ui->frame->geometry().height()/4
            pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
            pero.setWidth(5);//hrubka pera -3pixely
            pero.setColor(color0);//farba je zelena
            painter.setPen(pero);
            QColor farba = Qt::black;
            farba.setAlpha(0);
            painter.setBrush(farba);

            painter.drawRect(22,size.height()-90-47,50,100);



            float batteryLevel = 0.5;
            painter.fillRect(27,size.height()-85-47+90*(1-batteryLevel),40,90*batteryLevel,Qt::green);

            pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
            pero.setWidth(5);//hrubka pera -3pixely
            pero.setColor(color0);//farba je zelena
            painter.setPen(pero);
            int help = size.height()-70;
            painter.drawLine(24,help,67,help);
            help = size.height()-106;
            painter.drawLine(24,help,67,help);

            batteryLevel = std::round(batteryLevel*100);

            help = size.height()-80;
            QString s = QString("%1").arg(batteryLevel);
            s.append("%");
            painter.drawText(37,help,s);

            if(mojRobot.stop){
                QFont font = painter.font();
                font.setPixelSize(50);
                painter.setFont(font);
                pero.setColor(Qt::red);
                painter.setPen(pero);
                painter.drawText(20,size.height()-450,500,500, 0, tr("WARNING!"));
                font.setPixelSize(30);painter.setFont(font);
                painter.drawText(20,size.height()-400,500,500, 0, tr("Obstacle in the way"));



            }








    }






}


/// toto je slot. niekde v kode existuje signal, ktory je prepojeny. pouziva sa napriklad (v tomto pripade) ak chcete dostat data z jedneho vlakna (robot) do ineho (ui)
/// prepojenie signal slot je vo funkcii  on_pushButton_9_clicked
void  MainWindow::setUiValues(double robotX,double robotY,double robotFi)
{

}

///toto je calback na data z robota, ktory ste podhodili robotu vo funkcii on_pushButton_9_clicked
/// vola sa vzdy ked dojdu nove data z robota. nemusite nic riesit, proste sa to stane
int MainWindow::processThisRobot(TKobukiData robotdata)
{
    battery = robotdata.Battery/255.0;

    static bool start = true;
    static int previousEncoderLeft = robotdata.EncoderLeft, previousEncoderRight = robotdata.EncoderRight;
    ///static double odometerLeft, odometerRight = 0;
    static float x = 0, y = 0;
    static float previousRads = 0;
    static queue<double> qxr, qyr;
    static double yr = 0, xr = 0;
    static double fi = 0;
    static float rads= 0;
    static int translation = 0;
    static int prev_translation = 0;
    static double arc_reg = 10000;
    double Pr = 2.8;
    int Pt = 500;
    static bool centered = true;
    double tTM = 0.000085292090497737556558;
    double diameter = 0.23;
    double pi1 = 3.14159265359;
    double finish = 0.05;
    static double e_fi = 0, e_pos = 0;




    polohaRobota.x = zaciatok.x + int(x/0.05);
    polohaRobota.y = zaciatok.y + int(y/0.05);
    ///cout << polohaRobota.x << " " << polohaRobota.y << endl;
    ///
    if(search){

             int inigyro = checkAngle(robotdata.GyroAngle);

             if(inigyro<180){
                 robot.setRotationSpeed(-0.2);
             }else{
                 robot.setRotationSpeed(0.2);
             }

             if(finder&&(checkAngle(robotdata.GyroAngle)<2||checkAngle(robotdata.GyroAngle)>358)){
                 robot.setRotationSpeed(0);
                 objectPoint = findObject();
                 finder=false;

             }else if(!finder&&iter==0){
                 robot.setRotationSpeed(0.5);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>62&&checkAngle(robotdata.GyroAngle)<66){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 1;
                 }
             }else if(!finder&&iter==1){
                 robot.setRotationSpeed(0.5);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>126&&checkAngle(robotdata.GyroAngle)<130){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 2;
                 }
             }else if(!finder&&iter==2){
                 robot.setRotationSpeed(0.5);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>190&&checkAngle(robotdata.GyroAngle)<192){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 3;
                 }
             }else if(!finder&&iter==3){
                 robot.setRotationSpeed(0.2);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>254&&checkAngle(robotdata.GyroAngle)<258){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 4;
                 }
             }else if(!finder&&iter==4){
                 robot.setRotationSpeed(0.5);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>318&&checkAngle(robotdata.GyroAngle)<322){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 5;
                 }
             }else if(!finder&&iter==5){
                 robot.setRotationSpeed(0.5);
                 cout<<"\n som v kroku "<<iter;
                 if(checkAngle(robotdata.GyroAngle)>22&&checkAngle(robotdata.GyroAngle)<26){
                     robot.setRotationSpeed(0);
                     objectPoint = findObject();
                     iter = 0;
                     search = false;
                 }

             }
             cout<<endl<<search;
             if(!search){
                 robot.setRotationSpeed(0);
                 ulohy.erase(ulohy.begin());
                 novaTrasa = true;
             }

         }



    if(!ulohy.empty() && novaTrasa){
        printf("IDEM0\n");

        ///int algMapa[mapSize.x][300] = {0};
        std::vector<std::vector<int>> algMapa(
                    mapaOrig.size(),
                    std::vector<int>(mapaOrig[0].size()));
        std::vector<std::vector<char>> mapa = mapaOrig;

        for(int k = 0; k < 6; k++){
            int i = 0;
            int j = 0;
                for (i=0; i < mapa[0].size(); i++)
                {
                    for (j = 0; j < mapa.size(); j++)
                    {
                        if(mapa[j][i] == '1'){

                            for (int k = 0; k < 8; k++)
                            {
                                int indexX = i + offset[k][0];
                                int indexY = j + offset[k][1];
                                if(indexX >= 0 && indexX < mapa[0].size() && indexY >=0 && indexY < mapa.size()){
                                    if(mapa[indexY][indexX] != '1')
                                        mapa[indexY][indexX] = '2';
                                }

                            }
                        }


                    }
                }

                for (int i = 0; i < mapa[0].size(); i++)
                {
                    for (int j = 0; j < mapa.size(); j++)
                    {
                        if(mapa[j][i] == '2')mapa[j][i] = '1';
                    }
                }
            }


        int idx[2];
        Index nodeIdx;
        Alg alg;

        if(ulohy.front().x > mapa[0].size() ){
            alg.current.x = mapa[0].size()-1;
        }else if (ulohy.front().x < 0 ){
            alg.current.x= 0;
        }else alg.current.x = ulohy.front().x;

        if(ulohy.front().y > mapa.size() ){
            alg.current.y = mapa.size()-1;
        }else if (ulohy.front().y < 0 ){
            alg.current.x = 0;
        }else alg.current.y = ulohy.front().y;

        idx[0] = alg.current.x;
        idx[1] = alg.current.y;


        for (int k = 0; k < 8; k++)
        {
            int indexX = alg.current.x + offset[k][0];
            int indexY = alg.current.y + offset[k][1];
            if(indexX >=0 && indexY >= 0 && indexX < mapa[0].size() && indexY < mapa.size()){

                mapa[indexY][indexX] = 'u';
                nodeIdx.x = indexX;
                nodeIdx.y = indexY;
                alg.unvisited.push(nodeIdx);
            }

        }
        while(!alg.unvisited.empty()){
            //cout << "alg : "<< alg.unvisited.size() << endl;
            nodeIdx = alg.unvisited.front();
            alg.unvisited.pop();
            alg.current.x = nodeIdx.x;
            alg.current.y = nodeIdx.y;
            int min = 999999;

            for (int k = 0; k < 8; k++)
            {
                int indexX = alg.current.x + offset[k][0];
                int indexY = alg.current.y + offset[k][1];
                if(indexX >=0 && indexY >= 0 && indexX < mapa[0].size() && indexY < mapa.size()){
                    if(mapa[indexY][indexX] == '0'){
                        mapa[indexY][indexX] = 'u';
                        nodeIdx.x = indexX;
                        nodeIdx.y = indexY;
                        alg.unvisited.push(nodeIdx);
                    }else if(algMapa[indexY][indexX] > 1 && !(indexX == ulohy.front().x && indexY == ulohy.front().y)){
                        if(algMapa[indexY][indexX] < min){
                            min = algMapa[indexY][indexX];
                        }
                    }else if(indexX == ulohy.front().x && indexY == ulohy.front().y){
                        min = 1;
                    }
                }


            }
            mapa[alg.current.y][alg.current.x] = 'v';
            algMapa[alg.current.y][alg.current.x] = min+1;
            ///std::cout<<mapa[alg.current.x][alg.current.y]<<std::endl;

            //std::cout<<test<<std::endl;
        }

        Index smallestIndex;
        int min = algMapa[polohaRobota.y][polohaRobota.x];
        alg.current.x = polohaRobota.x;
        alg.current.y = polohaRobota.y;
        bool end = false;
        cout << "poloha robota : " << polohaRobota.x << " + " << polohaRobota.y << endl;

        while(!end){
            cout << "poloha robota alg : " << alg.current.x << " + " << alg.current.y << endl;

            for (int k = 0; k < 8; k++)
            {
                int indexX = alg.current.x + offset[k][0];
                int indexY = alg.current.y + offset[k][1];
                if(indexX < mapa[0].size() && indexX >= 0 && indexY < mapa.size() && indexY >= 0){
                    if(algMapa[indexY][indexX]<min && algMapa[indexY][indexX] > 0){

                        min = algMapa[indexY][indexX];
                        smallestIndex.x = indexX;
                        smallestIndex.y = indexY;

                    }

                }

            }
            alg.current.x = smallestIndex.x;
            alg.current.y = smallestIndex.y;
            path.push_back(smallestIndex);
            ///cout << algMapa[smallestIndex.x][smallestIndex.y] << endl;
            if(algMapa[smallestIndex.y][smallestIndex.x] < 3) end = true;

        }
        Index prevIdx = path.front();
        Index prevSmer;

        prevSmer.x = 0;
        prevSmer.y = 0;

        for(int k = 0; k < path.size();k++){
            cout << "problem1" << endl;
            int dx = path[k].x - prevIdx.x;
            int dy = path[k].y - prevIdx.y;
             ///cout << "dx: " << dx << " x : " << prevSmer.x << " | dy: " << dy << " y: " << prevSmer.y;
            if(prevSmer.x != dx || prevSmer.y != dy){
               Index addThis;
               addThis.x = prevIdx.x;
               addThis.y = prevIdx.y;
               if(pathPoints.size() > 1){
                   int pk = pathPoints.size()-1;
                   Index test;
                   cout << "problem2 + " << pk << endl;
                   test.x = pathPoints[pk].x - addThis.x;
                   test.y = pathPoints[pk].y - addThis.y;
                   cout << "problem2b" << endl;
                   if(test.x < 2 && test.x > (-2) && test.y < 2 && test.y > (-2)){
                       if(mapa[pathPoints[pk].y][(pathPoints[pk].x + 1)] == 'v' && mapa[addThis.y][(addThis.x-1)] == '1'){
                           cout << "problem3" << endl;
                           addThis.x = pathPoints[pk].x;
                           addThis.y = (pathPoints[pk].y + 1);
                       }else if(mapa[pathPoints[pk].y][(pathPoints[pk].x - 1)] == 'v' && mapa[addThis.y][(addThis.x+1)] == '1'){
                           cout << "problem4" << endl;
                           addThis.x = pathPoints[pk].x;
                           addThis.y = (pathPoints[pk].y - 1);
                       }else if(mapa[pathPoints[pk].y+1][(pathPoints[pk].x)] == 'v' && mapa[addThis.y-1][addThis.x] == '1'){
                           cout << "problem5" << endl;
                           addThis.x = (pathPoints[pk].x+1);
                           addThis.y = pathPoints[pk].y;
                       }else if(mapa[(pathPoints[pk].y-1)][(pathPoints[pk].x)] == 'v' && mapa[(addThis.y+1)][addThis.x] == '1'){
                           cout << "problem6" << endl;
                           addThis.x = (pathPoints[pk].x-1);
                           addThis.y = pathPoints[pk].y;
                       }
                       cout << "problem7" << endl;
                       pathPoints.erase(pathPoints.begin()+pk);

                   }

               }
               cout << "problem8" << endl;
               pathPoints.push_back(addThis);

               //cout << " => TARGET";
            }
            //cout << endl;
            cout << "problem9" << endl;
            prevIdx.x = path[k].x;
            prevIdx.y = path[k].y;
            prevSmer.x = dx;
            prevSmer.y = dy;

        }
        Index addThis;
        addThis.x = idx[0];
        addThis.y = idx[1];
        pathPoints.push_back(addThis);
        pathPoints.erase(pathPoints.begin());

        float cellSize = 0.05;
        Index start;
        start.x = zaciatok.x;
        start.y = zaciatok.y;

        while(!qyr.empty()){
            qyr.pop();
            qxr.pop();
        }


        for(int i = 0; i < pathPoints.size();i++){
           double testX = pathPoints[i].x*cellSize-start.x*cellSize;
           double testY = pathPoints[i].y*cellSize-start.y*cellSize;
           cout << testX << " | " << testY << endl;

           qyr.push(testY);
           qxr.push(testX);


        }


        //////////////////
        ///ofstream occGridALG("C:/Users/lukac/Desktop/HMI/HMI02/occGridALG.txt");
        ///ofstream occGridALG2("C:/Users/lukac/Desktop/HMI/HMI02/occGridALG2.txt");
        ofstream occGridALG("C:/Users/pao/Desktop/kurva/dsa/occGridALG.txt");
        ofstream occGridALG2("C:/Users/pao/Desktop/kurva/dsa/occGridALG2.txt");
        printf("Zapisujem do mapy");

        for (int i = 0; i < mapa[0].size(); i++) {
            for (int j = 0; j < mapa.size(); j++) {


                bool pathWriten = false;
                if(polohaRobota.x == i && polohaRobota.y == j){
                    pathWriten = true;
                    occGridALG << 'R';
                }

                for(int k = 0; k < pathPoints.size();k++){
                    if(pathWriten){
                        break;
                    }
                    if(pathPoints[k].x == i && pathPoints[k].y == j){
                        occGridALG << 'T';
                        pathWriten = true;
                        ///path.erase(path.begin() + k);
                    }


                }


                for(int k = 0; k < path.size();k++){
                    if(pathWriten){
                        break;
                    }
                    if(path[k].x == i && path[k].y == j){
                        occGridALG << 'X';
                        pathWriten = true;
                        ///path.erase(path.begin() + k);
                    }

                }


                if(!pathWriten) occGridALG << mapa[j][i];

                if(algMapa[j][i] > 2){

                    occGridALG2 << 'U';
                }else occGridALG2 << algMapa[j][i];

                ///std::cout << i << ":" << j << " = " << algMapa[i][j] << std::endl;



            }
            occGridALG << endl;
            occGridALG2 << endl;
        }
        occGridALG.close();
        occGridALG2.close();
        novaTrasa = false;
        navigujem = true;
        pause = false;
        path.clear();
        pathPoints.clear();



    }

    if(!navigujem){
        if(!ulohy.empty()){
            ui->pushButton_9->setText("START NAVIGATION");
             ui->label->setText("YOU CAN START YOUR TASKS");
            start_stop = true;
        }else {
            ui->pushButton_9->setText("NO POINTS SELECTED");
            ui->label->setText("PLEASE SELECT TASKS ON MAP");
        }
    }



    ///TU PISTE KOD... TOTO JE TO MIESTO KED NEVIETE KDE ZACAT,TAK JE TO NAOZAJ TU. AK AJ TAK NEVIETE, SPYTAJTE SA CVICIACEHO MA TU NATO STRING KTORY DA DO HLADANIA XXX



            if(abs(previousEncoderLeft - robotdata.EncoderLeft) > 10000){
               printf("\nLeft encoder pretec\n");
               if(previousEncoderLeft > robotdata.EncoderLeft){
                   previousEncoderLeft -= 65535;
               }else if(previousEncoderLeft < robotdata.EncoderLeft) previousEncoderLeft += 65535;

            }
            if(abs(previousEncoderRight - robotdata.EncoderRight) > 10000){
               printf("\nRight encoder pretec\n");
               if(previousEncoderRight > robotdata.EncoderRight){
                   previousEncoderRight -= 65535;
               }else if(previousEncoderRight < robotdata.EncoderRight) previousEncoderRight += 65535;

            }

            float rightWheel = tTM*(robotdata.EncoderRight - previousEncoderRight);
            float leftWheel = tTM*(robotdata.EncoderLeft - previousEncoderLeft);

            if((rightWheel != 0) || (leftWheel != 0)){
                mojRobot.moving = true;
            }else if((rightWheel == 0) && (leftWheel == 0)) mojRobot.moving = false;


            previousEncoderLeft = robotdata.EncoderLeft;
            previousEncoderRight = robotdata.EncoderRight;

            if(translation == 0){
                rads = (robotdata.GyroAngle/100.0) * (pi1/180.0);
            }else {
                rads += (rightWheel - leftWheel)/diameter;
                if(rads > (6.283185/2)){
                    rads = -(6.283185/2) + (rads-(6.283185/2));
                }else if(rads < -(6.283185/2)){
                    rads = (6.283185/2) + (rads + (6.283185/2));
                }
            }

            if((rightWheel - leftWheel) != 0 && translation != 0){
                x += ((diameter*(rightWheel + leftWheel)) / (2.0*(rightWheel - leftWheel)))*(sin(rads) - sin(previousRads));
                y -= ((diameter*(rightWheel + leftWheel)) / (2.0*(rightWheel - leftWheel)))*(cos(rads) - cos(previousRads));

            }else{
                x += ((rightWheel + leftWheel)/2.0)*cos(rads);
                y += ((rightWheel + leftWheel)/2.0)*sin(rads);

            }

            previousRads = rads;

            //printf("\n%d",mojRobot.stop);

        if(!search){
            ///POLOHOVANIE
            if(!mojRobot.stop && !pause){
                ///cout << "POLOHOVANIE" << endl;

                if(!qyr.empty()){
                    yr = qyr.front();
                    xr = qxr.front();
                    // cout << xr << " + " << yr << endl;
                }



                fi = atan2(yr-y,xr-x);

                e_fi = fi - rads;
                if(e_fi > (6.283185/2)){
                    e_fi -= 6.283185;
                }else if(e_fi < (-6.283185/2)){
                    e_fi +=6.283185;
                }


                e_pos =sqrt(pow((xr-x),2)+pow((yr-y),2));
                odchylka_pol = e_pos;
                float translation_reg = Pt * e_pos + 50;
                if(e_fi != 0){
                    arc_reg = 100/e_fi;
                    if(arc_reg > 35000) arc_reg = 35000;
                }else arc_reg = 35000;






                if(abs(e_fi) >= 0.2) centered = false;

                if((abs(e_fi) < 0.2) && centered){


                    if(e_pos < finish){
                        translation = 0;

                        if(!qyr.empty()){
                            qyr.pop();
                            qxr.pop();
                            mojRobot.numOfPoints = qxr.size();
                            if(qyr.empty() && navigujem && !ulohy.empty()){

                                if(ulohy.front().hladaj){
                                    novaTrasa = false;
                                    search=true;
                                    finder=true;
                                }else{
                                    novaTrasa = true;
                                    ulohy.erase(ulohy.begin());
                                }



                                if(ulohy.empty()){
                                    start_stop = false;
                                    navigujem = false;
                                    pause = true;
                                    novaTrasa = false;
                                    ui->pushButton_9->setText("NO POINTS SELECTED");
                                    ui->label->setText("PLEASE SELECT TASKS ON MAP");


                                }
                            }

                        }

                    }else{

                        if(translation_reg > 400){
                            translation_reg = 400;
                        }

                        if(translation <= translation_reg){
                            translation += 5;
                        }else if(translation > translation_reg) translation -= 5;

                        if(translation > translation_reg) translation = translation_reg;
                        if(translation < 0) translation = 0;
                    }

                    robot.setArcSpeed(translation,arc_reg);


                }else{
                    translation -= 10;
                    if(translation < 0) translation = 0;
                    if(translation > 0) robot.setTranslationSpeed(translation);

                    if(translation == 0){
                        double rotacia = Pr*e_fi;
                         if(e_pos > finish){
                            if(rotacia > 3.14159/3) rotacia = 3.14159/3;
                            if(rotacia < -3.14159/3) rotacia = -3.14159/3;
                            robot.setRotationSpeed(rotacia);
                         }


                    }
                    if(abs(e_fi) < 0.03) centered = true;
                }
            }else if(!ulohy.empty()){
                translation -= 50;
                if (translation < 0) translation = 0;
                if(translation > 0) robot.setArcSpeed(translation,arc_reg);

                if(translation == 0){

                    if(!qyr.empty()){
                        yr = qyr.front();
                        xr = qxr.front();
                        // cout << xr << " + " << yr << endl;
                    }



                    fi = atan2(yr-y,xr-x);

                    double e_fi = fi - rads;
                    if(e_fi > (6.283185/2)){
                        e_fi -= 6.283185;
                    }else if(e_fi < (-6.283185/2)){
                        e_fi +=6.283185;
                    }

                    double rotacia = Pr*e_fi;
                    if(abs(e_fi) > 0.03){
                        if(rotacia > 3.14159/3) rotacia = 3.14159/3;
                        if(rotacia < -3.14159/3) rotacia = -3.14159/3;

                        robot.setRotationSpeed(rotacia);
                    }else robot.setRotationSpeed(0);



                }
                ///printf("\nEMERGENCY STOP");
            }
        }



        ///emit uiValuesChanged(xr, yr, rads);


        datacounter++;
        prev_translation = translation;

        mojRobot.angle = rads;
        mojRobot.x = x;
        mojRobot.y = y;
        mojRobot.translation = translation;

        cout << e_fi << endl;



        return 0;

}

///toto je calback na data z lidaru, ktory ste podhodili robotu vo funkcii on_pushButton_9_clicked
/// vola sa ked dojdu nove data z lidaru
int MainWindow::processThisLidar(LaserMeasurement laserData)
{


    memcpy( &copyOfLaserData,&laserData,sizeof(LaserMeasurement));
    //tu mozete robit s datami z lidaru.. napriklad najst prekazky, zapisat do mapy. naplanovat ako sa prekazke vyhnut.
    // ale nic vypoctovo narocne - to iste vlakno ktore cita data z lidaru
    updateLaserPicture=1;
    int i = 0;
    for(int k=0;k<copyOfLaserData.numberOfScans;k++){

        if(((copyOfLaserData.Data[k].scanDistance/1000) < 0.40) && (copyOfLaserData.Data[k].scanDistance/1000) != 0 && (k < 40 || k > 235))i++;
       ///if(copyOfLaserData.Data[k].scanAngle > 88 && copyOfLaserData.Data[k].scanAngle < 92) cout << copyOfLaserData.Data[k].scanDistance/1000 << endl;
    }
    if(i > 0){
        mojRobot.stop = true;
    }else mojRobot.stop = false;

    //update();//tento prikaz prinuti prekreslit obrazovku.. zavola sa paintEvent funkcia


    return 0;

}

///toto je calback na data z kamery, ktory ste podhodili robotu vo funkcii on_pushButton_9_clicked
/// vola sa ked dojdu nove data z kamery
int MainWindow::processThisCamera(cv::Mat cameraData)
{

    cameraData.copyTo(frame[(actIndex+1)%3]);//kopirujem do nasej strukury
    actIndex=(actIndex+1)%3;//aktualizujem kde je nova fotka
    updateLaserPicture=1;
    update();
    return 0;
}
void MainWindow::on_pushButton_9_clicked() //start button
{
        static bool start = false;
    if(!start){
        polohaRobota.x = 100;
        polohaRobota.y = 8;
        zaciatok.x = polohaRobota.x;
        zaciatok.y = polohaRobota.y;
        forwardspeed=0;
        rotationspeed=0;
        //tu sa nastartuju vlakna ktore citaju data z lidaru a robota
        connect(this,SIGNAL(uiValuesChanged(double,double,double)),this,SLOT(setUiValues(double,double,double)));

        ///setovanie veci na komunikaciu s robotom/lidarom/kamerou.. su tam adresa porty a callback.. laser ma ze sa da dat callback aj ako lambda.
        /// lambdy su super, setria miesto a ak su rozumnej dlzky,tak aj prehladnost... ak ste o nich nic nepoculi poradte sa s vasim doktorom alebo lekarnikom...
        robot.setLaserParameters(ipaddress,52999,5299,/*[](LaserMeasurement dat)->int{std::cout<<"som z lambdy callback"<<std::endl;return 0;}*/std::bind(&MainWindow::processThisLidar,this,std::placeholders::_1));
        robot.setRobotParameters(ipaddress,53000,5300,std::bind(&MainWindow::processThisRobot,this,std::placeholders::_1));
        //---simulator ma port 8889, realny robot 8000
        robot.setCameraParameters("http://"+ipaddress+":8889/stream.mjpg",std::bind(&MainWindow::processThisCamera,this,std::placeholders::_1));

        ///ked je vsetko nasetovane tak to tento prikaz spusti (ak nieco nieje setnute,tak to normalne nenastavi.cize ak napr nechcete kameru,vklude vsetky info o nej vymazte)
        robot.robotStart();



        //ziskanie joystickov
        instance = QJoysticks::getInstance();


        /// prepojenie joysticku s jeho callbackom... zas cez lambdu. neviem ci som to niekde spominal,ale lambdy su super. okrem toho mam este rad ternarne operatory a spolocneske hry ale to tiez nikoho nezaujima
        /// co vas vlastne zaujima? citanie komentov asi nie, inak by ste citali toto a ze tu je blbosti
        connect(
            instance, &QJoysticks::axisChanged,
            [this]( const int js, const int axis, const qreal value) { if(/*js==0 &&*/ axis==1){forwardspeed=-value*300;}
                if(/*js==0 &&*/ axis==0){rotationspeed=-value*(3.14159/2.0);}}
        );
    ///////////////////////////////////////////////////////////////////////////////
        std::string eachrow;
        ///std::ifstream myfile("C:/Users/lukac/Desktop/HMI/HMI02/idealOccGrid2.txt");
        std::ifstream myfile("C:/Users/pao/Desktop/kurva/dsa/idealOccGrid2.txt");
        int check = 0;


        while (std::getline(myfile, eachrow))
        {
           std::vector<char> row;

           for (char &x : eachrow)
           {
               if (x != ' ')row.push_back(x);
               if(x == '1')check++;
           }
            if(check > 0)
            {
                mapaOrig.push_back(row);
                ///cout << "idem" << endl;
            }
            check = 0;

        }
        scale = 3;
        scale2 = 2;
        offsetImg = 50;

        mapaImageOrig = QImage(mapaOrig[1].size() * scale,mapaOrig.size() * scale + offsetImg, QImage::Format_RGBA64);
        mapaImageOrig.fill(Qt::black);
        QPainter painterMap(&mapaImageOrig);

        std::vector< std::vector<int> >::const_iterator row;
        std::vector<int>::const_iterator col;
        QPen pero;
        pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
        pero.setWidth(5);//hrubka pera -3pixely
        pero.setColor(Qt::green);//farba je zelena
        painterMap.setPen(pero);

        int j,i;


        for (i = 0; i < mapaOrig.size(); i++)
           {
               for (j = 0; j < mapaOrig[i].size(); j++)
               {
                   if(mapaOrig[i][j] == '1'){
                       pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
                       pero.setWidth(5);//hrubka pera -3pixely
                       pero.setColor(Qt::green);//farba je zelena
                       painterMap.setPen(pero);
                       painterMap.drawPoint(j * scale,i * scale2 + offsetImg);
                   }else{
                       pero.setStyle(Qt::SolidLine);//styl pera - plna ciara
                       pero.setWidth(5);//hrubka pera -3pixely
                       pero.setColor(Qt::black);//farba je zelena
                       painterMap.setPen(pero);
                       painterMap.drawPoint(j * scale,i * scale2 + offsetImg);
                   }

               }

           }
        start = true;

    }

    if(start_stop && !ulohy.empty()){
        cout << "START ON" << endl;
        ui->pushButton_9->setStyleSheet("background-color: grey");
        start_stop = false;
        robot.setTranslationSpeed(0);
        if(!navigujem){
            novaTrasa = true;
            pause = false;
            ui->pushButton_9->setText("PAUSE NAVIGATION");
            ui->label->setText("NAVIGATION TO TASK IS ONGOING");
        }else {
            ui->pushButton_9->setText("PAUSE NAVIGATION");
            ui->label->setText("NAVIGATION TO TASK IS ONGOING");
            pause = false;
        }




    }else {
        cout << "START OFF" << endl;
        ui->pushButton_9->setStyleSheet("background-color: white");
        start_stop = true;
        if(navigujem){
            pause = true;
            ui->pushButton_9->setText("RESUME NAVIGATION");
            ui->label->setText("NAVIGATION TO TASK IS PAUSED");
        }else if(!ulohy.empty()){
            ui->pushButton_9->setText("START NAVIGATION");
            ui->label->setText("YOU CAN START YOUR TASKS");
        }else {
            ui->pushButton_9->setText("NO POINTS SELECTED");
            ui->label->setText("PLEASE SELECT TASKS ON MAP");
            start_stop = false;
        }


    }

//////////////////////////





}

void MainWindow::on_pushButton_2_clicked() //forward
{
    //pohyb dopredu
    robot.setTranslationSpeed(500);

}

void MainWindow::on_pushButton_3_clicked() //back
{
    robot.setTranslationSpeed(-250);

}

void MainWindow::on_pushButton_6_clicked() //left
{
robot.setRotationSpeed(3.14159/2);

}

void MainWindow::on_pushButton_5_clicked()//right
{
robot.setRotationSpeed(-3.14159/2);

}

void MainWindow::on_pushButton_4_clicked() //stop
{
    robot.setTranslationSpeed(0);

}

void MainWindow::on_pushButton_image_clicked() //stop
{
    if(pushBtnImg){
        pushBtnImg = false;
    }else pushBtnImg = true;
    update();

}

void MainWindow::on_pushButton_rem_clicked()
{
    if(!ulohy.empty() && pause){
        ulohy.pop_back();
    }

    if(ulohy.empty()){
        start_stop = false;
        navigujem = false;
        pause = true;
        novaTrasa = false;
        ui->pushButton_9->setText("NO POINTS SELECTED");
        ui->label->setText("PLEASE SELECT TASKS ON MAP");


    }
}




void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::getNewFrame()
{

}

void MainWindow::chod(){
    Uloha uloha;
    uloha.x = suradnice.x;
    uloha.y = suradnice.y;
    uloha.drawX = suradnice.drawX;
    uloha.drawY = suradnice.drawY;
    uloha.hladaj = false;
    ulohy.push_back(uloha);

    std::cout<<"chod sem"<<std::endl;
    update();
}

void MainWindow::hladaj(){
    Uloha uloha;
    uloha.x = suradnice.x;
    uloha.y = suradnice.y;
    uloha.drawX = suradnice.drawX;
    uloha.drawY = suradnice.drawY;
    uloha.hladaj = true;
    ulohy.push_back(uloha);

    std::cout<<"hladaj tu"<<std::endl;
    update();
}

void MainWindow::ShowContextMenu(const QPoint &pos)
{
    if(pushBtnImg){

        float pomerW = float(mapaImageOrig.width())/float(sizeG.width());
        float pomerH = float(mapaImageOrig.height())/float(sizeG.height());

        int drawPx = int(pomerW * float(pos.x()-10));
        int drawPy = int(pomerH * float(pos.y()-25));

        int x = int(float(drawPx)/float(scale));
        int y = int((float(drawPy)/float(scale2)))-offsetImg/2;
        int ok = 0;
        int i = 1;
        std::cout<<"x:"<<x<<"y:"<<y<<std::endl;
        suradnice.x = x;
        suradnice.y = y;
        suradnice.drawX = drawPx;
        suradnice.drawY = drawPy;

        if(y >= 0 && y < mapaOrig.size()){
            while((x-i) >= 0){
                if(mapaOrig[y][x-i] == '1'){
                    ok++;
                    break;
                }
                i++;
            }
            cout << "dsa1" << endl;
            i = 1;
            while((x+i) < mapaOrig[0].size()){
                if(mapaOrig[y][x+i] == '1'){
                    ok++;
                    break;
                }
                i++;
            }
            cout << "dsa2" << endl;
            i = 1;
            while((y-i) >= 0){
                if(mapaOrig[y-i][x] == '1'){
                    ok++;
                    break;
                }
                i++;
            }
            cout << "dsa3" << endl;
            i = 1;
            while((y+i) < mapaOrig.size()){
                if(mapaOrig[y+i][x] == '1'){
                    ok++;
                    break;
                }
                i++;
            }
            cout << "dsa4" << endl;

            cout << "ok :" << ok << endl;
            if(ok == 4){
                QMenu contextMenu(tr("Context menu"), this);

                QAction action1("Chod sem (C)", this);
                QAction action2("Hladaj tu (H)", this);
                connect(&action1, SIGNAL(triggered()), this, SLOT(chod()));
                connect(&action2, SIGNAL(triggered()), this, SLOT(hladaj()));
                contextMenu.addAction(&action1);
                contextMenu.addAction(&action2);
                ///std::cout<<"x:"<<pos.x()<<" y:"<<pos.y()<<" size:"<<mapaImage.width()<<"x"<< mapaImage.height()<<std::endl;
                contextMenu.exec(mapToGlobal(pos));
        }




        }








    }

}
