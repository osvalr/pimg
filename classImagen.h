#if !defined(_CLASSIMAGEN_H_)
# define _CLASSIMAGEN_H_

# include <cstdio>
# include <cstring>

# include <ostream>
# include <string>
# include <fstream>

# include "classPixel.h"
# include "classFiltro.h"

namespace imgn {
	typedef enum {
		UMBRALIZACION = 0,
		NEGATIVO,
		CONTRASTE,
		ECUALIZACION
	} lookup_table_t;

}
enum {
	METODO_VMP = 1,
	METODO_IL
};

enum pnm_e {
	PXM_ERROR = 0,
	PBM_ASCII,
	PGM_ASCII,
	PPM_ASCII,
	PBM_BINARY,
	PGM_BINARY,
	PPM_BINARY
};

enum paso_e {
	NMAGICO = 0,
	DIMENSIONES,
	MAX_VALOR
};

typedef pixel pixel_t;

static const size_t lu_table_size = 256;

//class filtro;
class imagen
{
    friend bool operator== ( imagen&, imagen& );
	friend bool operator!= ( imagen&, imagen& );
	friend std::ostream& operator<< ( std::ostream&, imagen& );

	private:
	    pixel_t **raster;         // Raster de la imagen rgb, 1 byte por componente
	    size_t width;             // Ancho de la imagen
	    size_t height;            // Alto de la imagen

	    uint8_t magic_number;     // Número Mágico
	    uint16_t max_val;         // Maximo valor especificado en el archivo leído {255|65535}
	    pixel_type_e tipo_imagen; // Tipo de imagen {BIN|GRAY|RGB}
	    size_t relacion_h;        // Relacion Horizontal ( Por defecto, 1 )
	    size_t relacion_v;        // Relacion Vertical ( Por defecto, 1 )

	    pixel_t *lu_table;        // Tabla LookUp
	    bool crear_raster( size_t __ancho, size_t __alto, double __matriz );

	public:
		    double **M, **D;
	    imagen();
	    imagen( size_t, size_t, pixel_type_e, uint8_t, uint16_t );
	    imagen( const char * );
	    ~imagen();

	    // Metodos de obtención
	    pixel_type_e get_tipo_imagen();
	    componente_t get_pixel( size_t x, size_t y);
	    uint8_t get_magic_number();
	    uint16_t get_max_val();
	    size_t get_number_elements();
	    size_t get_width();
	    size_t get_height();
	    size_t get_rvertical();
	    size_t get_rhorizontal();
	    std::string get_extension();

	    // Metodos de asignacion
	    void set_rhorizontal( size_t );
	    void set_rvertical( size_t );
	    void set_numero_magico( uint8_t );
	    void set_valor_maximo( uint16_t );
	    void set_tipo_imagen( pixel_type_e __tipo_imagen );
	    void set_pixel( size_t x, size_t y, pixel_t );
	    void set_pixel( size_t x, size_t y, double );

	    // Herramientas de la imagen
	    bool guardar_imagen( const char * );
	    void cargar_imagen( const char * );
	    void eliminar_imagen();

	    // Métodos de transformaciones básicas de la imagen
	    imagen * ampliar( int,  size_t, size_t );
	    imagen * reducir( size_t, size_t );
	    imagen * negativo();
	    imagen * umbralizar( size_t );
	    imagen * aumento_constraste();
	    imagen * ecualizacion();

	    // Metodos para la creación/uso del lookup table
	    void generar_look_up_table( imgn::lookup_table_t );
	    void generar_look_up_table( imgn::lookup_table_t, size_t __indice );
	    componente_t from_lookup_table( int );

	    // Metodos para la Detección de Bordes
		void gradiente( unsigned __operador, unsigned *__conjunto_mascaras, unsigned __cantidad_mascaras );
		int ** laplaciano( unsigned __vecindad );
		static int ** reescalar( int **__src, size_t alto, size_t ancho );
};

#endif //_CLASSIMAGEN_H_
