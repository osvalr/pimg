#if !defined( CLASSPIXEL_H_)
# define _CLASSPIXEL_H_

# include <stdint.h>

# include <iostream>
# include <iomanip>
# include <string>

typedef uint8_t componente_t; /* 0..255 = 256 valores */

typedef enum {
    SIN_TIPO = 0,
    BIN,
    GRAY,
    RGB
} pixel_type_e;

class pixel {

    friend std::ostream& operator<< ( std::ostream &, pixel& );
    friend std::istream& operator>> ( std::istream &, pixel& );
    friend bool operator== ( pixel&, pixel& );
    friend bool operator!= ( pixel&, pixel& );


    private:
        componente_t r,g,b;
        pixel_type_e tipo_pixel;


    public:
        pixel();
        pixel( pixel_type_e );
        pixel( componente_t );
        pixel( componente_t r, componente_t g, componente_t b );
        ~pixel();

        pixel & operator= ( pixel& );

        void set_tipo_pixel ( pixel_type_e );
        void set_r( componente_t );
        void set_g( componente_t );
        void set_b( componente_t );
        void set_rgb( componente_t, componente_t, componente_t  );

        bool set_color( componente_t );
        unsigned get_color();
        componente_t get_r();
        componente_t get_g();
        componente_t get_b();

};
#endif // _CLASSPIXEL_H_
