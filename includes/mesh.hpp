/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:29:40 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:29:40 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <memory>
#include <string>
#include <vector>

class mesh
{
    public:
        // bool loadOBJ(const std::string& path);

        // const std::vector<Vertex>& vertices() const;
        // const std::vector<uint32_t>& indices() const;

    private:
        //std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
};