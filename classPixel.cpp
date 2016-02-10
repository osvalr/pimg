#if !defined(_CLASSPIXEL_CPP_)
# define _CLASSPIXEL_CPP_

# include "classPixel.h"

pixel::pixel( )
{
    tipo_pixel = SIN_TIPO;
    r = g = b = 0;
}

pixel::pixel( pixel_type_e _t )
{
    tipo_pixel = _t;
}

pixel::pixel( componente_t _r, componente_t _g, componente_t _b )
{
    r = _r;
    g = _g;
    b = _b;
    tipo_pixel = RGB;
}

pixel::pixel( componente_t color )
{
    r = b = g = color;
    tipo_pixel = GRAY;
}

pixel::~pixel()
{}

bool pixel::set_color( componente_t color )
{
	tipo_pixel = GRAY;
	r = b = g = color;
	return true;
}

unsigned pixel::get_color()
{
    return (unsigned) r;
}
void pixel::set_r( componente_t _r )
{
    r = _r;
}

void pixel::set_g( componente_t _g )
{
    g = _g;
}

void pixel::set_b( componente_t _b )
{
    b = _b;
}

void pixel::set_tipo_pixel ( pixel_type_e _t )
{
    tipo_pixel = _t;
}

componente_t pixel::get_r()
{
    return r;
}

componente_t pixel::get_g()
{
    return g;
}

componente_t pixel::get_b()
{
    return b;
}

std::ostream& operator<< ( std::ostream& out, pixel& pixel1 )
{
    switch ( pixel1.tipo_pixel )
    {
        case RGB:
            out << "(";
            out << std::setfill('0') << std::setw(3) << (unsigned)pixel1.r;
            out << ",";
            out << std::setfill('0') << std::setw(3) << (unsigned)pixel1.g;
            out << ",";
            out << std::setfill('0') << std::setw(3) << (unsigned)pixel1.b;
            out << ")";
            break;

        case BIN:
        case GRAY:
            out << std::setfill('0') << std::setw(3) << (unsigned)pixel1.r;
            break;
        case SIN_TIPO:
            out << "(NaN)";

    }
    return out;
}

std::istream& operator>> ( std::istream &in, pixel& c )
{
	in >> std::skipws >> c.r >> c.g >> c.b;
	return in;
}

bool operator== ( pixel& pixel1, pixel& pixel2 )
{
    if ( pixel1.tipo_pixel != pixel2.tipo_pixel )
        return false;

    if ( pixel1.tipo_pixel <= GRAY )
        return pixel1.r == pixel2.r;
    else
        return pixel1.r == pixel2.r
            && pixel1.g == pixel2.g
            && pixel1.b == pixel2.b;
}

bool operator!= ( pixel& pixel1, pixel& pixel2 )
{
    if ( pixel1.tipo_pixel == pixel2.tipo_pixel )
       return false;

    if ( pixel1.tipo_pixel <= GRAY )
        return pixel1.r != pixel2.r;
    else
        return pixel1.r != pixel2.r
            || pixel1.g != pixel2.g
            || pixel1.b != pixel2.b;
}

pixel & pixel::operator= ( pixel & p )
{
    if ( this == &p )
        return *this;
    switch ( p.tipo_pixel )
    {
        case BIN:
        case GRAY:
            this->set_color( (componente_t) p.get_color() );
            this->set_tipo_pixel(GRAY);
            break;
        case RGB:
            this->set_rgb( p.get_r(), p.get_g(), p.get_b() );
            this->set_tipo_pixel(RGB);
            break;
		case SIN_TIPO:
			std::cout << __FILE__ << "@" << __LINE__ << "no se tiene definido un tipo de pixel para este objeto." << std::endl;
    }
    return *this;
}

void pixel::set_rgb( componente_t _r, componente_t _g, componente_t _b )
{
    r = _r;
    g = _g;
    b = _b;
}

#endif // _CLASSPIXEL_CPP_
