#include <chrono>
#include <cmath>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

#include <png++/png.hpp>

#include <SDL.h>
#include <SDL2pp/SDL.hh>


struct Point4
{
    Point4(double x, double y, double z, double w) noexcept:
        x(x), y(y), z(z), w(w)
    { }

    inline void operator+=(const Point4& p) noexcept
    {
        x += p.x;
        y += p.y;
        z += p.z;
        w += p.w;
    }

    inline Point4 operator+(const Point4& p) const noexcept
    {
        Point4 c = *this;
        c += p;
        return c;
    }

    inline void operator*=(double k) noexcept
    {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
    }

    inline Point4 operator*(double k) const noexcept
    {
        Point4 c = *this;
        c *= k;
        return c;
    }

    inline Point4 operator-() const noexcept
    {
        return Point4(-x, -y, -z, -w);
    }

    inline void operator-=(const Point4& p) noexcept
    {
        *this += -p;
    }

    inline Point4 operator-(const Point4& p) const noexcept
    {
        Point4 c = *this;
        c -= p;
        return c;
    }

    double x;
    double y;
    double z;
    double w;
};


struct Point2
{
    Point2(double x, double y) noexcept:
        x(x), y(y)
    { }

    inline void operator+=(const Point2& p) noexcept
    {
        x += p.x;
        y += p.y;
    }

    inline Point2 operator+(const Point2& p) const noexcept
    {
        Point2 c = *this;
        c += p;
        return c;
    }

    inline Point2 operator-() const noexcept
    {
        return Point2(-x, -y);
    }

    inline void operator-=(const Point2& p) noexcept
    {
        *this += -p;
    }

    inline Point2 operator-(const Point2& p) const noexcept
    {
        Point2 c = *this;
        c -= p;
        return c;
    }

    inline void rotate(double angle) noexcept
    {
        auto cos_a = cos(angle);
        auto sin_a = sin(angle);
        auto new_x = x * cos_a - y * sin_a;
        auto new_y = x * sin_a + y * cos_a;
        x = new_x;
        y = new_y;
    }

    inline Point2 rotated(double angle) const noexcept
    {
        Point2 c = *this;
        c.rotate(angle);
        return c;
    }

    double x;
    double y;
};


struct Texture
{
    Texture(png::image<png::rgb_pixel> image):
        image(std::move(image))
    { }

    size_t width() const
    {   
        return image.get_width();
    }

    size_t height() const
    {   
        return image.get_height();
    }

    png::image<png::rgb_pixel> image;
};


struct Block
{
    std::shared_ptr<Texture> top;
    std::shared_ptr<Texture> bottom;
    std::shared_ptr<Texture> left;
    std::shared_ptr<Texture> right;
    std::shared_ptr<Texture> front;
    std::shared_ptr<Texture> back;
};


namespace sdl = SDL2pp;

/*

std::optional<Point2> project(const Point4& p, const Point4& camera, double alpha, double phi)
{
    auto r = p - camera;
    auto xz = Point2(r.x, r.z);
    xz.rotate(-alpha);
    auto t = sqrt(xz.x * xz.x + xz.y * xz.y);
    if (t != 0) {
        auto yt = Point2(r.y, t);
        yt.rotate(-phi);
        r.x *= yt.y / t;
        r.z *= yt.y / t;
        r.y = yt.x;
    }

    
    static const double near = 0.1;
    static const double far = 1000.0;
    static const double fov = M_PI / 2.0;
    static const double screen_height = 0.15;
    static const double screen_width = 0.2;

    if (r.x * r.x + r.y * r.y + r.z * r.z <= near) {
        return {};
    }

    double hdist = sqrt(r.x * r.x + r.z * r.z);
    double proj_y = (r.y / hdist * near) / (screen_height/2);
    double proj_x = x / z * near / (screen_width/2);
    return Point2(proj_x, proj_y);
}

*/


struct Coords
{
    bool operator<(const Coords& other) const
    {
        return std::make_tuple(x, y, z) < std::make_tuple(other.x, other.y, other.z);
    }

    int x;
    int y;
    int z;
};


struct Nothing
{ };


using World = std::map<Coords, Block>;


struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};


struct Target
{
    int a;
    int b;
    int c;
    int d;
    int x1;
    int y1;
    int x2;
    int y2;
    std::shared_ptr<Texture> texture;
};


