#ifndef SCIDBASEMODEL_H
#define SCIDBASEMODEL_H

#include <QAbstractTableModel>
#include <QDir>
#include <QRegularExpression>
#include <QWidget>
#include <QVariant>
class ScidBaseModel : public QAbstractTableModel
{
public:
    explicit ScidBaseModel(QWidget *parent = nullptr);
    void setInfo(QString dir, uint number);
protected:
    int columnCount(const QModelIndex &parent ) const override;
    int rowCount( const QModelIndex &parent)const override;
    QVariant data ( const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override ;

private:
    QString mDir;
    int mNumber;
    QWidget *mWid;

};

#endif // SCIDBASEMODEL_H
