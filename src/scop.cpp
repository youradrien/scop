/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:29:51 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:29:52 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "scop.hpp"

Scop::Scop(const std::string& objFile):
    m_objFile(objFile),
    _window(nullptr), 
    _vk(nullptr), 
    _mesh(nullptr),
    //_camera(nullptr),
    _running(false)
{
    (void)(objFile);
}

void Scop::init()
{
    this->_window = std::unique_ptr<window>(
        new window(1280, 720, "scop")
    );
    this->_vk = std::unique_ptr<vulkan_context>(
        new vulkan_context(*(this->_window))
    );
    this->_running = true;
}

void Scop::loop()
{
    while (_running && !_window->should_close())
    {
        _window->pollEvents();
        // _vk->drawFrame();
    }
}

void Scop::run()
{
    init();
    loop();
    cleanup();
}

void Scop::cleanup()
{
    if (_vk)
    {
        // _vk->waitIdle();
        // _vk.reset();
        // _vk = nullptr;
    }

    if (_window)
    {
        _window.reset();
        _window = nullptr;
    }

    SDL_Quit();
}

Scop::~Scop()
{
}