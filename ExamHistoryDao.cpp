#include "ExamHistoryDao.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

ExamHistoryDao::ExamHistoryDao()
{
    //创建user表
    QSqlQuery query;
    query.exec("create table if not exists examhistory("
               "subjectid integer,"
               "subjectname char(20),"
               "stuid integer,"
               "entertime datetime,"
               "leavetime datetime"
               ")");
}

bool ExamHistoryDao::addHistory(ExamHistoryEntity &ee)
{
    QSqlQuery query;
    query.prepare("insert into examhistory values(?,?,?,datetime('now'),datetime('now'))");
    query.bindValue(0, ee.subjectId);
    query.bindValue(1, ee.subjectName);
    query.bindValue(2, ee.stuid);
    
    return query.exec();
}

bool ExamHistoryDao::updateHistory(ExamHistoryEntity &ee)
{
    QSqlQuery query;
    query.prepare(
                "update examhistory set leavetime = datetime('now') "
                "where stuid = ? and subjectid = ?");
    query.bindValue(0, ee.stuid);
    query.bindValue(1, ee.subjectId);
    
    return query.exec();
}

bool ExamHistoryDao::selectAll(vector<ExamHistoryEntity> &v)
{
    QSqlQuery query;
    bool ok = query.exec("select * from examhistory");
    
    while(query.next())
    {
        ExamHistoryEntity ee;
        ee.subjectId = query.record().value("subjectid").toInt();
        ee.subjectName = query.record().value("subjectname").toString();
        ee.stuid = query.record().value("stuid").toInt();
        ee.enterTime = query.record().value("entertime").toDateTime();
        ee.leaveTime = query.record().value("leavetime").toDateTime();
        
        v.push_back(ee);
    }
    return ok;
}
