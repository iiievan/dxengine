#ifndef __PLANE_H
#define __PLANE_H

#include <optional>
#include <array>
#include "Vertex.h"
#include "DirectXMath.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"

class Plane
{
public:
	static IndexedTriangleList MakeTesselatedTextured(Dvtx::VertexLayout layout, int divisions_x, int divisions_y)
	{
        namespace dx = DirectX;

        assert(divisions_x >= 1);
        assert(divisions_y >= 1);

        constexpr float width = 1.0f;
        constexpr float height = 1.0f;
        const int       nVertices_x = divisions_x + 1;
        const int       nVertices_y = divisions_y + 1;
        Dvtx::VertexBuffer vb{std::move(layout)};

        {
            const float divisionSize_x = width / float(divisions_x);
            const float divisionSize_y = height / float(divisions_y);
            const float divisionSize_x_uv = 1.0f/float(divisions_x);
            const float divisionSize_y_uv = 1.0f/float(divisions_y);

            for (int y = 0, i = 0; y < nVertices_y; y++)
            {
                const float y_pos = float(y) * divisionSize_y - 1.0f;
                const float y_pos_uv = 1.0f - float(y) * divisionSize_y_uv;
                for (int x = 0; x < nVertices_x; x++, i++)
                {
                    const float x_pos = float(x) * divisionSize_x - 1.0f;
                    const float x_pos_uv = float(x) * divisionSize_x_uv;

                    vb.EmplaceBack(dx::XMFLOAT3{x_pos, y_pos,0.0f},
                                   dx::XMFLOAT3{0.0f, 0.0f, -1.0f},
                                   dx::XMFLOAT2{x_pos_uv, y_pos_uv});
                }
            }
        }

        std::vector<unsigned short> indices;
        indices.reserve(sq(divisions_x * divisions_y) * 6);
	    //int divisions_x = 10;
	    //int divisions_y = 10;
	    //int cells_count = divisions_x * divisions_y; // 100 ячеек
	    //int indices_needed = cells_count * 6;        // 600 индексов <- this wy we need sqare root of cells_count
		{
		    // Эта лямбда функция Преобразует 2D координаты (x,y) в линейный индекс вершины
			const auto vxy2i = [nVertices_x]( size_t x,size_t y )
			{
				return (unsigned short)(y * nVertices_x + x);
			};

		    // разбиваем полученные ячейки плоскости на треугольники и индексируем их
			for(size_t y = 0; y < divisions_y; y++)
			{
				for(size_t x = 0; x < divisions_x; x++)
				{
				    // Создание quad'а из 4 вершин
					const std::array<unsigned short,4> indexArray =
					{
					    vxy2i( x,y ),           // левый-верхний
					    vxy2i( x + 1,y ),       // правый-верхний
					    vxy2i( x,y + 1 ),       // левый-нижний
					    vxy2i( x + 1,y + 1 )    // правый-нижний
					};

				    // Разбиение quad'а на 2 треугольника
					indices.push_back(indexArray[0]);   // Вершины: 0 ->
					indices.push_back(indexArray[2]);   // 2 ->
					indices.push_back(indexArray[1]);   // 1 :   Диагональ идет из левого-нижнего в правый-верхний
					indices.push_back(indexArray[1]);   //  Вершины: 1 →
					indices.push_back(indexArray[2]);   // 2 →
					indices.push_back(indexArray[3]);   // 3 : Завершает quad

                    // Вершины:          Индексы для ячейки (x,y):
                    // 0──1──2──         Quad состоит из вершин:
                    // │  │  │           [0] = (x,y)     = A
                    // 3──4──5──         [1] = (x+1,y)   = B
                    // │  │  │           [2] = (x,y+1)   = C
                    // 6──7──8──         [3] = (x+1,y+1) = D
                    //
                    // Треугольник 1: A → C → B   (A─C─B)
                    // Треугольник 2: B → C → D   (B─C─D)
				}
			}
		}

		return{ std::move( vb ),std::move( indices ) };
	}

	static IndexedTriangleList Make()
	{
	    using Dvtx:: VertexLayout;
	    VertexLayout vl;
	    vl.Append(VertexLayout::Position3D);
	    vl.Append(VertexLayout::Normal);
	    vl.Append(VertexLayout::Texture2D);

		return MakeTesselatedTextured(std::move(vl), 1,1 );
	}
};

#endif //__PLANE_H
