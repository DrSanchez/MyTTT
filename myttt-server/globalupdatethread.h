#ifndef GLOBALUPDATETHREAD_H
#define GLOBALUPDATETHREAD_H

#include <QObject>
#include <QRunnable>

class GlobalUpdateThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit GlobalUpdateThread(QObject *parent = 0);

    void addSocket(int sock);
    void setBytes(char * data);
    void clearSocketSet();

signals:

public slots:

protected:
    void run();

private:
    //private data members
    QByteArray * _bytes;
    QList<int> * _socketsToSend;

};

#endif // GLOBALUPDATETHREAD_H
