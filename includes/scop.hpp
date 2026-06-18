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

class Window;
class VulkanContext;
class Mesh;
class Camera;

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
        void mainLoop();
        void cleanup();

        void update(float deltaTime);
        void render();
        
        std::string m_objFile;

        std::unique_ptr<Window>         _window;
        std::unique_ptr<VulkanContext>  _vk;
        std::unique_ptr<Mesh>           _mesh;
        std::unique_ptr<Camera>         _camera;

        bool _running;
};
