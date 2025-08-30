#ifndef __SPHERE_H
#define __SPHERE_H

#include <DirectXMath.h>
#include "ChiliMath.h"
#include "IndexedTriangleList.h"

class Sphere
{
public:
    template <class V>
    static IndexedTriangleList<V> MakeTesselated(int lat_div, int long_div)
    {
        /*
           Северный полюс
               ▲
               │
            ◄-----┼-----► долготы
               │
               ▼
            Южный полюс

            Пояса (parallels): lat_div - 2 колец
            Меридианы (meridians): long_div линий
            Вершины: (lat_div - 1) * long_div + 2 (пояса + полюса)
            Треугольники: 2 * (lat_div - 2) * long_div (пояса) + 2 * long_div (шапки)
        */
        namespace dx = DirectX;
        assert(lat_div >= 3);
        assert(long_div >= 3);

        constexpr float radius = 1.0f;
        const auto      base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f); // Верхняя точка
        const float     lattitude_angle = PI / lat_div;           // Угол между параллелями
        const float     longitude_angle = 2.0f * PI / long_div;   // Угол между меридианами

        // Генерация вершин "поясов" сферы
        std::vector<V> vertices;
        for (int iLat = 1; iLat < lat_div; iLat++)
        {
            const auto latBase = dx::XMVector3Transform(base, dx::XMMatrixRotationX(lattitude_angle * iLat));
            for (int iLong = 0; iLong < long_div; iLong++)
            {
                vertices.emplace_back();
                auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longitude_angle * iLong));
                dx::XMStoreFloat3(&vertices.back().pos, v);
            }
        }

        // Северный полюс
        const auto iNorthPole = (unsigned short)vertices.size();
        vertices.emplace_back();
        dx::XMStoreFloat3(&vertices.back().pos, base);

        // Южный полюс
        const auto iSouthPole = (unsigned short)vertices.size();
        vertices.emplace_back();
        dx::XMStoreFloat3(&vertices.back().pos, dx::XMVectorNegate(base));

        const auto calcIdx = [lat_div, long_div](unsigned short iLat, unsigned short iLong)
        {
            return iLat * long_div + iLong; // Преобразование 2D→1D координат
        };

        // Генерация индексов для "поясов"
        std::vector<unsigned short> indices;
        for (unsigned short iLat = 0; iLat < lat_div - 2; iLat++)
        {
            for (unsigned short iLong = 0; iLong < long_div - 1; iLong++)
            {
                // Два треугольника на ячейку
                indices.push_back(calcIdx(iLat, iLong));
                indices.push_back(calcIdx(iLat + 1, iLong));
                indices.push_back(calcIdx(iLat, iLong + 1));
                indices.push_back(calcIdx(iLat, iLong + 1));
                indices.push_back(calcIdx(iLat + 1, iLong));
                indices.push_back(calcIdx(iLat + 1, iLong + 1));
            }

            // Замыкание полосы (последняя ячейка в ряду)
            indices.push_back(calcIdx(iLat, long_div - 1));
            indices.push_back(calcIdx(iLat + 1, long_div - 1));
            indices.push_back(calcIdx(iLat, 0));
            indices.push_back(calcIdx(iLat, 0));
            indices.push_back(calcIdx(iLat + 1, long_div - 1));
            indices.push_back(calcIdx(iLat + 1, 0));
        }

        // cap fans
        for (unsigned short iLong = 0; iLong < long_div - 1; iLong++)
        {
            // Северная шапка (веер треугольников)
            indices.push_back(iNorthPole);
            indices.push_back(calcIdx(0, iLong));
            indices.push_back(calcIdx(0, iLong + 1));

            // Южная шапка (аналогично)
            indices.push_back(calcIdx(lat_div - 2, iLong + 1));
            indices.push_back(calcIdx(lat_div - 2, iLong));
            indices.push_back(iSouthPole);
        }
        // Замыкание северной шапки
        indices.push_back(iNorthPole);
        indices.push_back(calcIdx(0, long_div - 1));
        indices.push_back(calcIdx(0, 0));

        // Замыкание южной шапки
        indices.push_back(calcIdx(long_div - 2, 0));
        indices.push_back(calcIdx(long_div - 2, long_div - 1));
        indices.push_back(iSouthPole);

        return {std::move(vertices), std::move(indices)};
    }

    template <class V>
    static IndexedTriangleList<V> Make()
    {
        return MakeTesselated<V>(12, 24);
    }
};

#endif //__SPHERE_H
