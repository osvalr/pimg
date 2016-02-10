#if !defined(_CLASSIMAGEN_CPP_)
# define _CLASSIMAGEN_CPP_

# include <cstdlib>
# include <cmath>

# include <sstream>
# include <fstream>
# include <iomanip>

# include "classImagen.h"

imagen::imagen()
{
    raster = NULL;
    tipo_imagen = SIN_TIPO;
    magic_number = PXM_ERROR;
    width = height = relacion_v = relacion_h = 0;
    max_val = 0;
}

imagen::imagen( size_t altura, size_t ancho, pixel_type_e ti, uint8_t nm, uint16_t mv )
{
	size_t i;
	height = altura;
	width = ancho;
	lu_table = ( pixel_t * ) new pixel_t[lu_table_size];

	raster = ( pixel_t ** ) new pixel_t* [height];
	for ( i = 0; i < height; ++i )
		raster[i] = ( pixel_t * ) new pixel_t [width];

	tipo_imagen = ti;
	magic_number = nm;
	max_val = mv;
}

imagen::imagen( const char *imgfile )
{
    D = M = NULL;
    cargar_imagen( imgfile );
}

imagen::~imagen()
{
    eliminar_imagen();
}

void imagen::eliminar_imagen()
{
    for ( size_t i = 0; i < height; ++i )
        if ( raster[i] )
            delete [] raster[i];
    delete [] raster;
    raster = NULL;

    delete [] lu_table;
    tipo_imagen = SIN_TIPO;
    magic_number = PXM_ERROR;
    width = height = 0;
    max_val = 0;

    D = M = NULL;
}

void imagen::cargar_imagen( const char *imgfile )
{
    std::ifstream img( imgfile, std::ios::in | std::ios::binary );
    bool continuar = true;
    int index;
    size_t i, j, k;
    paso_e paso = NMAGICO;
    uint8_t caracter, tmp[8], *row;
    lu_table = ( pixel_t * ) new pixel_t[lu_table_size];
    while ( continuar )
    {
        index = img.tellg();
        img.read( (char *)&caracter, 1 );

        if ( caracter == 8
            || caracter == 10
            || caracter == 13
            || caracter == 32 ) // TAB/LF/CR/SPACE considerado un espacio, se salta
            continue;

        img.seekg( index );
        if ( caracter == '#' )  // Se encontró el inicio de un comentario, ignora hasta LF mas próximo
        {
            while ( caracter != '\n' )
                img.read( (char *)&caracter, 1 );
            continue;
        }

        switch ( paso )
        {
            case NMAGICO:
                img.read( (char *)tmp, sizeof(uint8_t) * 2 );
                magic_number = tmp[1] - 48;

                if ( magic_number < PBM_ASCII || magic_number > PPM_BINARY )
                    magic_number = PXM_ERROR;

                if ( magic_number == PXM_ERROR )
                {
                    std::cerr << "<<Numero Mágico erróneo>>" << std::endl;
                    img.close();
                    exit(EXIT_FAILURE);
                }

                switch( magic_number )
                {
                    case PBM_ASCII:
                    case PBM_BINARY:
                        tipo_imagen = BIN;
                        break;
                    case PGM_ASCII:
                    case PGM_BINARY:
                        tipo_imagen = GRAY;
                        break;
                    case PPM_ASCII:
                    case PPM_BINARY:
                        tipo_imagen = RGB;
                }

                paso = DIMENSIONES;
                continue;

            case DIMENSIONES:
                i = 0;
                img.read( (char *)&(tmp[i]), 1 );
                while( !isspace(tmp[i]) )
                    img.read( (char *)&(tmp[++i]), 1 );
                width = atoi( (char *)tmp );

                i = 0;
                img.read( (char *)&(tmp[i]), 1 );
                while( !isspace(tmp[i]) )
                    img.read( (char *)&(tmp[++i]), 1 );
                height = atoi( (char *)tmp );

                raster = ( pixel_t ** ) new pixel_t* [height];
                for ( i = 0; i < height; ++i )
                    raster[i] = ( pixel_t * ) new pixel_t [width];

                if ( !raster || !raster[0] )
                {
					std::cerr << __FILE__ << "@" << __LINE__ << ": tipo_imagen=" << (unsigned)tipo_imagen << " al parecer el numero magico puede que esté fuera de rango" << std::endl;
                    exit( EXIT_FAILURE );
                }

                paso = MAX_VALOR;
                if ( magic_number == PBM_ASCII
                    || magic_number == PBM_BINARY )
                    continuar = false;
                continue;

            case MAX_VALOR:
                i = 0;
                img.read((char *)&(tmp[i]), 1 );
                while( !isspace(tmp[i]) )
                    img.read((char *)&(tmp[++i]), 1 );
                max_val = (uint16_t) atoi( (char *)tmp );
                continuar = false;
        }
    }

    switch ( magic_number )
    {
        case PBM_ASCII:
            for ( i = 0; i < height; ++i )
                for ( j = 0; j < width; ++j )
                {
                    img >> tmp[0];
                    raster[i][j].set_color( (componente_t) (tmp[0] - 48)? 255 : 0 );
                }
            break;
        case PGM_ASCII:
            for ( i = 0; i < height; ++i )
                for ( j = 0; j < width; ++j )
                {
                    img >> tmp;
                    raster[i][j].set_color( (componente_t) atoi( (char *) tmp ) );
                }
            break;

        case PPM_ASCII:
            for ( i = 0; i < height; ++i )
                for ( j = 0; j < width; ++j )
                {
                    img >> tmp;
                    raster[i][j].set_r( (componente_t) atoi((char *)tmp) );

                    img >> tmp;
                    raster[i][j].set_g( (componente_t) atoi((char *)tmp) );

                    img >> tmp;
                    raster[i][j].set_b( (componente_t) atoi((char *)tmp) );
                }
            break;

        case PBM_BINARY:
            for ( i = 0, j = -1; i < height; )
            {
                img.read( (char *) &tmp[0], 1 );
                for ( k = 8; k > 0; --k )
                {
                    i = j + 1 == width? i + 1 : i; // pasa a la siguiente línea sii se completaron las columnas de la actual
                    j = (j + 1) % width; // pasa a la siguiente columna
                    raster[i][j].set_color( (componente_t) tmp[0]>>(k-1) & 1? 0xFF : 0x0 );
                    if ( i + 1 == height && j + 1 == width )
                    {
                        i = height; // Para terminar el ciclo mas externo, dado que se ha alcanzado la ultima línea de la imagen
                        break;
                    }
                }
            }
            break;
        case PGM_BINARY:
            row = new uint8_t[width];
            for ( i = 0; i < height; ++i )
            {
                img.read( (char *) row, sizeof(uint8_t) * width);
                for ( j = 0; j < width; ++j )
                    raster[i][j].set_color( (componente_t) row[j] );
            }
            delete [] row;
            break;

        case PPM_BINARY:
/*
            img >> mr[i][j]; // Para PPM debe estar sobrecargado el ifstream
*/
            break;
    }
    relacion_h = relacion_v = 1;
    img.close();
}

