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
    void traceForests (QImage & image);
    void traceOneForest (QImage & image, int x, int y, int forestColor, int backgroundColor);

    void fill (QImage & image, int x, int y, int newColor, int oldColor);

    void simplifyForest (QPolygonF & forest);

    void showImage (const QImage & image);

    void createMap ();

    inline int index (int x, int y) const {
        return m_width * y + x;
    }

    inline void fromIndex (int index, int & x, int & y) const {
        y = index / m_width;
        x = index - m_width * y;
    }

    Ui::GeneratorDialog *ui;

    // the map we create
    Map * m_map;

    int m_width;
    int m_height;

    // found forests
    QList<QPolygonF> m_forests;
};

#endif // GENERATORDIALOG_HPP
