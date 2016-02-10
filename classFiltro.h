#ifndef CLASSFILTRO_H
# define CLASSFILTRO_H

# include "classImagen.h"
# include <stdint.h>

typedef double **mascara;

namespace Filtro {
	namespace Operador {
		enum {
			Sobel = 1,
			Prewitt,
			Laplaciano,
			Gaussiano,
			Media_Diez,
			Media_Dieciseis,
			Media_Nueve
		};
	}

	namespace Direccion {
		enum {
			Horizontal_LR = 0,
			Diagonal_UR,
			Vertical_BU,
			Diagonal_UL,
			Horizontal_RL,
			Diagonal_LB,
			Vertical_UB,
			Diagonal_BR,
			Eje_X = Vertical_BU,
			Eje_Y = Horizontal_LR
		};
	}

	namespace Vecindad {
		enum {
			CUATRO_VECINOS_CP = 0,
			CUATRO_VECINOS_CN,
			OCHO_VECINOS_CP,
			OCHO_VECINOS_CN
		};
	}
}

static const uint8_t NumeroDirecciones = 8;
static const uint8_t NumeroOperadores = 5;
static const uint8_t NumeroVecindad = 4;

static const int8_t f[3][NumeroDirecciones][3][3] =
{
	{
		{{ 1, 2, 1},
		 { 0, 0, 0},
		 {-1,-2,-1},},

		{{ 0, 1, 2},
		 {-1, 0, 1},
		 {-2,-1, 0},},

		{{-1, 0, 1},
		 {-2, 0, 2},
		 {-1, 0, 1},},

		{{-2,-1, 0},
		 {-1, 0, 1},
		 { 0, 1, 2},},

		{{-1,-2,-1},
		 { 0, 0, 0},
		 { 1, 2, 1},},

		{{ 0,-1,-2},
		 { 1, 0,-1},
		 { 2, 1, 0},},

		{{ 1, 0,-1},
		 { 2, 0,-2},
		 { 1, 0,-1},},

		{{ 2, 1, 0},
		 { 1, 0,-1},
		 { 0,-1,-2},},
	},{
		{{ 1, 1, 1},
		 { 0, 0, 0},
		 {-1,-1,-1},},

		{{ 0, 1, 1},
		 {-1, 0, 1},
		 {-1,-1, 0},},

		{{-1, 0, 1},
		 {-1, 0, 1},
		 {-1, 0, 1},},

		{{-1,-1, 0},
		 {-1, 0, 1},
		 { 0, 1, 1},},

		{{-1,-1,-1},
		 { 0, 0, 0},
		 { 1, 1, 1},},

		{{ 0,-1,-1},
		 { 1, 0,-1},
		 { 1, 1, 0},},

		{{ 1, 0,-1},
		 { 1, 0,-1},
		 { 1, 0,-1},},

		{{ 1, 1, 0},
		 { 1, 0,-1},
		 { 0,-1,-1},},

	},{
		{{ 0,-1, 0},{-1, 4,-1},{ 0,-1, 0},},
		{{ 0, 1, 0},{ 1,-4, 1},{ 0, 1, 0},},
		{{-1,-1,-1},{-1, 8,-1},{-1,-1,-1},},
		{{ 1, 1, 1},{ 1,-8, 1},{ 1, 1, 1},},
	},
};

static const int8_t media[2][3][3] =
{
	{{ 1, 1, 1},{ 1, 2, 1},{ 1, 1, 1},},
	{{ 1, 2, 1},{ 2, 4, 2},{ 1, 2, 1},},
};

class imagen;
class filtro
{
	private:
		imagen *I;

		mascara W;
		unsigned ancho, alto;

		double **raster;
		unsigned ancho_raster, alto_raster;

		double suma_coeficientes;
		double sigma;

		bool eliminar_raster();
		bool eliminar_mascara();

	public:
		filtro();
		filtro( imagen *i );
		filtro( int ancho, int alto, mascara m );
		~filtro();

		void set_mascara( int  w, int  h, mascara m );
		void get_mascara( int *w, int *h, mascara m );

		void set_mascara( unsigned w, unsigned h, unsigned __tipo_mascara );
		void set_mascara( unsigned __operador, unsigned __direccion  );

		static bool existe_operador ( unsigned __operador );

		void     set_imagen( imagen *i );
		imagen * get_imagen();

		void actualiza_imagen();

		void convolucion2();

		double **get_raster();
		void filtro_media( unsigned __tipo_media );
		void filtro_gaussiano( unsigned width, unsigned height, double sigma );

		void set_sigma( double v );
		double w( int x, int y );
		double G( unsigned x, unsigned y );

		static bool eliminar(unsigned width, unsigned height, double **e );
		void mostrar();

		double centro();
};

#endif // CLASSFILTRO_H