uint8_t imagen::get_magic_number()
{
    return magic_number;
}

uint16_t imagen::get_max_val()
{
    return max_val;
}

size_t imagen::get_number_elements()
{
    return width * height;
}

size_t imagen::get_width()
{
    return width;
}

size_t imagen::get_height()
{
    return height;
}

size_t imagen::get_rvertical()
{
    return relacion_v;
}

size_t imagen::get_rhorizontal()
{
    return relacion_h;
}

pixel_type_e imagen::get_tipo_imagen()
{
    return tipo_imagen;
}

void imagen::set_numero_magico( uint8_t numero )
{
    magic_number = numero;
}

void imagen::set_valor_maximo( uint16_t valor )
{
    max_val = valor;
}

void imagen::set_tipo_imagen( pixel_type_e tipo )
{
    tipo_imagen = tipo;
}

std::string imagen::get_extension()
{
	switch ( magic_number )
	{
		case PBM_ASCII:
		case PBM_BINARY:
			return ".pbm";

		case PGM_ASCII:
		case PGM_BINARY:
			return ".pgm";

		case PPM_ASCII:
		case PPM_BINARY:
			return ".ppm";
	}
	return "(Error with file extension)";
}

componente_t imagen::from_lookup_table( int index )
{
	return lu_table[index].get_color();
}

componente_t imagen::get_pixel( size_t x, size_t y )
{
    if ( raster == NULL )
    {
        fprintf(stderr, "!raster@%s\n", __FILE__);
        return (componente_t) 0;
    }

	if ( raster[y] == NULL )
    {
        fprintf(stderr, "!raster[x]@%s\n", __FILE__);
        return (componente_t) 0;
    }

	return raster[y][x].get_color();
}

bool operator== ( imagen& img1, imagen& img2 )
{
    size_t i, j;

    if ( img1.magic_number != img2.magic_number
        || img1.max_val != img2.max_val
        || img1.height != img2.height
        || img1.width != img2.width
        || img1.tipo_imagen != img2.tipo_imagen )
        return false;

    for ( i = 0; i < img1.height; ++i )
    {
        for ( j = 0; j < img1.width
              && img1.raster[i][j] == img2.raster[i][j]; ++j )
            ;
        if ( j != img1.width )
            break;
    }

    if ( i != img1.height )
        return false;

    return true;
}

