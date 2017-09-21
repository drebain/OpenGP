#include <stdio.h>

#include <png.h>

#include "Image.h"



//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    struct PNGReaderData {

        FILE *fid;
        png_struct *read_struct;
        png_info *info_struct;

    };

}

PNGReader::~PNGReader() {

    PNGReaderData *data = (PNGReaderData*)private_data;

    if (data != nullptr) {
        png_read_end(data->read_struct, nullptr);
        png_destroy_read_struct(&(data->read_struct), &(data->info_struct), nullptr);
        fclose(data->fid);
    }

    delete data;

}

void PNGReader::read_info() {

    private_data = new PNGReaderData();

    PNGReaderData *data = (PNGReaderData*)private_data;

    using filep = FILE*;

    filep &fid = data->fid;

    fid = fopen(path.c_str(), "rb");
    if (fid == nullptr)
        mFatal() << "Could not open file";

    uint8_t header[8];
    if( fread(header, sizeof(header), 1, fid) != 1 )
        mFatal() << "Cannot read PNG header";

    if (png_sig_cmp(header, 0, 8))
        mFatal() << "File is not a PNG image";

    png_structp &read_struct = data->read_struct;
    png_infop &info_struct = data->info_struct;

    read_struct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_struct = png_create_info_struct(read_struct);

    png_init_io(read_struct, fid);
    png_set_sig_bytes(read_struct, 8);

    png_read_info(read_struct, info_struct);

    width = png_get_image_width(read_struct, info_struct);
    height = png_get_image_height(read_struct, info_struct);
    components = png_get_channels(read_struct, info_struct);

}

void PNGReader::read_data(const ReadFunction &read_function) {

    PNGReaderData *data = (PNGReaderData*)private_data;

    using filep = FILE*;

    filep &fid = data->fid;
    png_structp &read_struct = data->read_struct;
    png_infop &info_struct = data->info_struct;

    int passes = png_set_interlace_handling(read_struct);
    png_read_update_info(read_struct, info_struct);

    Eigen::Matrix<uint8_t*, Eigen::Dynamic, Eigen::Dynamic> row_ptrs(height, 1);
    for (int i = 0;i < height;i++) {
        row_ptrs(i) = (uint8_t*)malloc(png_get_rowbytes(read_struct, info_struct));
    }

    png_read_image(read_struct, row_ptrs.data());

    int bit_depth = png_get_bit_depth(read_struct, info_struct);
    int row_width = components * width * 8;

    for (int row=0;row < height;row++) {
        for (int col=0;col < width;col++) {
            for (int c=0;c < components;c++) {

                uint32_t int_val = 0;

                int start_idx = (components * col + c) * (bit_depth / 8);
                for (int i=0;i < bit_depth / 8;i++) {
                    int_val <<= 8;
                    int_val |= row_ptrs(row)[start_idx + i];
                }

                double val = ((double)int_val) / (((uint64_t)0x1) << bit_depth);
                read_function(row, col, c, val);
            }
        }
    }

    for (int i = 0;i < height;i++) {
        free(row_ptrs(i));
    }

}

//=============================================================================
} // OpenGP::
//=============================================================================
