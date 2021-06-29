#include "graphics.h"

Graphics::Graphics(const uint8_t width, const uint8_t height, const uint8_t scale_factor, const std::string& prog) :
	scale_factor_(scale_factor), window_(sf::VideoMode(width * scale_factor_, height * scale_factor_), prog)
{
	auto desk { sf::VideoMode::getDesktopMode() };  
	window_.setPosition(sf::Vector2i(desk.width / 4, desk.height / 4));
};
