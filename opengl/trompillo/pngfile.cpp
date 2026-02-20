
// https://gist.github.com/jeroen/10eb17a9fb0e5799b772
// http://www.libpng.org/pub/png/libpng-manual.txt

#include "framework.h"
#include "pngfile.h"

// constructor
CPngFile::CPngFile()
{
	width = height = 1;
	buffer = NULL;
}

//destructor
CPngFile::~CPngFile()
{
	if (buffer != NULL) delete[] buffer;
}

//
bool CPngFile::Open(wchar_t* filename)
{
	FILE* pf;
	errno_t err;
	const unsigned int number = 8;
	png_byte header[number];
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte color_type, bit_depth;
	size_t i, k, rowbytes;
	png_bytep* row_pointers;

	// open file
	if ((err = _wfopen_s(&pf, filename, L"rb")) != 0) return false;

	// read header
	if (fread(header, 1, number, pf) != number) {
		fclose(pf);
		return false;
	}

	//  checks whether the given number of bytes match the PNG signature
	if (png_sig_cmp(header, 0, number)) {
		fclose(pf);
		return false;
	}

	// allocate and initialize a png_struct structure
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(pf);
		return false;
	}

	// allocate and initialize a png_info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// when libpng encounters an error, it expects to longjmp back to your routine
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// initialize the default input/output functions for the PNG file to standard C streams
	png_init_io(png_ptr, pf);

	// to inform libpng that we have read that many bytes from the start of the file
	png_set_sig_bytes(png_ptr, number);

	// reads the information before the actual image data from the PNG file
	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	// updates the structure pointed to by info_ptr to reflect any transformations that have been requested
	png_read_update_info(png_ptr, info_ptr);

	// returns the number of bytes needed to hold a transformed row of an image
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// allocate memory for the image
	if (buffer != NULL) delete[] buffer;
	buffer = new png_byte[rowbytes * height];
	if (buffer == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// allocate row pointer for interlaced image
	row_pointers = new png_bytep[height];
	if (row_pointers == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// point row to the beginning of a row of pixels
	k = 0;
	for (i = 0; i < height; i++) {
		row_pointers[i] = &buffer[k];
		k += rowbytes;
	}

	// reads the entire image into memory at once
	png_read_image(png_ptr, row_pointers);

	// deallocate memory
	delete[] row_pointers;

	// close file
	fclose(pf);

	return true;
}

//
void CPngFile::Read(unsigned int* width, unsigned int* height, unsigned char** buffer)
{
	*width = this->width;
	*height = this->height;
	*buffer = this->buffer;
}

//
void CPngFile::Close()
{
	if (buffer != NULL) delete[] buffer;
	buffer = NULL;
}

//
bool CPngFile::Save(wchar_t* filename, size_t width, size_t height, unsigned char* buffer)
{
	FILE* pf;
	errno_t err;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep* row_pointers;
	size_t i, k, rowbytes;

	// open file
	if ((err = _wfopen_s(&pf, filename, L"wb")) != 0) return false;

	// allocate and initialize a png_struct structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(pf);
		return false;
	}

	// allocate and initialize a png_info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// when libpng encounters an error, it expects to longjmp back to your routine
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// initialize the default input/output functions for the PNG file to standard C streams
	png_init_io(png_ptr, pf);

	// set image header information in info_ptr. width is the image width in pixels. height is the image height in pixels
	// 8 bit depth, RGB format
	png_set_IHDR(
		png_ptr,
		info_ptr,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	// writes the PNG information in info_ptr to file
	png_write_info(png_ptr, info_ptr);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	// png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);

	// allocate row pointer for interlaced image
	row_pointers = new png_bytep[height];
	if (row_pointers == NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(pf);
		return false;
	}

	// point row to the beginning of a row of pixels
	rowbytes = 3 * width;
	k = 0;
	for (i = 0; i < height; i++) {
		row_pointers[height - 1 - i] = &buffer[k];
		k += rowbytes;
	}

	// writes all of the pixel data to the file
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);

	// deallocate memory
	delete[] row_pointers;

	// close file
	fclose(pf);

	return true;
}
