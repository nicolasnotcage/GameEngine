/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/camera.hpp"
#include "platform/math.hpp"

#include <cmath>
#include <cstring>

namespace cge
{

Camera::Camera(float center_x, float center_y, float height, float width) 
	: center_x(center_x), center_y(center_y), height(height), width(width) 
	{
}

void Camera::set_position(float x, float y)
{
    center_x = x;
    center_y = y;
}

void Camera::set_dimensions(float x, float y)
{
    width = x;
    height = y;
}

void Camera::move(float dx, float dy)
{
    center_x += dx;
    center_y += dy;
}

void Camera::zoom(float factor)
{
    width *= factor;
    height *= factor;
}

Vector2 Camera::get_position() const { return Vector2(center_x, center_y); }
float   Camera::get_width() const { return width; }
float   Camera::get_height() const { return height; }

Matrix3 Camera::get_world_to_screen_matrix(int screen_width, int screen_height) const
{
    // Formula from slides
    // 1. Translate camera center to origin
    Matrix3 translate_center = Matrix3::translation_matrix(-center_x, -center_y);

    // 2. Scale world to normalized device coordinates
    Matrix3 scale_to_ndc = Matrix3::scale_matrix(2.0f / width, 2.0f / height);

    // 3. Flip y-axis to match screen coordinates
    Matrix3 flip_y = Matrix3();
    flip_y.a[4] = -1.0f;

    // 4. Scale to screen dimensions
    Matrix3 scale_to_screen = Matrix3::scale_matrix(screen_width / 2.0f, screen_height / 2.0f);

    // 5. Translate to screen center
    Matrix3 translate_to_screen = Matrix3::translation_matrix(screen_width / 2.0f, screen_height / 2.0f);

    // Combine all transformations (right to left)
    return translate_to_screen * scale_to_screen * flip_y * scale_to_ndc * translate_center;
}

Vector2 Camera::world_to_screen(const Vector2 &world_position,
                                int            screen_width,
                                int            screen_height) const
{
    // Get the transformation matrix and apply it to the world position
    Matrix3 transform = get_world_to_screen_matrix(screen_width, screen_height);
    return transform * world_position;
}

Vector2 Camera::screen_to_world(const Vector2 &screen_position,
                                int            screen_width,
                                int            screen_height) const
{
    // 1. Translate screen position relative to screen center
    float screen_x = screen_position.x - screen_width / 2.0f;
    float screen_y = screen_position.y - screen_height / 2.0f;

    // 2. Scale from screen to normalized device coordinates
    screen_x /= (screen_width / 2.0f);
    screen_y /= (screen_height / 2.0f);

    // 3. Flip Y back
    screen_y = -screen_y;

    // 4. Scale from normalized device coordinates to world coordinates
    float world_x = (screen_x * width / 2.0f) + center_x;
    float world_y = (screen_y * height / 2.0f) + center_y;

    return Vector2(world_x, world_y);
}

} // namespace cge
