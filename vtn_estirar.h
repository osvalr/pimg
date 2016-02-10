#ifndef VTN_ESTIRAR_H
# define VTN_ESTIRAR_H

# include <QDialog>
# include <QIntValidator>
namespace Ui {
    class vtn_estirar;
}

typedef enum {
    ZOOM_ACERCAR = 0,
    ZOOM_ALEJAR
} tipo_estiramiento;

typedef enum {
    NO_LE = 0,
    HORIZONTAL_LE,
    VERTICAL_LE
} line_edit_id;

enum {
	VMP,
	IL
};

class vtn_estirar : public QDialog
{
    Q_OBJECT

	public:
		explicit vtn_estirar(QWidget *parent = 0);
		~vtn_estirar();
		void set_tipo( tipo_estiramiento );
		size_t get_ancho();
		size_t get_alto();
		int get_metodo();
		bool is_ok();
		void set_dimensiones_maximas( size_t max_w, size_t max_h );
	private:
		Ui::vtn_estirar *ui;
		bool zoom_mantener_aspecto;
		line_edit_id ultimo_le;
		QIntValidator *validator;
		tipo_estiramiento tipoe;
		size_t max_ancho, max_alto;
		int metodo;
		bool allok;
		static const unsigned MIN_ESTIRAMIENTO = 2;
		static const unsigned MAX_ESTIRAMIENTO = 10;

	private slots:
		void on_btn_rel_aspecto_clicked();
		void on_le_hor_textChanged(const QString &arg1);
		void on_le_ver_textChanged(const QString &arg1);
		void on_btn_aplicar_clicked();
		void on_btn_cancelar_clicked();
		void on_cb_metodo_currentIndexChanged( int );
};

#endif // VTN_ESTIRAR_H