bool operator!= ( imagen& img1, imagen& img2 )
{
    size_t i, j;

    i = j = 0;
    if ( img1.magic_number != img2.magic_number
        || img1.max_val != img2.max_val
        || img1.height != img2.height
        || img1.width != img2.width
        || img1.tipo_imagen != img2.tipo_imagen )
        return true;

    for ( i = 0; i < img1.height; ++i )
    {
        for ( j = 0; j < img1.width
              && img1.raster[i][j] != img2.raster[i][j]; ++j )
            ;
        if ( j != img1.width )
            break;
    }

    if ( i == img1.height && j == img2.width )
        return false;

    return true;
}

std::ostream & operator<< ( std::ostream& out, imagen& img )
{
    size_t i , j;

	out << std::endl << "Filetype      : " << img.get_extension() << std::endl;
    out << "Número Mágico : P" << ( unsigned)img.magic_number << std::endl;
    out << "Ancho         : " << img.width << std::endl;
    out << "Alto          : " << img.height << std::endl;
    out << "Max Value     : " << img.max_val << std::endl << std::endl;

    for ( i = 0; i < img.height; ++i )
    {
        for ( j = 0; j < img.width; ++j )
            out << img.raster[i][j] << " ";
        out << std::endl;
    }
    return out;
}

bool imagen::guardar_imagen( const char *imgfile )
{
	size_t i, j;
	unsigned char gris;
	std::ofstream file;

	file.open( imgfile );
	file << "P"<<(unsigned) magic_number << '\n';
	file << (unsigned) width << ' ' << (unsigned) height << '\n';
	if ( magic_number != PBM_ASCII
		 &&  magic_number != PBM_BINARY )
		file << (unsigned) max_val << '\n';

	switch ( magic_number )
	{
		case PBM_ASCII:
			// No se ha desarrollado
			break;

		case PGM_ASCII:
			for ( i = 0 ; i < height; ++i )
			{
				for ( j = 0 ; j < width; ++j )
					file << raster[i][j].get_color() << ' ';
				file << '\n';
			}
			break;

		case PPM_ASCII:
			// No se ha desarrollado
			break;

		case PBM_BINARY:
			// No se ha desarrollado
			break;

		case PGM_BINARY:
			for ( i = 0 ; i < height; ++i )
				for ( j = 0 ; j < width; ++j )
				{
					gris = raster[i][j].get_color();
					file.write( (char *) &gris, 1);
				}
			break;

		case PPM_BINARY:
			// No se ha desarrollado
			break;
	}
	return true;
}

void imagen::set_pixel( size_t x, size_t y, pixel_t p )
{
	raster[y][x] = p;
	raster[y][x].set_tipo_pixel(GRAY);
}

void imagen::set_pixel( size_t x, size_t y, double p )
{
	raster[y][x].set_color( (componente_t) p );
	raster[y][x].set_tipo_pixel(GRAY);
}

imagen * imagen::ampliar( int metodo,  size_t rel_w, size_t rel_h )
{
    size_t i, j, k, l, inicio_i, inicio_j, fin_i, fin_j;
	imagen * nueva_imagen = NULL;
    componente_t valor_region;
	double a, b, x, y, _k, _l;

    if ( !raster || !rel_w || !rel_w )
		return NULL;

    //Almacena la nueva dimension de la imagen
	nueva_imagen = new imagen( height * rel_h, width * rel_w, tipo_imagen, magic_number, max_val);

	a = b = x = y = _k = _l = .0;
	switch ( metodo )
	{
		case METODO_IL: // Obtención de niveles de gris por medio de la Interpolacion Lineal
			for ( k = 0; k < height; ++k )
				for ( l = 0; l < width; ++l )
				{
					inicio_i = k * rel_h;
					inicio_j = l * rel_w;

					fin_i = inicio_i + rel_h;
					fin_j = inicio_j + rel_w;

					for ( i = inicio_i; i < fin_i ; ++i )
						for ( j = inicio_j; j < fin_j; ++j )
						{
							y = i / (double)rel_h;
							x = j / (double)rel_w;

							_k = (int) y;
							_l = (int) x;

							a = x - _l;
							b = y - _k;
							nueva_imagen->set_pixel( j, i, ( 1.0 - a ) * ( 1.0 - b ) * raster[ k ][ l ].get_color()
														+ ( l + 1 < width ? a * ( 1.0 - b ) * raster[ k ][ l + 1 ].get_color() : .0 )
														+ ( k + 1 < height ? ( 1.0 - a ) * b * raster[ k + 1 ][ l ].get_color() : .0 )
														+ ( k + 1 < height && l + 1 < width ? a * b * raster[ k + 1 ][ l + 1 ].get_color() : .0 ) );
						}
				}
			break;
		case METODO_VMP: // Obtencion de niveles de gris por medio del Vecino Más Próximo
			for ( k = 0; k < height; ++k )
				for ( l = 0; l < width; ++l )
				{
					valor_region = (componente_t) raster[k][l].get_color();

					inicio_i = k * rel_h;
					inicio_j = l * rel_w;

					fin_i = inicio_i + rel_h;
					fin_j = inicio_j + rel_w;

					for ( i = inicio_i; i < fin_i; ++i )
						for ( j = inicio_j; j < fin_j; ++j )
							nueva_imagen->set_pixel( j, i, valor_region );
				}

	}

	return nueva_imagen;
}

