#include "ventana.h"
#include "ui_ventana.h"
#include "vtn_propiedades.h"
#include "vtn_estirar.h"
#include "vtn_filtro.h"
#include "classFiltro.h"

#include <QDesktopWidget>
#include <QPen>
#include <QVector>
#include <QString>
#include <climits>
#include <cmath>

// Laplacian Helper Enum
enum {
	SOLO_CONVOLUCION = 0,
	LAPLACIANO_REESCALADO,
	REALCE_IMAGEN
};

ventana::ventana(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ventana)
{
    ui->setupUi(this);
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
	bimg = new imagen();
	lb_img_area = new QLabel("(No hay imagen abierta)", this);

	ui->sa_imagen->setWidget(lb_img_area);
	ui->mn_imagen_cerrar->setEnabled(false);
	ui->mn_guardarcomo->setEnabled(false);
	ui->mn_guardar->setEnabled(false); // No se hace uso todavía
    ui->menuImagen->setEnabled( false );
	ajustar_scrollbars(false, false);
    hay_imagen = false;
    slider_value = 0;
	paleta = NULL;
    ui->gb_umbralizacion->hide();
    ui->gb_histograma->hide();
    ui->hist->xAxis->setRange(-1, 256);
    ui->qs_slider->setTickInterval(1);
}

ventana::~ventana()
{
	if ( paleta )
		delete paleta;

	if ( qimagen )
		delete qimagen;

	delete lb_img_area;
	delete img;
	delete bimg;
    delete ui;
}

void ventana::ajustar_scrollbars( bool ver_horizontal, bool ver_vertical )
{
	if ( ver_horizontal )
		ui->sa_imagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	else
		ui->sa_imagen->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	if ( ver_vertical )
		ui->sa_imagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	else
		ui->sa_imagen->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ventana::on_mn_imagen_abrir_triggered()
{
    size_t i, j;
    QString filtro = tr("PAM (*.pbm *.pgm *.ppm)");
    QString nombre_archivo = QFileDialog::getOpenFileName(
                this,
                "Abrir una imagen",
				"samples/",
                tr("PGM ( *.pgm )"));

    if  ( nombre_archivo.isEmpty() )
    {
        ui->statusbar->showMessage(QString::fromUtf8("No se seleccionó ningun archivo"));
        return;
    }

    ui->statusbar->showMessage(QString::fromUtf8("Abriendo imagen..."));
	bimg->cargar_imagen(nombre_archivo.toUtf8().constData());

	img = new imagen( bimg->get_height(), bimg->get_width(), bimg->get_tipo_imagen(), bimg->get_magic_number(), bimg->get_max_val());
	for ( i = 0 ; i < img->get_height(); ++i )
		for ( j = 0 ; j < img->get_width(); ++j )
			img->set_pixel( j, i, bimg->get_pixel(j, i) );

	if ( !paleta )
	{
		paleta = new QVector<QRgb>(256);
		for ( i = 0; i < 256; ++i )
		   (*paleta)[i] = qRgb( i, i, i );
	}

	qimagen = new QImage(img->get_width(), img->get_height(), QImage::Format_Indexed8);
    qimagen->setColorTable( *paleta );

	for ( i = 0; i < img->get_height(); ++i )
	   for ( j = 0; j < img->get_width(); ++j )
		   qimagen->setPixel( j, i, img->get_pixel(j, i));

	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));
	lb_img_area->adjustSize();
	ajustar_scrollbars( img->get_width() > SA_IMAGEN_DIM_H , img->get_height() >SA_IMAGEN_DIM_V );
    ui->mn_imagen_abrir->setEnabled(false);
	ui->mn_imagen_cerrar->setEnabled(true);
	ui->mn_guardarcomo->setEnabled(true);

    ui->menuImagen->setEnabled( true );

    ui->btn_aplicar_umbral->setEnabled( false );

    hay_imagen = true;
    ui->statusbar->showMessage(nombre_archivo);
}

