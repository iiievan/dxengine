#ifndef __MESH_H
#define __MESH_H

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "ConditionalNoexcept.h"
#include "bindable/BindableCommon.h"
#include "drawable/Drawable.h"

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
    friend class ModelWindow;

public:
    Node(int id, const std::string &name, std::vector<Mesh *> meshPtrs, const DirectX::XMMATRIX &transform_in) NOXND;
    void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulateTransform) const NOXND;
    void ShowTree(Node *&pSelectedNode) const noexcept;
    void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
    int  GetId() const noexcept { return m_ID; }

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
    void ShowWindow(const char *windowName = nullptr) noexcept;

private:
    static std::unique_ptr<Mesh> ParseMesh(Graphics &gfx, const aiMesh &mesh, const aiMaterial * const *pMaterials);
    std::unique_ptr<Node>        ParseNode(int &nextID, const aiNode &node);

    std::unique_ptr<Node>              m_pRoot;
    std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
    std::unique_ptr<class ModelWindow> m_pWindow;
};

#endif //__MESH_H