#ifndef CCHECKBOXHEADERVIEW_H
#define CCHECKBOXHEADERVIEW_H

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>

class CCheckBoxHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    CCheckBoxHeaderView(int checkColumnIndex,
                        Qt::Orientation orientation,
                        QWidget *parent=0):
    QHeaderView(orientation,parent)
    {
        m_checkColIdx= checkColumnIndex;
        m_isChecked = false;
    }

signals:
    void checkStatusChange(bool);

protected:
    void paintSection(QPainter *painter,const QRect &rect,int logicalIndex) const
    {
        painter->save();
        QHeaderView::paintSection(painter,rect,logicalIndex);
        painter->restore();
        if(logicalIndex == m_checkColIdx)
        {
            QStyleOptionButton option;
            int width = 10;
            for(int i=0; i < logicalIndex; ++i)
                width += sectionSize(i);
            option.rect = QRect(3,5,21,22);
            if(m_isChecked)
                option.state = QStyle::State_On;
            else
                option.state = QStyle::State_Off;
            this->style()->drawControl(QStyle::CE_CheckBox, &option, painter);
        }

    }

    void mousePressEvent(QMouseEvent *event)
    {
        if(visualIndexAt(event->pos().x()) == m_checkColIdx)
        {
            if(m_isChecked)
                m_isChecked = false;
            else
                m_isChecked = true;
            this->updateSection(m_checkColIdx);
            emit checkStatusChange(m_isChecked);
        }
        QHeaderView::mousePressEvent(event);
    }

private:
    bool m_isChecked;
    int  m_checkColIdx;

};

#endif // CCHECKBOXHEADERVIEW_H
