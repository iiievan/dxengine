#ifndef __MELON_H
#define __MELON_H

#include "DrawableBase.h"

class Melon : public DrawableBase<Melon>
{
public:
    Melon( Graphics& gfx,std::mt19937& rng,
      std::uniform_real_distribution<float>& adist,
      std::uniform_real_distribution<float>& ddist,
      std::uniform_real_distribution<float>& odist,
      std::uniform_real_distribution<float>& rdist,
      std::uniform_int_distribution<int>& longdist,
      std::uniform_int_distribution<int>& latdist );

    void Update( float dt ) noexcept override;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:

    // Позиционные параметры
    float m_r;              // Радиус орбиты - расстояние от центра вращения
    float m_roll {0.0f};    // Угол крена вокруг собственной оси Z
    float m_pitch {0.0f};   // Угол тангажа вокруг собственной оси X
    float m_yaw {0.0f};     // Угол рысканья вокруг собственной оси Y
    // Параметры вращения
    float m_theta;          // Угол орбитального вращения вокруг мировой оси Y
    float m_phi;            // Угол орбитального вращения вокруг мировой оси X
    float m_chi;            // Угол орбитального вращения вокруг мировой оси Z

    /*
    1. Тангаж (Pitch) - m_pitch
    Ось вращения: Поперечная ось (X) - проходит через крылья дрона
    Действие: Наклон вперед/назад
    Эффект: Дрон летит носом вверх или вниз
    2. Рысканье (Yaw) - m_yaw
    Ось вращения: Вертикальная ось (Y) - проходит сверху вниз через центр дрона
    Действие: Поворот вокруг вертикальной оси
    Эффект: Дрон поворачивает влево/вправо (меняет курс)
    3. Крен (Roll) - m_roll
    Ось вращения: Продольная ось (Z) - проходит от носа к хвосту
    Действие: Наклон влево/вправо
    Эффект: Дрон летит влево или вправо
     */

    // Скорости изменения позиции или вращения (радиан/секунду)
    float m_droll;      // Скорость вращения вокруг собственной оси Z (крен)
    float m_dpitch;     // Скорость вращения вокруг собственной оси X (тангаж)
    float m_dyaw;       // Скорость вращения вокруг собственной оси Y (рысканье)
    float m_dtheta;     // Скорость орбитального вращения вокруг мировой оси Y
    float m_dphi;       // Скорость орбитального вращения вокруг мировой оси X
    float m_dchi;       // Скорость орбитального вращения вокруг мировой оси Z
};



#endif //__MELON_H
