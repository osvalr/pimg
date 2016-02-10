#ifndef PIMG_H
# define PIMG_H

# include <QMainWindow>
# include <QFileDialog>
# include <QGraphicsScene>
# include <QPixmap>
# include <QVector>
# include <QRgb>
# include <QLabel>

# include "qcustomplot.h"
# include "classImagen.h"
# include "classFiltro.h"

# include <iostream>

namespace Ui {
    class ventana;
}

namespace grafico {
	enum {
		histograma = 0,
		puntero_slider
	};
}


static const unsigned SA_IMAGEN_DIM_H = 351;
static const unsigned SA_IMAGEN_DIM_V = 431;

class ventana : public QMainWindow {
    Q_OBJECT

    public:
        explicit ventana(QWidget *parent = 0);
        ~ventana();

    private:
		Ui::ventana *ui;        // Ventana
		imagen * bimg;          // Imagen original - backup image
		imagen * img;           // Imagen que se modificará
		filtro * f;             // Filtro

		QVector<QRgb> * paleta; // Paleta de colores necesaria para QImagen
		QImage * qimagen;       // Representacion de la imagen en la interfaz
		QLabel * lb_img_area;
		int slider_value;       // Posicion en X del QSlider
		size_t nivel_slider;    // Posicion en Y del QSlider
		bool hay_imagen;        // <eliminar, no recuerdo su uso>

		void mostrar_histograma();
		void restaurar_umbralizacion();
		void ajustar_scrollbars( bool , bool );

    private slots:
        // Menu Inicio
        void on_mn_imagen_abrir_triggered();
        void on_mn_imagen_cerrar_triggered();
		void on_mn_guardarcomo_triggered();
        void on_mn_salir_triggered();

		// Menu Imagen
		void on_mn_ver_propiedades_triggered();
		void on_mn_restaurar_imagen_triggered();
        void on_mn_obtener_histograma_triggered();
        void on_mn_negativo_triggered();
        void on_mn_umbral_triggered();
        void on_mn_amp_contraste_triggered();
		void on_mn_disminucion_triggered();
		void on_mn_aumento_triggered();
		void on_mn_eq_histograma_triggered();

        // Herramientas
		bool regraficar_imagen();
		void on_btn_aplicar_umbral_clicked();
        void on_qs_slider_sliderReleased();
        void on_qs_slider_valueChanged( int );
		void on_qs_slider_sliderMoved( int );
		void on_mn_filtro_triggered();

		// Menu Filtro
//		void on_mn_filtro_media_triggered();
		//void on_mn_filtro_gaussiano_triggered();
	//	void on_mn_gradiente_triggered();
	//	void on_mn_laplaciano_triggered();
	//	void on_mn_laplaciano_reescalado_triggered();
	//	void on_mn_realcebordes_triggered();

};

#endif // PIMG_H
