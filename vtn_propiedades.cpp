#include "vtn_propiedades.h"
#include "ui_vtn_propiedades.h"

#include <QString>
#include <iostream>
vtn_propiedades::vtn_propiedades(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vtn_propiedades)
{
    ui->setupUi(this);
}

void vtn_propiedades::set_propiedades(unsigned altura, unsigned ancho, unsigned tamano, unsigned colormax)
{
    ui->lbv_ancho->setText(QString::number(ancho));
    ui->lbv_altura->setText(QString::number(altura));
    ui->lbv_tamano->setText(QString::number(tamano));
    ui->lbv_valormax->setText(QString::number(colormax));
}

vtn_propiedades::~vtn_propiedades()
{
    delete ui;
}

void vtn_propiedades::on_btn_aceptar_clicked()
{
    close();
}
