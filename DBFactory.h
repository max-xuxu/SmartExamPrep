#ifndef DBFACTORY_H
#define DBFACTORY_H

#include <QSqlDatabase>
#include "ExamHistoryDao.h"

using namespace std;

class DBFactory
{
public:
    static DBFactory* getInstance();
    static void releaseInstance();
    inline ExamHistoryDao* getExamHistoryDao(){return ehDao;}

private:
    DBFactory();
    DBFactory(const DBFactory& other);
    ~DBFactory();
    
    static DBFactory* instance;
    QSqlDatabase db;
    ExamHistoryDao* ehDao;

};

#endif // DBFACTORY_H
