#ifndef EXAMHISTORYDAO_H
#define EXAMHISTORYDAO_H

#include <QString>
#include <QDateTime>
#include <vector>
#include <QSqlDatabase>

using namespace std;

struct ExamHistoryEntity
{
    int subjectId;//考场号
    QString subjectName;//科目名称
    int stuid;//学生考号
    QDateTime enterTime;//进场时间
    QDateTime leaveTime;//离场时间
};

class ExamHistoryDao
{
public:
    ExamHistoryDao();
    
    bool addHistory(ExamHistoryEntity& ee);//添加时间
    bool updateHistory(ExamHistoryEntity& ee);//更新离开时间
    bool selectAll(vector<ExamHistoryEntity>& v);//查询所有考试场次的时间
    //bool selectBySubject(int id, vector<ExamHistoryEntity>& v);//根据科目id查
    //bool selectByStu(int id, vector<ExamHistoryEntity>& v);//根据学生id查
    
};

#endif // EXAMHISTORYDAO_H
