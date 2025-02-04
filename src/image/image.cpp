/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "image/image.hpp"

// https://github.com/nothings/stb
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#endif

#include <iostream>

namespace cge::image
{

void replace_all(std::string &in, const std::string &old_str, const std::string &new_str)
{
    size_t start_pos = 0;
    while((start_pos = in.find(old_str, start_pos)) != std::string::npos)
    {
        in.replace(start_pos, old_str.length(), new_str);
        start_pos += new_str.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void load_image_data(ImageData &im_data, const std::string &fname)
{
    stbi_set_flip_vertically_on_load(false);
    im_data.data =
        stbi_load(fname.c_str(), &im_data.w, &im_data.h, &im_data.channels, STBI_rgb_alpha);

    // We are explicityly setting the loaded image to RGBA (using STBI_rgb_alpha)
    im_data.channels = 4;

    im_data.bytes_per_row = im_data.channels * im_data.w;

    if(im_data.data == nullptr)
    {
        std::cout << "Error getting image data for " << fname << '\n';
        return;
    }
}

void free_image_data(ImageData &im_data)
{
    if(im_data.data == nullptr) return;

    // Free the image data
    stbi_image_free(im_data.data);
    im_data.data = nullptr;
}

SDLTextureInfo create_texture(const SDLInfo &sdl_info, const std::string &filepath)
{
    SDLTextureInfo result;

    std::string corrected_filepath(filepath);

#ifdef __BUILD_WIN__
    replace_all(corrected_filepath, "/", "\\");
#endif

    ImageData im_data;
    load_image_data(im_data, filepath);

    // TODO: Changed to different pixel format, which solved the problem of swapped color channels.
    result.texture = SDL_CreateTexture(sdl_info.renderer,
                                       SDL_PIXELFORMAT_ABGR8888,
                                       SDL_TEXTUREACCESS_STATIC,
                                       im_data.w,
                                       im_data.h);

    result.width = im_data.w;
    result.height = im_data.h;
    if(!SDL_UpdateTexture(result.texture, NULL, im_data.data, im_data.bytes_per_row))
    {
        std::cout << "ERROR Loading image using STB\n";
    }

    free_image_data(im_data);

    return result;
}

void destroy_texture(const SDLTextureInfo &texture_info)
{
    SDL_DestroyTexture(texture_info.texture);
}

} // namespace cge::image
