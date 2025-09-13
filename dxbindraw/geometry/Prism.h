#ifndef __PRISM_H
#define __PRISM_H
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Prism
{
public:
    template <class V>
    static IndexedTriangleList<V> MakeTesselated(int long_div)
    {
        namespace dx = DirectX;
        assert(long_div >= 3);

        /*
              ▲ Z
              │
              ● (центр дальний, Z=1)
             /│\
            / │ \
   ●        ──●──● (дальнее основание)
   │          │  │
   ●        ──●──● (ближнее основание)
            \ │ /
             \│/
              ● (центр ближний, Z=-1)
         */

        const auto  base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);    // Базовая точка на ближнем основани
        const auto  offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);   // Смещение к дальнему основанию
        const float longitude_angle = 2.0f * PI / long_div;                                // Угол между гранями

        // Центр ближнего основания (Z = -1)
        std::vector<V> vertices;
        vertices.emplace_back();
        vertices.back().pos = {0.0f, 0.0f, -1.0f};
        const auto iCenterNear = (unsigned short)(vertices.size() - 1);

        // Центр дальнего основания (Z = 1)
        vertices.emplace_back();
        vertices.back().pos = {0.0f, 0.0f, 1.0f};
        const auto iCenterFar = (unsigned short)(vertices.size() - 1);

        // base vertices
        for (int iLong = 0; iLong < long_div; iLong++)
        {
            // Ближнее основание
            {
                vertices.emplace_back();
                auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));  // Вращение вокруг Z
                dx::XMStoreFloat3(&vertices.back().pos, v); // Z = -1
            }

            // Дальнее основание
            {
                vertices.emplace_back();
                auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));
                v = dx::XMVectorAdd(v, offset);  // Смещаем к Z = 1
                dx::XMStoreFloat3(&vertices.back().pos, v);
            }
        }

        // Генерация индексов для боковых граней
        std::vector<unsigned short> indices;
        for (unsigned short iLong = 0; iLong < long_div; iLong++)
        {
            const auto i = iLong * 2;       // Индекс начала текущей пары вершин
            const auto mod = long_div * 2;  // Модуль для замыкания

            // Два треугольника на боковую грань (quad)
            indices.push_back(i + 2);               // Ближняя вершина текущая
            indices.push_back((i + 2) % mod + 2);   // Ближняя вершина следующая
            indices.push_back(i + 1 + 2);           // Дальняя вершина текущая
            indices.push_back((i + 2) % mod + 2);   // Ближняя вершина следующая
            indices.push_back((i + 3) % mod + 2);   // Дальняя вершина следующая
            indices.push_back(i + 1 + 2);           // Дальняя вершина текущая
        }

		// Генерация индексов для оснований
        for (unsigned short iLong = 0; iLong < long_div; iLong++)
        {
            const auto i = iLong * 2;
            const auto mod = long_div * 2;

            // Ближнее основание (веер треугольников)
            indices.push_back(i + 2);               // Текущая вершина
            indices.push_back(iCenterNear);             // Центр ближнего основания
            indices.push_back((i + 2) % mod + 2);   // Следующая вершина

            // Дальнее основание (веер треугольников)
            indices.push_back(iCenterFar);              // Центр дальнего основания
            indices.push_back(i + 1 + 2);           // Текущая вершина
            indices.push_back((i + 3) % mod + 2);   // Следующая вершина
        }

        return {std::move(vertices), std::move(indices)};
	}
    /*
     * В чем отличие от обычного MakeTesselated?
     * Раздельные вершины: В обычной версии вершины оснований и боковой поверхности shared, здесь - отдельные
     * Индивидуальные нормали: Каждая группа вершин имеет свои оптимизированные нормали
     * Лучшее освещение: Основания выглядят плоскими, боковая поверхность - гладкой
     *
     * Визуализация структуры вершин:
     * Вершины: [CN] [N0, N1, N2...] [CF] [F0, F1, F2...] [FN0, FF0, FN1, FF1, FN2, FF2...]
     * Где:
     * CN - центр ближний
     * Nx - вершины ближнего основания
     * CF - центр дальний
     * Fx - вершины дальнего основания
     * FNx/FFx - вершины боковой поверхности (отдельные!)
     */
    template <class V>
    static IndexedTriangleList<V> MakeTesselatedIndependentCapNormals(int long_div)
    {
        namespace dx = DirectX;
        assert(long_div >= 3);

        const auto  base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);    //начальная точка на ближнем основании
        const auto  offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);   // смещение к дальнему основанию
        const float longitude_angle = 2.0f * PI / long_div;  // угол между соседними гранями

        std::vector<V> vertices;

        // Центр ближнего основания
        const auto iCenterNear = (unsigned short)vertices.size();
        vertices.emplace_back();
        vertices.back().pos = {0.0f, 0.0f, -1.0f};
        vertices.back().n = {0.0f, 0.0f, -1.0f};

        // Ближнее основание (отдельные вершины с нормалями "вниз")
        const auto iBaseNear = (unsigned short)vertices.size();
        for (int iLong = 0; iLong < long_div; iLong++)
        {
            vertices.emplace_back();
            auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));
            dx::XMStoreFloat3(&vertices.back().pos, v);
            vertices.back().n = {0.0f, 0.0f, -1.0f};
        }

        // Центр дальнего основания
        const auto iCenterFar = (unsigned short)vertices.size();
        vertices.emplace_back();
        vertices.back().pos = {0.0f, 0.0f, 1.0f};
        vertices.back().n = {0.0f, 0.0f, 1.0f};
        // Дальнее основание (отдельные вершины с нормалями "вверх")
        const auto iBaseFar = (unsigned short)vertices.size();
        for (int iLong = 0; iLong < long_div; iLong++)
        {
            vertices.emplace_back();
            auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));
            v = dx::XMVectorAdd(v, offset);
            dx::XMStoreFloat3(&vertices.back().pos, v);
            vertices.back().n = {0.0f, 0.0f, 1.0f};
        }

        // Боковая поверхность
        //Ключевой момент: Для боковой поверхности создаются отдельные вершины с другими нормалями,
        //хотя их позиции совпадают с вершинами оснований!
        const auto iFusilage = (unsigned short)vertices.size();
        for (int iLong = 0; iLong < long_div; iLong++)
        {
            // Ближняя вершина боковой поверхности
            {
                vertices.emplace_back();
                auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));
                dx::XMStoreFloat3(&vertices.back().pos, v);
                vertices.back().n = {vertices.back().pos.x, vertices.back().pos.y, 0.0f}; // Нормаль наружу!
            }

            // Дальняя вершина боковой поверхности
            {
                vertices.emplace_back();
                auto v = dx::XMVector3Transform(base, dx::XMMatrixRotationZ(longitude_angle * iLong));
                v = dx::XMVectorAdd(v, offset);
                dx::XMStoreFloat3(&vertices.back().pos, v);
                vertices.back().n = {vertices.back().pos.x, vertices.back().pos.y, 0.0f};  // Нормаль наружу!
            }
        }

        // Создание индексов
        std::vector<unsigned short> indices;

        // Ближнее основание
        for (unsigned short iLong = 0; iLong < long_div; iLong++)
        {
            const auto i = iLong * 2;
            const auto mod = long_div * 2;
            // near
            indices.push_back(i + iBaseNear);               // Текущая вершина основания
            indices.push_back(iCenterNear);                     // Центр
            indices.push_back((i + 2) % mod + iBaseNear);   // Следующая вершина
        }
        // Дальнее основание (обратный порядок для правильной ориентации)
        for (unsigned short iLong = 0; iLong < long_div; iLong++)
        {
            const auto i = iLong * 2;
            const auto mod = long_div * 2;
            // far
            indices.push_back(iCenterFar);
            indices.push_back(i + 1 + iBaseFar);
            indices.push_back((i + 3) % mod + iBaseFar);
        }
        // Боковая поверхность (квады из двух треугольников):
        for (unsigned short iLong = 0; iLong < long_div; iLong++)
        {
            const auto i = iLong * 2;
            const auto mod = long_div * 2;

            // Два треугольника на одну грань
            indices.push_back(i + iFusilage);               // Ближняя текущая
            indices.push_back((i + 2) % mod + iFusilage);   // Ближняя следующая
            indices.push_back(i + 1 + iFusilage);           // Дальняя текущая

            indices.push_back((i + 2) % mod + iFusilage);   // Ближняя следующая
            indices.push_back((i + 3) % mod + iFusilage);   // Дальняя следующая
            indices.push_back(i + 1 + iFusilage);           // Дальняя текущая
        }

        return {std::move(vertices), std::move(indices)};
    }

    template <class V>
    static IndexedTriangleList<V> Make()
    {
        return MakeTesselated<V>(24);  // Призма с 24 гранями (почти цилиндр)
    }
};

#endif //__PRISM_H
