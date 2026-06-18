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

class VulkanContext;

class Window
{
    public:

        Window(uint32_t w, uint32_t h, const std::string& title);
        ~Window();

        bool shouldClose() const;
        void pollEvents();

        GLFWwindow* handle() const;

    private:

        GLFWwindow* _window;

};