#include "classFiltro.h"
#include <cstdlib>
#include <cmath>
#include <QString>

filtro::filtro()
{
	suma_coeficientes = 0;
	sigma = 0;
	W = NULL;
	I = NULL;
	raster = NULL;
	ancho = alto = 0;
	ancho_raster = alto_raster = 0;
}

filtro::filtro( int w, int h, mascara c )
{
	int x, y;

	y = sizeof( c ) / sizeof( double * );
	x = sizeof( c[0] ) / sizeof( double );

	if ( w != x || h != y )
	{
		std::cout << "Las dimensiones de la mascara c no coinciden con la dimension recibida [ " << __LINE__ << " @ " << __FILE__ << "]" << std::endl;
		return;
	}

	if ( !w || !h )
	{
		std::cout << "La dimensión no puede ser cero [ " << __LINE__ << " @ " << __FILE__ << "]"<< std::endl;
		return;
	}

	if ( !c )
		std::cout << "La estructura de coeficientes es NULL, se usará 1 en todos sus valores [ " << __LINE__ << " @ " << __FILE__ << "]" << std::endl;

	set_mascara( w, h, c );
}

filtro::~filtro()
{
	eliminar_mascara();
	eliminar_raster();
	I = NULL;
}

void filtro::set_mascara( int dim_w, int dim_h, mascara c )
{
	size_t i, j;
	I = NULL;
	if ( !dim_w%2 || !dim_h%2 )
	{
		std::cout << "La dimensión 'dim' debe ser un valor impar [ " << __LINE__ << " @ " << __FILE__ << "]" << std::endl;
		return;
	}

	raster = W = NULL;
	ancho_raster = alto_raster = 0;
	ancho = dim_w;
	alto = dim_h;
	W = new double* [alto];
	for ( i = 0; i < alto; ++i )
		W[i] = new double [ancho];

	suma_coeficientes = .0;
	for ( i = 0; i < alto; ++i )
		for ( j = 0; j < ancho; ++j )
		{
			W[i][j] = c == NULL? 1.0 : c[i][j];
			if ( c != NULL )
				suma_coeficientes += c[i][j];
		}
}

void filtro::get_mascara( int *width, int *height, mascara c )
{
	size_t i, j;

	*width = ancho;
	*height = alto;

	c = new double* [alto];
	for ( i = 0; i < alto; ++i )
	{
	    c[i] = new double [ancho];
	    for ( j = 0; j < ancho; ++j )
		    c[i][j] = W[i][j];
	}
}

void filtro::set_mascara( unsigned width, unsigned height, unsigned __tipo_mascara )
{
    size_t i, j;

	if ( ancho != width && alto != height )
	{
		eliminar_mascara();
		ancho = width;
		alto = height;
		W = new double* [alto];
			for ( i = 0; i < alto; ++i )
				W[i] = new double [ancho];
	}

	if ( __tipo_mascara == Filtro::Operador::Media_Dieciseis
		||  __tipo_mascara == Filtro::Operador::Media_Diez )
			__tipo_mascara -= 5;

	suma_coeficientes = .0;
    for ( i = 0; i < alto; ++i )
    {
		for ( j = 0; j < ancho; ++j )
		{
			switch ( __tipo_mascara )
			{
				case Filtro::Operador::Media_Nueve:
					W[i][j] = 1.0;
					break;

				case Filtro::Operador::Gaussiano:
					W[i][j] = G( j, i );
					break;

				default: // Media 10 y Media 16
					W[i][j] = media[__tipo_mascara][i][j];
			}
			suma_coeficientes += W[i][j];
		}
	}
}

void filtro::set_mascara( unsigned __operador, unsigned __direccion  )
{
	size_t i, j;

	if ( __operador >= NumeroOperadores )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: Operador fuera de rango : " << __operador <<  std::endl;
		return;
	}

	if ( __direccion >= NumeroDirecciones )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: Direccion fuera de rango : " << __direccion <<  std::endl;
		return;
	}

	if ( ancho != 3 && alto != 3 )
	{
		eliminar_mascara();

		ancho = alto = 3;
		W = new double* [alto];
			for ( i = 0; i < alto; ++i )
				W[i] = new double [ancho];
	}
	for ( i = 0; i < alto; ++i )
		for ( j = 0; j < ancho; ++j )
			W[i][j] = f[__operador][__direccion][i][j];
}

bool filtro::existe_operador ( unsigned __operador )
{
	return __operador <= NumeroOperadores;
}

void filtro::set_imagen( imagen *img )
{
	if ( !img )
		return;

	I = img;

	eliminar_raster();
	ancho_raster = I->get_width();
	alto_raster = I->get_height();
	raster = new double* [alto_raster];
	for ( size_t i = 0; i < alto_raster; ++i )
		raster[i] = new double[ancho_raster];
}

imagen * filtro::get_imagen()
{
	return I;
}

