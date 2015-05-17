#ifndef TTTREQUESTBASE_H
#define TTTREQUESTBASE_H

#include <QObject>
#include <QRunnable>

class TTTRequestBase : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TTTRequestBase(QObject *parent = 0);

signals:

public slots:

};

#endif // TTTREQUESTBASE_H