void ventana::on_mn_imagen_cerrar_triggered()
{
    if ( !hay_imagen )
        return;

	ajustar_scrollbars(false, false);
	bimg->eliminar_imagen();
	delete img;

    ui->mn_imagen_abrir->setEnabled(true);
    ui->mn_imagen_cerrar->setEnabled(false);
	ui->mn_guardarcomo->setEnabled(false);

	ui->menuImagen->setEnabled(false);

    restaurar_umbralizacion();

    ui->gb_histograma->hide();
    ui->gb_umbralizacion->hide();
    restaurar_umbralizacion();
	lb_img_area->setBaseSize(SA_IMAGEN_DIM_H, SA_IMAGEN_DIM_V);
	lb_img_area->setText("(No hay imagen abierta)");
    ui->statusbar->showMessage(QString::fromUtf8("Imagen cerrada..."));

	delete qimagen;
}

void ventana::on_mn_guardarcomo_triggered()
{
	int i, index;
	QString newfile;
	QStringList path;
	QString file = QFileDialog::getSaveFileName(this,
												tr("Guardar Imagen como.. "),
												"..pimg/samples/",
												tr("PAM (*.pgm)") );

	if ( file.isEmpty() )
	{
		ui->statusbar->showMessage(QString::fromUtf8("La imagen no fué almanenada"));
		return;
	}
	std::cout << file.toUtf8().constData() << std::endl;

	path = file.split("/");
	newfile = path.last();
	path.removeLast();

	if ( (index = newfile.indexOf(".")) > 0 )
	{
		for ( i = 0 ; i < index; ++i )
			file.append(newfile.at(i));
		newfile = file;
	}
	newfile.append( QString::fromStdString(img->get_extension()) );
	path.append(newfile);
	file = path.join("/");

	img->guardar_imagen( file.toUtf8().constData() );
	ui->statusbar->showMessage(QString::fromUtf8("Almacenada la imagen actual"));

}

void ventana::on_mn_salir_triggered()
{
    exit(EXIT_SUCCESS);
}

void ventana::on_mn_obtener_histograma_triggered()
{
    ui->statusbar->showMessage(QString::fromUtf8("Mostrando Histograma.."));
    mostrar_histograma();
    restaurar_umbralizacion();
    ui->gb_umbralizacion->hide();
}

void ventana::mostrar_histograma()
{
    size_t i, j;
    QVector<double> x(256), y(256);
    double maximo = .0;

    for ( i = 0; i < 256; ++i )
    {
        y[i] = 0;
        x[i] = i;
    }
    ui->hist->clearGraphs();

    ui->hist->addGraph();	
	ui->hist->addGraph();
	ui->hist->graph(grafico::histograma)->setPen(QPen(Qt::blue));
	ui->hist->graph(grafico::histograma)->setBrush(QBrush(Qt::Dense3Pattern));

    for ( i = 0; i < img->get_height(); ++i )
        for ( j = 0; j < img->get_width(); ++j )
            y[qimagen->pixelIndex(j,i)]++;

	for ( i = 0; i < 256; ++i )
        maximo = y[i] > maximo? y[i] : maximo;

	nivel_slider = maximo;
	ui->hist->yAxis->setRange(0 , maximo + 10 );
	ui->hist->graph(grafico::histograma)->setData(x, y);
    ui->hist->replot();
    ui->gb_histograma->show();
}

void ventana::on_qs_slider_sliderReleased()
{
    ui->btn_aplicar_umbral->setEnabled( true );
}

void ventana::on_mn_ver_propiedades_triggered()
{
    vtn_propiedades *p = new vtn_propiedades(this);
    ui->statusbar->showMessage(QString::fromUtf8("Propiedades..."));
    p->set_propiedades(img->get_height(),img->get_width(),img->get_number_elements(),img->get_max_val());
    p->setModal(true);
    p->exec();
    delete p;
}

void ventana::on_qs_slider_valueChanged( int value )
{

    slider_value =  value;
    ui->lb_valor_actual->setText(QString::number(slider_value));
}

void ventana::on_mn_umbral_triggered()
{
	mostrar_histograma();

	ui->gb_umbralizacion->show();
	ui->gb_histograma->show();
	ui->qs_slider->setEnabled(true);

	ui->hist->graph(grafico::puntero_slider)->setPen(QPen(Qt::red));
	ui->hist->graph(grafico::puntero_slider)->setBrush(QColor(Qt::red));
	ui->statusbar->showMessage(QString::fromUtf8("Seleccione un nivel de gris para aplicar la umbralizacion..."));
}

