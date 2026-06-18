/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:37:11 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:37:12 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "window.hpp"
#include <stdexcept>

window::window(uint32_t width, uint32_t height, const std::string& title):
      _window(nullptr),
      _width(width), _height(height),
      _running(true)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL_Init failed");
    }

    // IMPORTANT : flag Vulkan
    std::cout << title << ":" << width << height << std::endl;
    _window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!_window)
        throw std::runtime_error("SDL_CreateWindow failed");
}

void window::pollEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            _running = false;
    }
}

// getters
SDL_Window* window::get() const
{
    return _window;
}
uint32_t window::width() const
{
    return _width;
}
uint32_t window::height() const
{
    return _height;
}

bool window::should_close() const
{
    return !_running;
}
window::~window()
{
    if (_window)
        SDL_DestroyWindow(_window);

    SDL_Quit();
}