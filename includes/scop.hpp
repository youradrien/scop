/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:29:34 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:31:15 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "window.hpp"
#include "vulkan_context.hpp"
#include "mesh.hpp"
#include "camera.hpp"
// class window;
// class vulkancontext;
// class mesh;
// class camera;

class Scop
{
    public:

        Scop(const std::string& objFile);
        ~Scop();

        Scop(const Scop&) = delete;
        Scop& operator=(const Scop&) = delete;

        void run();

    private:

        void init();
        void loop();
        void cleanup();
        void update(float deltaTime);
        void render();
        
        std::string m_objFile;

        std::unique_ptr<window>         _window;
        std::unique_ptr<vulkan_context>  _vk;
        std::unique_ptr<mesh>           _mesh;
        //std::unique_ptr<camera>         _camera;

        bool _running;
};