imagen * imagen::reducir( size_t rel_w, size_t rel_h )
{
	imagen * nueva_imagen = NULL;
	size_t i, j;

	if ( height / rel_w < 1
		|| width / rel_h < 1 )
		{
			std::cout << "No se puede disminuir esta imagen" << std::endl;
			return NULL;
		}

	nueva_imagen = new imagen( height / rel_h, width / rel_w, tipo_imagen, magic_number, max_val );

	for ( i = 0; i < nueva_imagen->get_height(); ++i )
		for ( j = 0; j < nueva_imagen->get_width(); ++j )
			nueva_imagen->set_pixel( j, i, raster[i * rel_h][j * rel_w].get_color() );

	return nueva_imagen;
}

imagen * imagen::negativo()
{
	size_t i, j;
	imagen * nueva_imagen = new imagen( height, width, tipo_imagen, magic_number, max_val );

	generar_look_up_table( imgn::NEGATIVO );

	for ( i = 0; i < height; ++i )
		for ( j = 0; j < width; ++j )
			nueva_imagen->set_pixel( j, i, lu_table[ raster[i][j].get_color() ].get_color() );

	return nueva_imagen;
}

imagen * imagen::umbralizar( size_t nivel )
{
	size_t i, j;

	imagen * nueva_imagen = new imagen( height, width, tipo_imagen, magic_number, max_val );

	generar_look_up_table( imgn::UMBRALIZACION, nivel );

	for ( i = 0; i < height; ++i )
		for ( j = 0; j < width; ++j )
			nueva_imagen->set_pixel( j, i, lu_table[ raster[i][j].get_color() ] );

	return nueva_imagen;
}

imagen * imagen::aumento_constraste()
{
	size_t i, j;
	imagen * nueva_imagen = new imagen( height, width, tipo_imagen, magic_number, max_val );

	generar_look_up_table( imgn::CONTRASTE );

	for ( i = 0; i < height; ++i )
		for ( j = 0; j < width; ++j )
			nueva_imagen->set_pixel( j, i, lu_table[ raster[i][j].get_color() ].get_color() );

	return nueva_imagen;
}

imagen * imagen::ecualizacion()
{
	size_t i, j;
	imagen * nueva_imagen = new imagen( height, width, tipo_imagen, magic_number, max_val );

	generar_look_up_table( imgn::ECUALIZACION );

	for ( i = 0; i < height; ++i )
		for ( j = 0; j < width; ++j )
			nueva_imagen->set_pixel( j, i, lu_table[ raster[i][j].get_color() ].get_color() );

	return nueva_imagen;
}

void imagen::set_rhorizontal( size_t rh )
{
    relacion_h = rh;
}

void imagen::set_rvertical( size_t rv )
{
    relacion_v = rv;
}

