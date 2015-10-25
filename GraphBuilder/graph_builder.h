#pragma once
#include <QPixmap>
#include <QFont>

namespace ff0x
{

class GraphBuilder
{
public:
    typedef QVector< QPointF > LinePoints;
    typedef std::pair< LinePoints, Qt::GlobalColor > Line;
    typedef QVector< Line > GraphDataLine;
    GraphBuilder( int width, int height, QFont font = QFont() );


    QPixmap Draw(GraphDataLine const& data,
                  qreal x_interval,
                  qreal y_interval,
                  qreal x_step,
                  qreal y_step,
                  QString x_label,
                  QString y_label,
                  bool draw_greed = false ) const;
private:
    int mWidth;
    int mHeight;
    QFont mFont;
};

}//namespace ff0x