void filtro::actualiza_imagen()
{
	int width = ( ancho - 1 ) / 2;
	int height = ( alto - 1 ) / 2;
	for ( size_t i = height; i < I->get_height() - height; ++i )
		for ( size_t j = width; j < I->get_width() - width; ++j )
			I->set_pixel( j, i, (componente_t) raster[i][j] );

}

void filtro::convolucion2()
{
	int i, j, k, l,width, height;
	double valor;

	if ( !I )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: No se puede aplicar este metodo sin una imagen asociada en I" << std::endl;
		return;
	}

	if ( !I->get_height() || !I->get_width() )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: No se puede aplicar este metodo con dimensiones 0 en la imagen" << std::endl;
		return;
	}

	// Calculo de borde de la mascara horizontal y verticalmente
	width = ( ancho - 1 ) / 2;
	height = ( alto - 1 ) / 2;

	// Se inicializan los bordes de columna ( dependientes al ancho de la
	// mascara ) con los valores de la imagen original
	for ( i = 0; i < (int)alto_raster; ++i )
		for ( j = 0; j < (int) (ancho - 1)/2; ++j )
		{
			raster[i][j] = I->get_pixel( j, i );
			raster[i][ancho_raster - (ancho -1)/2 + j] = I->get_pixel( ancho_raster - (ancho -1)/2 + j, i );
		}

	// Se inicializan los bordes de fila ( dependientes al alto de la
	// mascara ) con los valores de la imagen original
	for ( i = 0; i < (int)( alto - 1 ) / 2; ++i )
		for ( j = 0; j < (int)ancho_raster; ++j )
		{
			raster[i][j] = I->get_pixel( j, i );
			raster[alto_raster - (alto - 1)/2 + i][j] = I->get_pixel( j, alto_raster -(alto - 1)/2 + i );
		}

	// Se aplica la convolucion de la imagen y se almacena en el raster para su
	// posterior calculo en la imagen
	for ( i = height; i < (int)I->get_height() - height; ++i )
		for ( j = width; j < (int)I->get_width() - width; ++j )
		{
			valor = .0;
			for ( k = -height; k <= height; ++k )
				for ( l = -width; l <= width; ++l )
					valor += w( l, k ) * I->get_pixel( j + l, i + k );
			raster[i][j] = valor;
		}
}

void filtro::filtro_media( unsigned __tipo_media )
{
    size_t i, j;
	set_mascara( 3, 3, __tipo_media );
	convolucion2();

	for ( i = 0; i < alto_raster; ++i )
		for ( j = 0; j < ancho_raster; ++j )
			raster[i][j] /= suma_coeficientes;

	actualiza_imagen();
}

void filtro::filtro_gaussiano( unsigned width, unsigned height, double s )
{
	size_t i, j;
	set_sigma(s);
	set_mascara( width, height, Filtro::Operador::Gaussiano);
    convolucion2();

	for ( i = 0; i < alto_raster; ++i )
		for ( j = 0; j < ancho_raster; ++j )
			raster[i][j] /= suma_coeficientes;

	actualiza_imagen();
}

bool filtro::eliminar(unsigned width, unsigned height, double **m )
{
	if ( m && width && height )
	{
		for ( size_t i = 0; i <  height; ++i )
			delete [] m[i];
		delete [] m;
		m = NULL;
		width = height = 0;
		return true;
	}
	return false;
}

bool filtro::eliminar_raster()
{
	return eliminar( ancho_raster, alto_raster, raster );
}

bool filtro::eliminar_mascara()
{
	return eliminar( ancho, alto, W );
}

void filtro::set_sigma( double v )
{
	sigma = v;
}

double filtro::G( unsigned x, unsigned y )
{
	return exp(( -( pow( x, 2 ) + pow( y, 2 ) ) / 2 ) / ( 2 * pow( sigma, 2 ))) / ( 2 * M_PI * pow( sigma, 2 ));
}

double filtro::w( int x, int y )
{
	int i, j;

	i = ( alto - 1 ) / 2;  // Indice del centro vertical de la mascara
	j = ( ancho - 1 ) / 2; // Indice del centro horizontal de la mascara

	if ( i + y < 0 || i + y >= (int) alto )
	{

		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: El valor de 'i' excede los límites de la mascara, se usará i = -y " << std::endl;
		i = -y;
	}

	if ( j + x < 0 || j + x >= (int) ancho )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: El valor de 'j' excede los límites de la mascara, se usará j = -x " << std::endl;
		j = -x;
	}
	return W[i + y][j + x];
}

double ** filtro::get_raster()
{
	return raster;
}

void filtro::mostrar()
{
	size_t i, j;
	for ( i = 0; i < alto; ++i )
	{
		for ( j = 0; j < ancho; ++j )
			std::cout << W[i][j] << " ";
		std::cout << std::endl;
	}
	if ( alto )
		std::cout << std::endl;
}

double filtro::centro()
{
	if ( !W || !alto || !ancho )
	{
		std::cout << "[" << __LINE__ << " @ " << __FILE__ << "]: No se puede obtener el centro, no está definida la Mascara" << std::endl;
		return .0;
	}
	return W[(alto-1)/2][(ancho-1)/2];
}