Color trace_ray(const Point4& camera, const Point4& delta, const World& world)
{
    std::pair<double, Color> min_value = {std::numeric_limits<double>::infinity(), {0, 0, 0, 255}};
    std::vector<Target> targets;
    for (const auto& [coords, block] : world) {
        targets.push_back({-1,  0,  0, coords.x, coords.y, coords.z, coords.y + 1, coords.z + 1, block.left});
        targets.push_back({-1,  0,  0, coords.x + 1, coords.y, coords.z, coords.y + 1, coords.z + 1, block.right});
        targets.push_back({ 0, -1,  0, coords.y, coords.x, coords.z, coords.x + 1, coords.z + 1, block.bottom});
        targets.push_back({ 0, -1,  0, coords.y + 1, coords.x, coords.z, coords.x + 1, coords.z + 1, block.top});
        targets.push_back({ 0,  0, -1, coords.z, coords.x, coords.y, coords.x + 1, coords.y + 1, block.front});
        targets.push_back({ 0,  0, -1, coords.z + 1, coords.x, coords.y, coords.x + 1, coords.y + 1, block.back});
    }

    for (const auto& target : targets) {
        double num = target.a * camera.x + target.b * camera.y + target.c * camera.z + target.d;
        double den = target.a * delta.x + target.b * delta.y + target.c * delta.z;
        double t = -num / den;
        if (t < 0 || t > min_value.first || isnan(t)) {
            continue;
        }

        auto point = camera + delta * t;
        double flatx, flaty;
        if (target.a != 0) {
            flatx = point.y;
            flaty = point.z;
        } else if (target.b != 0) {
            flatx = point.x;
            flaty = point.z;
        } else {
            flatx = point.x;
            flaty = point.y;
        }

        if (flatx < target.x1 || flatx > target.x2 || flaty < target.y1 || flaty > target.y2) {
            continue;
        }

        double kx = flatx - target.x1;
        double ky = flaty - target.y1;
        if (kx < 0) {
            kx = 0;
        }
        if (kx >= 1) {
            kx = 1 - 1e-7;
        }
        if (ky < 0) {
            ky = 0;
        }
        if (ky >= 1) {
            ky = 1 - 1e-7;
        }

        if (target.a != 0) {
            std::swap(kx, ky);
            ky = 1 - 1e-7 - ky;
        } else if (target.b != 0) {
            // nothing
        } else {
            ky = 1 - 1e-7 - ky;
        }

        int pixel_x = int(kx * target.texture->width());
        int pixel_y = int(ky * target.texture->height());
        assert(pixel_x >= 0);
        assert(pixel_y >= 0);

        //std::cerr << kx << ", " << ky << std::endl;
        assert(pixel_x < int(target.texture->width()));
        assert(pixel_y < int(target.texture->height()));

        //Color color = {uint8_t(255 * kx), uint8_t(255 * ky), 70, 255};
        auto pixel = target.texture->image[pixel_y][pixel_x];
        min_value = {t, {pixel.red, pixel.green, pixel.blue}};
    }
    return min_value.second;
}


int main()
{
    const int img_width = 800;
    const int img_height = 600;
    const double screen_distance = 0.2;
    const double screen_width = 0.4;
    const double screen_height = 0.3;
    png::image<png::rgb_pixel> img(800, 600);


    auto grass_top = std::make_shared<Texture>(png::image<png::rgb_pixel>("textures/grass/top.png"));
    auto grass_bottom = std::make_shared<Texture>(png::image<png::rgb_pixel>("textures/grass/bottom.png"));
    auto grass_side = std::make_shared<Texture>(png::image<png::rgb_pixel>("textures/grass/side.png"));
    Block grass = {grass_top, grass_bottom, grass_side, grass_side, grass_side, grass_side};
    
    World world;
    world.emplace(Coords{0, 0, 5}, grass);
    world.emplace(Coords{-2, -1, 4}, grass);
    world.emplace(Coords{1, -2, 3}, grass);
    world.emplace(Coords{1, 2, 5}, grass);
    //world.emplace(Coords{2, 1, 3}, Nothing{});
    //world.emplace(Coords{2, 0, 3}, Nothing{});


    for (int x = 0; x < img_width; ++x) {
        for (int y = 0; y < img_height; ++y) {
            double wx = double(x) / img_width * screen_width - screen_width / 2;
            double wy = -(double(y) / img_height * screen_height - screen_height / 2);
            double wz = screen_distance;
            Point4 camera(0, 0, 0, 0);
            Point4 point_on_screen(wx, wy, wz, 0);
            auto delta = point_on_screen - camera;
            auto color = trace_ray(camera, delta, world);
            img[y][x] = png::rgb_pixel(color.r, color.g, color.b);
        }
    }

    img.write("output.png");
    /*
    try {
        sdl::SDL sdl(SDL_INIT_VIDEO);
        sdl::Window window("Blockrenderer window", SDL_WINDOWPOS_UNDEFINED, 800, 600);
        sdl::Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
        //sdl::Surface buf(0, 800, 600, 24, 0x0000'00ff, 0x0000'ff00, 0x00ff'0000, 0xff00'0000);`
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << "Frame... ";
            
            renderer.SetDrawColor(0, 0, 0, 255);
            renderer.Clear();
            //render_frame(renderer);
            //renderer.Clear();
            //renderer.Copy(buf);

            std::cerr << "done" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    */
    //Point4 p1(
}
