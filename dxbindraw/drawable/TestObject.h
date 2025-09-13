#ifndef __TESTOBJECT_H
#define __TESTOBJECT_H

#include "DrawableBase.h"

template<class T>
class TestObject : public DrawableBase<T>
{
public:
    TestObject(
        Graphics                              &gfx,
        std::mt19937                          &rng,
        std::uniform_real_distribution<float> &adist,
        std::uniform_real_distribution<float> &ddist,
        std::uniform_real_distribution<float> &odist,
        std::uniform_real_distribution<float> &rdist)
        : m_r(rdist(rng)),
          m_droll(ddist(rng)),
          m_dpitch(ddist(rng)),
          m_dyaw(ddist(rng)),
          m_dphi(odist(rng)),
          m_dtheta(odist(rng)),
          m_dchi(odist(rng)),
          m_chi(adist(rng)),
          m_theta(adist(rng)),
          m_phi(adist(rng))
    {  }

    void Update(float dt) noexcept
    {
        m_roll += m_droll * dt;
        m_pitch += m_dpitch * dt;
        m_yaw += m_dyaw * dt;
        m_theta += m_dtheta * dt;
        m_phi += m_dphi * dt;
        m_chi += m_dchi * dt;
    }

    DirectX::XMMATRIX GetTransformXM() const noexcept
    {
        {
            namespace dx = DirectX;
            return  dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *            // 3. Локальное вращение
                   // pitch (Тангаж) = наклон вперед/назад
                   // yaw (Рысканье) = поворот вокруг вертикали
                   // roll (Крен) = наклон влево/вправо
                   dx::XMMatrixTranslation(m_r, 0.0f, 0.0f) *       // 2. Смещение от центра
                   // m_r = расстояние от центральной точки
                   dx::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi);     // 1. Орбитальное вращение
            // theta = вращение вокруг оси Y (как рысканье вокруг точки)
            // phi = вращение вокруг оси X (как тангаж вокруг точки)
            // chi = вращение вокруг оси Y (как крен вокруг точки)

            /*
            1. Назначение: Орбитальное вращение вокруг точки (0,0,20)
               Эффект: Объект вращается вокруг смещенного центра
            2. Назначение: Радиальное смещение от центра орбиты
               Эффект: Определяет расстояние от центра вращения до объекта
            3. Назначение: Локальное вращение объекта вокруг собственного центра
               Эффект: Объект вращается вокруг своей оси

            Представьте себе солнечную систему:
            Translation(0,0,20) - Солнце в точке (0,0,20)
            Rotation(m_theta, m_phi, m_chi) - Земля вращается вокруг Солнца
            Translation(m_r,0,0) - Расстояние от Солнца до Земли
            Rotation(m_pitch, m_yaw, m_roll) - Земля вращается вокруг своей оси
             */
        }
    }

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

#endif //__TESTOBJECT_H
