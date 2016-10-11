#include <stdlib.h>
#include <stdio.h>
#include <png.h>

#include "predprey.h"

#define DENSITY_INTERVAL 25
#define IMAGE_INTERVAL 50

const char *basename = "out/%03d.png";

void output_init(lattice_t *lattice) {
	// do nothing
}

static void create_image(lattice_t *lattice, const char *fname) {
	FILE *fp = fopen(fname, "w");
	if (fp == NULL) {
		perror("create_image()");
		fail(NULL);
	}

	png_structp png_ptr = 
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		fail("create_image(): png_create_write_struct() failed");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		fclose(fp);
		fail("create_image(): png_create_info_struct() failed");
	}

	if (setjmp(png_jmpbuf(png_ptr)) != 0) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		fail("create_image(): png creation failed");
	}

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, lattice->size, lattice->size, 8,
			PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	png_bytep row = (png_bytep)malloc(3 * lattice->size * sizeof(png_byte));

	for (int y = 0; y < lattice->size; y++) {
		for (int x = 0; x < lattice->size; x++) {
			switch(get_cell(lattice, x, y)) {
				case PREDATOR:
					row[3 * x] = 0xFF;
					row[3 * x + 1] = 0;
					row[3 * x + 2] = 0;
					break;
				case PREY:
					row[3 * x] = 0;
					row[3 * x + 1] = 0xFF;
					row[3 * x + 2] = 0;
					break;
				case EMPTY:
				default:
					row[3 * x] = 0xFF;
					row[3 * x + 1] = 0xFF;
					row[3 * x + 2] = 0xFF;
					break;
			}
		}
		png_write_row(png_ptr, row);
	}

	free(row);

	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
}

void output(lattice_t *lattice) {
	if (lattice == NULL)
		fail("output(): lattice cannot be NULL");

	if (lattice->time % DENSITY_INTERVAL == 0) {
		printf("%f\t%f\n", predator_density(lattice), prey_density(lattice));
	}

	if (lattice->time % IMAGE_INTERVAL == 0) {
		char fname[16];
		sprintf(fname, basename, lattice->time / IMAGE_INTERVAL);
		create_image(lattice, fname);
	}
}

void output_teardown(lattice_t *lattice) {
	// do nothing
}