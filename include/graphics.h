#pragma once

#include <cstdint>
#include <array>
#include <string>

#include <SFML/Graphics.hpp>

class Graphics final
{
public:
    Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog);
    Graphics() = delete;
    ~Graphics() = default;
    // Graphics(const Graphics& other) = delete;
    // constexpr Graphics(Graphics&& other) = delete;
    // Graphics& operator=(const Graphics& other) = delete;
    // constexpr Graphics& operator=(Graphics&& other) = delete;

    sf::RenderWindow window_;

    template<uint8_t width, uint8_t height>
    void draw_window(const std::array<std::array<uint8_t, width>, height>& screen);
private:
    const uint8_t scale_factor_;
};

template<uint8_t width, uint8_t height>
void Graphics::draw_window(const std::array<std::array<uint8_t, width>, height>& screen)
{
    window_.clear(sf::Color::Black);
    sf::RectangleShape pixel(sf::Vector2f(scale_factor_, scale_factor_));
    for (int i = 0; i < height; i++)
    {
    	for(int j = 0; j < width; j++)
    	{
    		if (screen[i][j])
    		{
                pixel.setPosition(j * scale_factor_, i * scale_factor_);
                pixel.setFillColor(sf::Color::White);
                window_.draw(pixel);
    		}
    	}
    }
    window_.display();
}