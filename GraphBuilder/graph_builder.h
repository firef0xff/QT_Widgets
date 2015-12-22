#pragma once
#include <QPixmap>
#include <QFont>

namespace ff0x
{

class GraphBuilder
{
public:
    enum Mode
    {
        PlusPlus,
        TopHalf,
        BottomHalf,
        Full,
    };
    struct LabelInfo
    {
        LabelInfo():
            mName( "" ),
            mColor( Qt::black )
        {}
        LabelInfo( QString name, Qt::GlobalColor color ):
            mName( name ),
            mColor( color )
        {}

        QString mName;
        Qt::GlobalColor mColor;
    };
    typedef QVector< QPointF > LinePoints;
    typedef std::pair< LinePoints, LabelInfo > Line;
    typedef QVector< Line > GraphDataLine;
    GraphBuilder( int width, int height, Mode mode, QFont font = QFont() );


    QPixmap Draw(GraphDataLine const& data,
                  qreal x_interval,
                  qreal y_interval,
                  qreal x_step,
                  qreal y_step,
                  QString x_label,
                  QString y_label,
                  bool draw_greed = false ) const;
private:
    Mode mMode;
    int mWidth;
    int mHeight;
    QFont mFont;
};

}//namespace ff0x
