#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv.hpp>
#include <vector>
#include <QTimer>
#include <face.hpp>
#include <map>

using namespace cv;
using namespace std;
using namespace cv::face;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onTimeout();//定时器槽函数
    void on_pushButton_in_clicked();
    
    void on_pushButton_out_clicked();
    
private:
    void addLabels();//添加头像QLabel对象
    void findFace();//找脸
    void showImage();//显示图像
    void initReconigzer();//初始化人脸识别器
    void studyFace(int id);//学习人脸
    int checkFace();//识别人脸  返回学生考号
    void randSeet(int id);//随机一个座位
    bool repeatSeet(QLabel* label);//判断随机的座位是否重复，发现重复返回true
private:
    Ui::MainWindow *ui;
    CascadeClassifier cc;//级联分类器
    vector<Rect> faces;//找到的人脸
    VideoCapture vc;
    Mat frame;//存放一帧视频
    QTimer timer;//定时器
    Ptr<FaceRecognizer> recognizer;//人脸识别器
    map<int, QLabel*> stus;//保存学生id和座位信息
};
#endif // MAINWINDOW_H