void ventana::on_mn_negativo_triggered()
{
	size_t i, j;

	imagen * tmp = img->negativo();
	delete img;
	img = tmp;

	for ( i = 0; i < img->get_height(); ++i )
		for ( j = 0; j < img->get_width(); ++j )
			qimagen->setPixel( j, i, img->get_pixel(j, i) );

	ui->statusbar->showMessage(QString::fromUtf8("Aplicado Negativo..."));
	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));
}

void ventana::on_mn_amp_contraste_triggered()
{
	size_t i, j;

	imagen * tmp = img->aumento_constraste();
	delete img;
	img = tmp;

	for ( i = 0; i < img->get_height(); ++i )
		for ( j = 0; j < img->get_width(); ++j )
			qimagen->setPixel( j, i, img->get_pixel(j, i) );

	ui->statusbar->showMessage(QString::fromUtf8("Aplicado Estiramiento del Contraste..."));
	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));
}

void ventana::on_mn_restaurar_imagen_triggered()
{
	size_t i, j;

	delete img;

	img = new imagen( bimg->get_height(), bimg->get_width(), bimg->get_tipo_imagen(), bimg->get_magic_number(), bimg->get_max_val());
	for ( i = 0 ; i < img->get_height(); ++i )
		for ( j = 0 ; j < img->get_width(); ++j )
			img->set_pixel( j, i, bimg->get_pixel(j, i) );
	regraficar_imagen();

	ui->statusbar->showMessage(QString::fromUtf8("Imagen restaurada al original"));

	ui->gb_histograma->hide();
	restaurar_umbralizacion();
	ui->gb_umbralizacion->hide();
}

void ventana::on_mn_eq_histograma_triggered()
{
	size_t  i, j;

	imagen * tmp = img->ecualizacion();
	delete img;
	img = tmp;

	for ( i = 0; i < img->get_height(); ++i )
		for ( j = 0; j < img->get_width(); ++j )
			qimagen->setPixel( j, i, img->get_pixel(j, i) );

	ui->statusbar->showMessage(QString::fromUtf8("Aplicada la Ecualización del Histograma..."));
	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));
}

void ventana::on_mn_disminucion_triggered()
{
	size_t ancho, alto;
	imagen * tmp = NULL;
	vtn_estirar *v = new vtn_estirar(this);
	v->set_tipo(ZOOM_ALEJAR);
	v->set_dimensiones_maximas( img->get_width(), img->get_height());
	v->setModal(true);
	v->exec();

	if ( v->is_ok() )
	{
		alto = v->get_alto();
		ancho = v->get_ancho();
		tmp = img->reducir( ancho, alto );
		delete img;
		img = tmp;
		regraficar_imagen();
	}

	delete v;
}

void ventana::on_mn_aumento_triggered()
{
	size_t ancho, alto;
	imagen * tmp = NULL;
	vtn_estirar *v = new vtn_estirar(this);
	v->set_tipo(ZOOM_ACERCAR);
	v->setModal(true);
	v->exec();

	if ( v->is_ok() )
	{
		alto = v->get_alto();
		ancho = v->get_ancho();

		tmp = img->ampliar( v->get_metodo(), ancho, alto );
		delete img;
		img = tmp;
		regraficar_imagen();
	}

	delete v;
}

void ventana::on_btn_aplicar_umbral_clicked()
{
	size_t i, j;

	imagen * tmp = img->umbralizar(slider_value);
	delete img;
	img = tmp;

	for ( i = 0; i < img->get_height(); ++i )
		for ( j = 0; j < img->get_width(); ++j )
			qimagen->setPixel( j, i, img->get_pixel(j, i) );

	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));

    mostrar_histograma();
    ui->btn_aplicar_umbral->setEnabled( false );
    ui->statusbar->showMessage(QString::fromUtf8("Aplicada la Umbralizacion con un nivel de gris de ") + QString::number(slider_value) );
}

void ventana::restaurar_umbralizacion()
{
    ui->qs_slider->setValue(0);
}

