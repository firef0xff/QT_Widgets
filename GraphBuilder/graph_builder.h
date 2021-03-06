#pragma once
#include <QPixmap>
#include <QFont>

namespace ff0x
{

class BasicGraphBuilder
{
public:
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

    BasicGraphBuilder( int width, int height, QFont font );

protected:
    int mWidth;
    int mHeight;
    QFont mFont;
};
class GraphBuilder : public BasicGraphBuilder
{
public:
    enum Mode
    {
        PlusPlus,
        TopHalf,
        BottomHalf,
        Full,
    };
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
};

class NoAxisGraphBuilder :public BasicGraphBuilder
{
public:
    NoAxisGraphBuilder( int width, int height, QFont font = QFont() );

    void DrawBorderCeil( bool );

    QPixmap Draw(GraphDataLine const& data,
                  QPointF x_range,
                  QPointF y_range,
                  qreal x_step,
                  qreal y_step,
                  QString x_label,
                  QString y_label,
                  bool draw_greed = false ) const;
protected:
    virtual qreal TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const;
    virtual qreal TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const;
    bool mDrawBorderCeil = true;
};

class Log10GraphBuilder : public NoAxisGraphBuilder
{
public:
    Log10GraphBuilder( int width, int height, QFont font = QFont() );

    QPixmap Draw(GraphDataLine const& data,
                  QPointF x_range,
                  QPointF y_range,
                  qreal y_step,
                  QString x_label,
                  QString y_label,
                  bool draw_greed = false ) const;

protected:
    virtual qreal TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const;
    virtual qreal TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const;

};

void DataLength( QPointF const& range, QPointF &out_range, double &out_step );

QPointF MergeRanges( QPointF const& range1, QPointF const& range2, bool use_r2 = true );

}//namespace ff0x
