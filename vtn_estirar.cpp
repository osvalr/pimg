#include "vtn_estirar.h"
#include "ui_vtn_estirar.h"

#include <QString>
#include <string>
#include <iostream>

#include <QMessageBox>

vtn_estirar::vtn_estirar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vtn_estirar)
{
    ui->setupUi(this);
    zoom_mantener_aspecto = true;

    validator = new QIntValidator(MIN_ESTIRAMIENTO, MAX_ESTIRAMIENTO, this);
    ui->le_hor->setValidator(validator);
    ui->le_ver->setValidator(validator);

    ultimo_le = NO_LE;
	metodo = 0;
	allok = false;
}

vtn_estirar::~vtn_estirar()
{
    delete validator;
    delete ui;
}

void vtn_estirar::on_btn_rel_aspecto_clicked()
{
    if( zoom_mantener_aspecto )
    {
        ui->btn_rel_aspecto->setIcon(QPixmap("images/nora.png"));
    }
    else
    {
        ui->btn_rel_aspecto->setIcon(QPixmap("images/sira.png"));
        switch ( ultimo_le )
        {
            case HORIZONTAL_LE:
                ui->le_ver->setText(ui->le_hor->text());
                break;
            case VERTICAL_LE:
                ui->le_hor->setText(ui->le_ver->text());
                break;
            case NO_LE:
                ui->le_hor->setText("0");
                ui->le_ver->setText("0");
        }
    }
    zoom_mantener_aspecto = !zoom_mantener_aspecto;
}

void vtn_estirar::set_tipo( tipo_estiramiento t )
{
	QString titulo;
	tipoe = t;
	switch ( t )
	{
		case ZOOM_ACERCAR:
			titulo = "Aumentar";
			break;
		case ZOOM_ALEJAR:
			titulo = "Disminuir";
			ui->cb_metodo->hide();
			ui->label->hide();
			break;
		default:
			std::cerr << __FILE__ << "@" << __LINE__ << ": argumento inválido, lo permitido es {ZOOM_ACERCAR|ZOOM_ALEJAR}" << std::endl;
	}

	ui->btn_aplicar->setText( titulo );
}

void vtn_estirar::set_dimensiones_maximas( size_t max_w, size_t max_h )
{
	if ( tipoe == ZOOM_ACERCAR )
	{
		std::cerr << __FILE__ << "@" << __LINE__ << ": Este metodo no aplica para ZOOM_ACERCAR" << std::endl;
		return;
	}
	max_ancho = max_w;
	max_alto = max_h;

}

size_t vtn_estirar::get_alto()
{
    return (size_t) ui->le_ver->text().toUInt();
}

size_t vtn_estirar::get_ancho()
{
    return (size_t) ui->le_hor->text().toUInt();
}

int  vtn_estirar::get_metodo()
{
	return metodo;
}

bool vtn_estirar::is_ok()
{
	return allok;
}

void vtn_estirar::on_le_hor_textChanged(const QString &arg1)
{
    ultimo_le = HORIZONTAL_LE;
    if ( zoom_mantener_aspecto )
        ui->le_ver->setText(arg1);
}

void vtn_estirar::on_le_ver_textChanged(const QString &arg1)
{
    ultimo_le = VERTICAL_LE;
    if ( zoom_mantener_aspecto )
        ui->le_hor->setText(arg1);
}

void vtn_estirar::on_btn_aplicar_clicked()
{
	switch ( tipoe )
	{
		case ZOOM_ACERCAR:
			if ( metodo && get_ancho() && get_alto() )
				allok = true;
			break;

		case ZOOM_ALEJAR:
			if ( max_ancho / get_ancho() > 0
				&& max_alto / get_alto() > 0 )
				allok = true;
	}

	if ( allok )
		close();
}
void vtn_estirar::on_btn_cancelar_clicked()
{
	close();
}

void vtn_estirar::on_cb_metodo_currentIndexChanged( int index )
{
	metodo = index;
}