bool ventana::regraficar_imagen()
{
	size_t i, j;
	if ( !qimagen || !img )
		return false;

	delete qimagen;
	qimagen = new QImage(img->get_width(), img->get_height(), QImage::Format_Indexed8);
	qimagen->setColorTable( *paleta );

	for ( i = 0; i < img->get_height(); ++i )
	   for ( j = 0; j < img->get_width(); ++j )
		   qimagen->setPixel( j, i, img->get_pixel(j, i));

	ajustar_scrollbars(img->get_width() > SA_IMAGEN_DIM_H , img->get_height() > SA_IMAGEN_DIM_V );
	lb_img_area->setPixmap(QPixmap::fromImage(*qimagen));
	lb_img_area->adjustSize();
	ui->sa_imagen->setWidget(lb_img_area);
	return true;
}

void ventana::on_qs_slider_sliderMoved( int position )
{
	QVector<double> x(2), y(2);
	x[0] = position;
	x[1] = x[0] + 1;
	y[0] = y[1] = nivel_slider;

	ui->hist->graph(grafico::puntero_slider)->setData(x,y);

	ui->hist->replot();
}

void ventana::on_mn_filtro_triggered()
{
	size_t i, j;

	filtro * f;
	int **aux, **aux2 = NULL;
	vtn_filtro *v = new vtn_filtro(this);
	v->setModal(true);
	v->exec();

	if ( v->is_ok() )
	{
		f = new filtro();
		f->set_imagen(img);
		switch ( v->indice_lista )
		{
			case Filtro::Operador::Gaussiano :
				f->filtro_gaussiano( v->ancho, v->alto, v->sigma );
				break;

			case Filtro::Operador::Laplaciano :
				aux = img->laplaciano( v->nvecinos );
				switch( v->metodo_laplaciano )
				{
					case SOLO_CONVOLUCION:
						for ( i = 0 ; i < img->get_height(); ++i )
							for ( j = 0 ; j < img->get_width(); ++j )
								img->set_pixel( j, i, fabs(aux[i][j]) > 255 ? 255 : fabs(aux[i][j]));
						break;

					case LAPLACIANO_REESCALADO:
						aux2 = imagen::reescalar( aux, img->get_height(), img->get_width() );


						for ( i = 0 ; i < img->get_height(); ++i )
							for ( j = 0 ; j < img->get_width(); ++j )
								img->set_pixel( j, i, aux2[i][j]);
						filtro::eliminar(img->get_width(),img->get_height(), (double **) aux2);
						break;

					case REALCE_IMAGEN:
						aux2 = new int * [img->get_height()];
						for ( i = 0; i < img->get_height(); ++i )
						{
							aux2[i] = new int [img->get_width()];
							for ( j = 0 ; j < img->get_width(); ++j )
								aux2[i][j] = aux[i][j];
						}

						for ( i = 0; i < img->get_height(); ++i )
							for ( j = 0 ; j < img->get_width(); ++j )
								aux2[i][j] = img->get_pixel(j,i) - aux[i][j];

						filtro::eliminar(img->get_width(),img->get_height(), (double **) aux);

						aux = imagen::reescalar( aux2, img->get_height(), img->get_width() );

						filtro::eliminar(img->get_width(),img->get_height(), (double **) aux2);

						for ( i = 0 ; i < img->get_height(); ++i )
							for ( j = 0 ; j < img->get_width(); ++j )
								img->set_pixel( j, i, aux[i][j]);
				}
				filtro::eliminar(img->get_width(),img->get_height(), (double **) aux);
				break;

			case Filtro::Operador::Sobel :
			case Filtro::Operador::Prewitt :
				img->gradiente( v->indice_operador, v->direcciones, v->n_dir );
				for ( i = 0 ; i < img->get_height(); ++i )
					for ( j = 0 ; j < img->get_width(); ++j )
						img->set_pixel( j, i, v->mostrar_magnitud? img->M[i][j] : img->D[i][j] );

				filtro::eliminar(img->get_width(),img->get_height(),img->D);
				filtro::eliminar(img->get_width(),img->get_height(),img->M);
				break;

			default : // Filtro de Media
				f->filtro_media(v->indice_media);
		}
		delete f;

		regraficar_imagen();
	}
	delete v;
}
