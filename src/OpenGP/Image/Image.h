#pragma once

#include <regex>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <limits>
#include <cassert>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <OpenGP/headeronly.h>
#include <OpenGP/types.h>
#include <OpenGP/MLogger.h>

//=============================================================================
namespace OpenGP {
//=============================================================================


template <class PixelType>
using Image = Eigen::Matrix< PixelType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor >;

class ImageReader {
protected:

    void *private_data;

public:

    using ReadFunction = std::function<void(int, int, int, double)>;

    std::string path;

    int components;
    int width, height;

    virtual ~ImageReader() {}

    virtual void read_info() = 0;
    virtual void read_data(const ReadFunction&) = 0;

    void set_path(const std::string &path) { this->path = path; }

};

class PNGReader : public ImageReader {
public:
    ~PNGReader();
    void read_info();
    void read_data(const ReadFunction&);
};

class TGAReader : public ImageReader {
public:
    ~TGAReader();
    void read_info();
    void read_data(const ReadFunction&);
};

class ImageWriter {
protected:

    void *private_data;

public:

    //using ReadFunction = std::function<void(int, int, int, double)>;

    std::string path;

    //int components;
    //int width, height;

    virtual ~ImageWriter() {}

    //virtual void read_info() = 0;
    //virtual void read_data(const ReadFunction&) = 0;

    void set_path(const std::string &path) { this->path = path; }

};

class PNGWriter : public ImageWriter {
public:
    ~PNGWriter();
    //void read_info();
    //void read_data(const ReadFunction&);
};

class TGAWriter : public ImageWriter {
public:
    ~TGAWriter();
    //void read_info();
    //void read_data(const ReadFunction&);
};


// --- Pixel type is a vector or matrix ----------------------------------------

template <typename ImageType, typename=typename ImageType::Scalar::Scalar>
struct ImageTypeInfo {

    static constexpr int component_count = ImageType::Scalar::RowsAtCompileTime * ImageType::Scalar::ColsAtCompileTime;

    using Scalar = typename ImageType::Scalar::Scalar;

    static Scalar &channel_ref(ImageType &I, int row, int col, int c) {
        assert(c < component_count && c >= 0);
        return I(row, col)(c);
    }

};

// -----------------------------------------------------------------------------


// --- Pixel type is not a vector or matrix ------------------------------------

template <typename ImageType>
struct ImageTypeInfo<ImageType, void> {

    static constexpr int component_count = 1;

    using Scalar = typename ImageType::Scalar;

    static Scalar &channel_ref(ImageType &I, int row, int col, int c) {
        assert(c == 0);
        return I(row, col);
    }

};

// -----------------------------------------------------------------------------


template <typename Scalar>
typename std::enable_if<std::is_integral<Scalar>::value, Scalar>::type scalar_transfer(double val) {
    return val * std::numeric_limits<Scalar>::max();
}

template <typename Scalar>
typename std::enable_if<!std::is_integral<Scalar>::value, Scalar>::type scalar_transfer(double val) {
    return Scalar(val);
}


template <typename ImageType>
void imread(const char* path, ImageType& I) {

    std::regex suffix_regex(R"regex([\S\s]*\.([^\.]+)$)regex");

    std::cmatch match;
    if (!std::regex_match(path, match, suffix_regex))
        mFatal() << "Could not identify the image file type";

    std::string suffix = match[1];
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

    std::unique_ptr<ImageReader> reader;

    if (suffix == "png") {
        reader = std::unique_ptr<ImageReader>(new PNGReader());
    } else if (suffix == "tga") {
        reader = std::unique_ptr<ImageReader>(new TGAReader());
    } else {
        mFatal() << "Unknown image type suffix";
    }

    reader->set_path(path);

    reader->read_info();

    I.resize(reader->height, reader->width);

    int image_type_components = ImageTypeInfo<ImageType>::component_count;
    using Scalar = typename ImageTypeInfo<ImageType>::Scalar;

    ImageReader::ReadFunction read_function;

    if (reader->components == 1) {

        // duplicate single channel into first 3 provided channels
        if (image_type_components < 4) {
            read_function = [&](int row, int col, int c, double val) {
                for (int i = 0;i < image_type_components;i++) {
                    ImageTypeInfo<ImageType>::channel_ref(I, row, col, i) = scalar_transfer<Scalar>(val);
                }
            };
        } else if (image_type_components == 4) {
            // set alpha to 1
            read_function = [&](int row, int col, int c, double val) {
                for (int i = 0;i < 3;i++) {
                    ImageTypeInfo<ImageType>::channel_ref(I, row, col, i) = scalar_transfer<Scalar>(val);
                }
                ImageTypeInfo<ImageType>::channel_ref(I, row, col, 3) = scalar_transfer<Scalar>(1.0);
            };
        } else {
            mFatal() << "Image type and image file are incompatible";
        }

    } else if (reader->components == 3) {

        if (image_type_components == 1) {
            // use average of rgb values
            I.setZero();
            read_function = [&](int row, int col, int c, double val) {
                ImageTypeInfo<ImageType>::channel_ref(I, row, col, 1) += scalar_transfer<Scalar>(val / 3);
            };
        } else if (image_type_components == 3) {
            read_function = [&](int row, int col, int c, double val) {
                ImageTypeInfo<ImageType>::channel_ref(I, row, col, c) = scalar_transfer<Scalar>(val);
            };
        } else if (image_type_components == 4) {
            // set alpha to 1
            read_function = [&](int row, int col, int c, double val) {
                ImageTypeInfo<ImageType>::channel_ref(I, row, col, c) = scalar_transfer<Scalar>(val);
                if (c == 2)
                    ImageTypeInfo<ImageType>::channel_ref(I, row, col, 3) = scalar_transfer<Scalar>(1.0);
            };
        } else {
            mFatal() << "Image type and image file are incompatible";
        }

    } else if (reader->components == 4) {

        if (image_type_components == 1) {
            // use average of rgb values
            I.setZero();
            read_function = [&](int row, int col, int c, double val) {
                if (c != 3)
                    ImageTypeInfo<ImageType>::channel_ref(I, row, col, 1) += scalar_transfer<Scalar>(val / 3);
            };
        } else if (image_type_components == 3) {
            // drop alpha channel
            read_function = [&](int row, int col, int c, double val) {
                if (c < 3)
                    ImageTypeInfo<ImageType>::channel_ref(I, row, col, c) = scalar_transfer<Scalar>(val);
            };
        } else if (image_type_components == 4) {
            read_function = [&](int row, int col, int c, double val) {
                ImageTypeInfo<ImageType>::channel_ref(I, row, col, c) = scalar_transfer<Scalar>(val);
            };
        } else {
            mFatal() << "Image type and image file are incompatible";
        }

    } else if (image_type_components == reader->components) {

        read_function = [&](int row, int col, int c, double val) {
            ImageTypeInfo<ImageType>::channel_ref(I, row, col, c) = scalar_transfer<Scalar>(val);
        };

    } else {
        mFatal() << "Image type and image file are incompatible";
    }

    reader->read_data(read_function);

}

template <typename ImageType>
void imwrite(const char* path, const ImageType &I) {

}

//=============================================================================
} // namespace OpenGP
//=============================================================================

#ifdef HEADERONLY
    #include "PNGImage.cpp"
    #include "TGAImage.cpp"
#endif
