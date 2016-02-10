#include "vtn_filtro.h"
#include "ui_vtn_filtro.h"
#include <iostream>

vtn_filtro::vtn_filtro(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vtn_filtro)
{
    ui->setupUi(this);
	allok = false;
	reset_window();
}

vtn_filtro::~vtn_filtro()
{
    delete ui;
}

void vtn_filtro::reset_window()
{
	reset_directions();

	hide_gaussiano();
	hide_laplaciano();
	hide_sobel_prewitt();
	hide_media();
}

void vtn_filtro::reset_directions()
{
	ui->d0->setChecked(false);
	ui->d1->setChecked(false);
	ui->d2->setChecked(false);
	ui->d3->setChecked(false);
	ui->d4->setChecked(false);
	ui->d5->setChecked(false);
	ui->d6->setChecked(false);
	ui->d7->setChecked(false);
}

void vtn_filtro::hide_gaussiano()
{
	ui->gb_gaussiano->hide();
}

void vtn_filtro::show_gaussiano()
{
	// Mostrar solo lo necesario para Gaussiano
	ui->gb_gaussiano->show();
	ui->le_alto->setText("3");
	ui->le_ancho->setText("3");
	ui->le_sigma->setText("4.0");

	// Ocultar todo lo demas
	hide_laplaciano();
	hide_sobel_prewitt();
	hide_media();

}

void vtn_filtro::show_media()
{
	ui->gb_media->show();
	indice_media = Filtro::Operador::Media_Nueve;
	ui->rb_9->setDown(true);
	hide_gaussiano();
	hide_laplaciano();
	hide_sobel_prewitt();
}

void vtn_filtro::hide_media()
{
	ui->gb_media->hide();
}

void vtn_filtro::show_laplaciano()
{
	ui->chb_centro_positivo->setChecked(true);
	centro_positivo = true;
	ui->gb_laplaciano->show();

	// Ocultar todo lo demas
	hide_gaussiano();
	hide_media();
	hide_sobel_prewitt();

}

void vtn_filtro::hide_laplaciano()
{
	ui->gb_laplaciano->hide();
}

void vtn_filtro::show_sobel_prewitt()
{
	ui->rb_magnitud->setChecked(true);
	mostrar_magnitud = true;
	reset_directions();
	ui->gb_direcciones->show();
	hide_gaussiano();
	hide_media();
	hide_laplaciano();
}

void vtn_filtro::hide_sobel_prewitt()
{
	ui->gb_direcciones->hide();
}


void vtn_filtro::on_btn_cancelar_clicked()
{
	close();
}

void vtn_filtro::on_cb_lista_filtros_currentIndexChanged( int index )
{
	indice_lista = index;

	if ( !indice_lista )
	{
		hide_gaussiano();
		hide_laplaciano();
		hide_media();
		hide_sobel_prewitt();
		return;
	}

	switch ( indice_lista )
	{
		case Filtro::Operador::Gaussiano:
			show_gaussiano();
			break;
		case Filtro::Operador::Laplaciano:
			metodo_laplaciano = 0;
			show_laplaciano();
			ui->chb_centro_positivo->hide();
			ui->cb_laplaciano->hide();
			break;
		case Filtro::Operador::Sobel:
			indice_operador = 0; // El siguiente comentario es un mandato
			show_sobel_prewitt();
			break;
		case Filtro::Operador::Prewitt:
			indice_operador = 1; // Acomodar Enum && Namespaces
			show_sobel_prewitt();
			break;
		case Filtro::Operador::Media_Diez:
			show_media();
	}
}

bool vtn_filtro::is_ok()
{
	return allok;
}

void vtn_filtro::on_btn_aplicar_clicked()
{
	allok = true;
	switch ( indice_lista )
	{
		case Filtro::Operador::Gaussiano:
			sigma = ui->le_sigma->text().toDouble();
			ancho = ui->le_ancho->text().toInt();
			alto = ui->le_alto->text().toInt();
			break;

		case Filtro::Operador::Laplaciano:
			indice_operador = Filtro::Operador::Laplaciano;
			if ( nvecinos < 0 ) // No se ha seleccionado ninguna cantidad de vecinos
				allok = false;
			break;

		case Filtro::Operador::Sobel:
			indice_operador = Filtro::Operador::Sobel;
			n_dir = 0;
			generar_direcciones();
			if ( !n_dir ) // Al menor una direccion de mascara
				allok = false;
			break;

		case Filtro::Operador::Prewitt:
			indice_operador = Filtro::Operador::Prewitt;
			n_dir = 0;
			generar_direcciones();
			if ( !n_dir ) // Al menor una direccion de mascara
				allok = false;
			break;

		case Filtro::Operador::Media_Diez :
			if ( !indice_media )
				allok = false;
	}

	if ( allok )
		close();
}

void vtn_filtro::on_cb_laplaciano_currentIndexChanged( int index )
{
	metodo_laplaciano = index;
}

void vtn_filtro::on_cb_vecindad_currentIndexChanged( int index )
{
	if ( !index )
	{
		nvecinos = -1;
		ui->chb_centro_positivo->hide();
		ui->cb_laplaciano->hide();
	}
	else
	{
		indice_vecindad = index;
		ui->cb_laplaciano->show();
		ui->chb_centro_positivo->show();
		nvecinos = (indice_vecindad == 1? 0 : 2 ) + ( centro_positivo? 0 : 1) ;
	}
}

void vtn_filtro::on_chb_centro_positivo_clicked()
{
	centro_positivo = ui->chb_centro_positivo->isChecked();
	nvecinos = (indice_vecindad == 1? 0 : 2 ) + ( centro_positivo? 0 : 1) ;
}

void vtn_filtro::generar_direcciones()
{
	if ( ui->d0->isChecked() )
		direcciones[n_dir++] = 0;

	if ( ui->d1->isChecked() )
		direcciones[n_dir++] = 1;

	if ( ui->d2->isChecked() )
		direcciones[n_dir++] = 2;

	if ( ui->d3->isChecked() )
		direcciones[n_dir++] = 3;

	if ( ui->d4->isChecked() )
		direcciones[n_dir++] = 4;

	if ( ui->d5->isChecked() )
		direcciones[n_dir++] = 5;

	if ( ui->d6->isChecked() )
		direcciones[n_dir++] = 6;

	if ( ui->d7->isChecked() )
		direcciones[n_dir++] = 7;
}

void vtn_filtro::on_rb_10_clicked()
{
	indice_media = Filtro::Operador::Media_Diez;
}

void vtn_filtro::on_rb_16_clicked()
{
	indice_media = Filtro::Operador::Media_Dieciseis;
}

void vtn_filtro::on_rb_9_clicked()
{
	indice_media = Filtro::Operador::Media_Nueve;
}

void vtn_filtro::on_rb_direccion_clicked()
{
	mostrar_magnitud = false;
}

void vtn_filtro::on_rb_magnitud_clicked()
{
	mostrar_magnitud = true;
}
