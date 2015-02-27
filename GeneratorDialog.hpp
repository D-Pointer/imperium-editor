#ifndef GENERATORDIALOG_HPP
#define GENERATORDIALOG_HPP

#include <QDialog>
#include <QImage>
#include <QList>
#include <QPolygonF>

class Map;

namespace Ui {
class GeneratorDialog;
}


class GeneratorDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit GeneratorDialog(QWidget *parent = 0);
    ~GeneratorDialog();
    
    /**
     * Returns the created map or null if cancelled.
     *
     * @return the created map.
     */
    Map * getMap () {
        return m_map;
    }


private slots:

    void ok ();


private:

    void generatePerlin (QImage & image);

    void findForests (QImage & image);

    void cleanup (QImage & image);

    QList<QLine> trace (QImage & image);

    void buildPolygons (QList<QLine> & lines, QImage & image);

    void simplifyPolygon (QPolygon & polygon);

    void generateRivers (QImage &image);
    void generateOneRiver (QImage &image);

    void showImage (const QImage & image, const QString & title);

    void createMap ();

    Ui::GeneratorDialog *ui;

    // the map we create
    Map * m_map;

    int m_width;
    int m_height;

    static const int s_margin;
    static const int s_tileSize;

    // found forests
    QList<QPolygonF> m_forests;
};

#endif // GENERATORDIALOG_HPP