void imagen::generar_look_up_table( imgn::lookup_table_t t )
{
	size_t i, j;
	uint8_t Pm, r_min, r_max;
	double *table = NULL, acum, number_elements;

	switch ( t )
	{
		case imgn::NEGATIVO:
			for ( i = 0 ; i < lu_table_size; ++i )
				lu_table[i].set_color( (componente_t) 255 - i );
			break;

		case imgn::CONTRASTE:
			r_min = 255;
			r_max = 0;

			for( i = 0; i < height; ++i )
			   for( j = 0; j < width; ++j )
			   {
				   Pm = (uint8_t) raster[i][j].get_color();
				   r_min = r_min > Pm? Pm : r_min;
				   r_max = r_max < Pm? Pm : r_max;
			   }

			for( i = 0; i < lu_table_size; ++i )
				lu_table[i].set_color( (componente_t) ( i - r_min ) * ( 255 / ( r_max - r_min )) );
			break;

		case imgn::ECUALIZACION:

			table = new double[256];
			for( i = 0; i < 256; ++i )
				table[i] = 0;

			for( i = 0; i < height; ++i )
				for ( j = 0; j < width; ++j )
					table[raster[i][j].get_color()]++;

			number_elements = width * height;
			for( acum = .0, i = 0; i < 256; ++i )
			{
				acum += table[i];
				table[i] = acum * 255 / number_elements;
			}

			for ( i = 0; i < lu_table_size; ++i )
				lu_table[i].set_color( table[i] > 255.0 ? 255 : ( table[i] < 0 ? 0 : floor(table[i]) ) );

			delete [] table;
			break;
		default:
			std::cerr << __FILE__ << "@" << __LINE__ << ": argumento inválido, lo permitido es {lu::NEGATIVO|lu:CONTRASTE}" << std::endl;
	}
}

void imagen::generar_look_up_table( imgn::lookup_table_t t, size_t index )
{
	size_t i;

	if ( t != imgn::UMBRALIZACION )
	{
		std::cerr << __FILE__ << "@" << __LINE__ << ": argumento inválido, lo permitido es lu::UMBRALIZACION" << std::endl;
		return;
	}

	for ( i = 0 ; i < lu_table_size; ++i )
		lu_table[i].set_color( (componente_t) (i < index? 0x0 : 0xFF) );

}

void imagen::gradiente( unsigned __tipo, unsigned *mascaras, unsigned n )
{
	size_t i, j, k;
	filtro * f;
	double **Mk;

	if ( !n || !mascaras )
	{
		std::cerr << __FILE__ << "@" << __LINE__ << ": No se tiene ninguna mascara para procesar la imagen" << std::endl;
		return;
	}

	if ( !filtro::existe_operador( __tipo ) )
	{
		std::cerr << __FILE__ << "@" << __LINE__ << ": No existe el tipo de operador mascara["<< __tipo << "] recibida por parametros" << std::endl;
		return;
	}

	D = new double * [height];
	M = new double * [height];
	for ( i = 0; i < height; ++i )
	{
		D[i] = new double [width];
		M[i] = new double [width];
	}

	f = new filtro();

	f->set_imagen(this);

	for ( k = 0; k < n; ++k )
	{
		f->set_mascara( __tipo - 1, mascaras[k] );
		f->convolucion2();
		Mk = f->get_raster();
		for ( i = 0; i < height; ++i )
		{
			for ( j = 0; j < width; ++j )
			{
				M[i][j] = k? M[i][j] + fabs(Mk[i][j]) :  fabs(Mk[i][j]);
				D[i][j] = k? Mk[i][j] > D[i][j]? Mk[i][j] : D[i][j] : Mk[i][j];
			}
		}
	}

	delete f;
}

int ** imagen::laplaciano( unsigned __vecindad )
{
	size_t i, j;
	filtro * f;
	double **res;
	int ** lap_res = NULL;
	f = new filtro();

	lap_res = new int * [height];
	for ( i = 0; i < height; ++i )
		lap_res[i] = new int [width];

	f->set_imagen(this);
	f->set_mascara( Filtro::Operador::Laplaciano - 1, __vecindad );
	f->convolucion2();

	res = f->get_raster();

	for ( i = 0; i < height; ++i )
		for ( j = 0; j < width; ++j )
			lap_res[i][j] = res[i][j];

	delete f;
	return lap_res;
}

int ** imagen::reescalar( int **__src, size_t alto, size_t ancho )
{
	size_t i, j;
	double min = 0, max = 0;
	int ** res = NULL;

	res = new int * [alto];
	for ( i = 0; i < alto; ++i )
		res[i] = new int [ancho];

	// Busqueda del mínimo valor
	for ( i = 0; i < alto; ++i )
		for ( j = 0; j < ancho; ++j )
		{
			res[i][j] = __src[i][j];
			min = i == 0? res[i][j] : min > res[i][j]? res[i][j] : min;
		}

	// Sustración del mínimo con el raster y búsqueda del máximo valor
	for ( i = 0; i < alto; ++i )
		for ( j = 0; j < ancho; ++j )
		{
			res[i][j] -= min;
			max = i == 0? res[i][j] : max < res[i][j]? res[i][j] : max;
		}

	// Reescala de valo__src en la convolucion
	for ( i = 0; i < alto; ++i )
		for ( j = 0; j < ancho; ++j )
			res[i][j] *= (255.0/max);

	return res;
}

#endif // _CLASSIMAGEN_CPP_
