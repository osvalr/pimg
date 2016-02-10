#ifndef VTN_PROPIEDADES_H
#define VTN_PROPIEDADES_H

#include <QDialog>

namespace Ui {
    class vtn_propiedades;
}

class vtn_propiedades : public QDialog
{
    Q_OBJECT

public:
    explicit vtn_propiedades(QWidget *parent = 0);
    void set_propiedades(unsigned altura, unsigned ancho, unsigned tamano, unsigned colormax);
    ~vtn_propiedades();

private slots:
    void on_btn_aceptar_clicked();

private:
    Ui::vtn_propiedades *ui;
};

#endif // VTN_PROPIEDADES_H
