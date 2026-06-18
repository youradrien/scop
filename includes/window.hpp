/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:46:40 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:46:41 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>


class window
{
    public:
        window(uint32_t width, uint32_t height, const std::string& title);
        ~window();

        window(const window&) = delete;
        window& operator=(const window&) = delete;

        void pollEvents();

        // getters
        SDL_Window* get() const;
        uint32_t width() const;
        uint32_t height() const;
        bool should_close() const;

    private:
        SDL_Window* _window;

        uint32_t _width;
        uint32_t _height;

        bool _running;
};