#pragma once

#include <cstdint>
#include <array>
#include <string>

#include <SFML/Graphics.hpp>

class Graphics final
{
public:
    explicit Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog);
    Graphics() = delete;
    ~Graphics() = default;
    Graphics(const Graphics& other) = delete;
    constexpr Graphics(Graphics&& other) = delete;
    Graphics& operator=(const Graphics& other) = delete;
    constexpr Graphics& operator=(Graphics&& other) = delete;

    template<uint8_t width, uint8_t height>
    void draw_window(const std::array<std::array<uint8_t, width>, height>& screen);

    sf::RenderWindow _window;

private:
    const uint8_t _scale_factor;
};

template<uint8_t width, uint8_t height>
void Graphics::draw_window(const std::array<std::array<uint8_t, width>, height>& screen)
{
    _window.clear(sf::Color::Black);
    sf::RectangleShape pixel(sf::Vector2f(_scale_factor, _scale_factor));
    for (int i = 0; i < height; i++)
    {
    	for(int j = 0; j < width; j++)
    	{
    		if (screen[i][j])
    		{
                pixel.setPosition(j * _scale_factor, i * _scale_factor);
                pixel.setFillColor(sf::Color::White);
                _window.draw(pixel);
    		}
    	}
    }
    _window.display();
}