#ifndef SCIDBASEMODEL_H
#define SCIDBASEMODEL_H

#include <QAbstractTableModel>
#include <QDir>
#include <QRegularExpression>
#include <QWidget>

class ScidBaseModel : public QAbstractTableModel
{
public:
    explicit ScidBaseModel(QWidget *parent = nullptr);
    void setInfo(QString dir, uint number);
protected:
    int columnCount(const QModelIndex &parent ) const;
    int rowCount( const QModelIndex &parent)const;
    QVariant data ( const QModelIndex &index, int role) const;

private:
    QString mDir;
    int mNumber;
    QWidget *mWid;

};

#endif // SCIDBASEMODEL_H
