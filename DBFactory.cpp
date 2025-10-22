#include "DBFactory.h"
#include <QDebug>

DBFactory* DBFactory::instance = nullptr;

DBFactory *DBFactory::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DBFactory();
    }
    return instance;
}

void DBFactory::releaseInstance()
{
    if(instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

DBFactory::DBFactory()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sea.db");
    db.open();
    
    ehDao = new ExamHistoryDao();
}

DBFactory::DBFactory(const DBFactory &other)
{
    Q_UNUSED(other)
}

DBFactory::~DBFactory()
{
    db.close();
}
