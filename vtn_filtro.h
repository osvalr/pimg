#ifndef VTN_FILTRO_H
#define VTN_FILTRO_H

#include <QDialog>
#include "classFiltro.h"
namespace Ui {
    class vtn_filtro;
}

class vtn_filtro : public QDialog
{
    Q_OBJECT

	private:
		Ui::vtn_filtro *ui;

		void reset_window();
		void reset_directions();

		void hide_gaussiano();
		void hide_laplaciano();
		void hide_sobel_prewitt();

		void generar_direcciones();

		void show_gaussiano();
		void show_media();
		void hide_media();
		void show_laplaciano();
		void show_sobel_prewitt();
		bool allok;

		int indice_vecindad;

public:
		double sigma;
		size_t ancho, alto;
		unsigned direcciones[8];
		size_t n_dir;
		unsigned indice_operador;
		bool centro_positivo;
		int nvecinos;
		int metodo_laplaciano;
		unsigned indice_lista;
		unsigned indice_media;
		bool mostrar_magnitud;
		bool is_ok();
		explicit vtn_filtro(QWidget *parent = 0);
		~vtn_filtro();

	private slots:

		void on_cb_vecindad_currentIndexChanged(int index);
		void on_cb_laplaciano_currentIndexChanged( int index );
		void on_btn_aplicar_clicked();
		void on_cb_lista_filtros_currentIndexChanged(int index);
		void on_btn_cancelar_clicked();

		void on_chb_centro_positivo_clicked();
		void on_rb_10_clicked();
		void on_rb_16_clicked();
		void on_rb_9_clicked();
		void on_rb_direccion_clicked();
		void on_rb_magnitud_clicked();
};

#endif // VTN_FILTRO_H
