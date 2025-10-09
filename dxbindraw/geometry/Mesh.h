#ifndef __MESH_H
#define __MESH_H

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "ConditionalNoexcept.h"
#include "bindable/BindableCommon.h"
#include "drawable/Drawable.h"
#include "bindable/ConstantBuffer.h"
#include "type_traits"
#include "imgui.h"

class ModelException : public ChiliException
{
public:
    ModelException(int line, const char* file, std::string note) noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
    const std::string &GetNote() const noexcept;

private:
    std::string m_note;
};

class Mesh : public Drawable
{
public:
    Mesh(Graphics &gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
    void              Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 m_transform;
};

class Node
{
    friend class Model;
public:
    struct PSMaterialConstantFullmonte
    {
        BOOL normalMapEnabled = TRUE;
        BOOL specularMapEnabled = TRUE;
        BOOL hasGlossMap = FALSE;
        float specularPower = 20.0f;
        DirectX::XMFLOAT3 specularColor = {0.75f, 0.75f, 0.75f};
        float specularMapWeight = 0.671f;
    };

    struct PSMaterialConstantNoTex
    {
        DirectX::XMFLOAT4 materialColor = {0.447970f,0.327254f,0.176283f,1.0f};
        DirectX::XMFLOAT4 specularColor = {0.65f,0.65f,0.65f,1.0f};
        float specularPower = 120.0f;
        float padding[3];
    };

public:
    Node(int id, const std::string &name, std::vector<Mesh *> meshPtrs, const DirectX::XMMATRIX &transform_in) NOXND;
    void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulateTransform) const NOXND;
    void ShowTree(Node *&pSelectedNode) const noexcept;
    void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
    int  GetId() const noexcept { return m_ID; }

    template<class T>
    bool ControlMeDaddy(Graphics &gfx, T &c)
    {
        if (m_meshPtrs.empty())
            return false;

        if constexpr(std::is_same<T,PSMaterialConstantFullmonte>::value)
        {
            if (auto pcb = m_meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
            {
                ImGui::Text("Material");

                bool normalMapEnabled = (bool)c.normalMapEnabled;
                ImGui::Checkbox("Normal Map", &normalMapEnabled);
                c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

                bool specularMapEnabled = (bool)c.specularMapEnabled;
                ImGui::Checkbox("Specular Map", &specularMapEnabled);
                c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

                bool hasGlossMap = (bool)c.hasGlossMap;
                ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
                c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

                ImGui::SliderFloat("Spec weight", &c.specularMapWeight, 0.0f, 2.0f);
                ImGui::SliderFloat("Spec pow", &c.specularPower, 0.0f, 1000.0f, "%f",5.0f);
                ImGui::ColorPicker3("Spec color", reinterpret_cast<float*>(&c.specularColor));
                pcb->Update(gfx, c);
                return true;
            }
        }
        else
        if constexpr(std::is_same<T,PSMaterialConstantNoTex>::value)
        {
            if (auto pcb = m_meshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
            {
                ImGui::Text("Material");
                ImGui::ColorPicker3("Spec Color.", reinterpret_cast<float*>(&c.specularColor));
                ImGui::SliderFloat("Spec pow", &c.specularPower, 0.0f, 1000.0f, "%f",5.0f);
                ImGui::ColorPicker3("Diff color", reinterpret_cast<float*>(&c.materialColor));
                pcb->Update(gfx, c);
                return true;
            }
        }
        return false;
    }

private:
    void AddChild(std::unique_ptr<Node> pChild) NOXND;

    std::string                        m_Name;
    int                                m_ID;
    std::vector<std::unique_ptr<Node>> m_childPtrs;
    std::vector<Mesh *>                m_meshPtrs;
    DirectX::XMFLOAT4X4                m_transform;
    DirectX::XMFLOAT4X4                m_appliedTransform;
};

class Model
{
public:
    Model(Graphics &gfx, const std::string filename);
    ~Model() noexcept;

    void Draw(Graphics &gfx) const NOXND;
    void ShowWindow(Graphics &gfx, const char *windowName = nullptr) noexcept;
    void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;

private:
    static std::unique_ptr<Mesh> ParseMesh(Graphics &gfx, const aiMesh &mesh, const aiMaterial * const *pMaterials);
    std::unique_ptr<Node>        ParseNode(int &nextID, const aiNode &node);

    std::unique_ptr<Node>              m_pRoot;
    std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
    std::unique_ptr<class ModelWindow> m_pWindow;
};

#endif //__MESH_H