#ifndef __MESH_H
#define __MESH_H

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "ConditionalNoexcept.h"
#include "Vertex.h"
#include "bindable/BindableCommon.h"
#include "drawable/DrawableBase.h"

class Mesh : public DrawableBase<Mesh>
{
public:
    Mesh(Graphics &gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);
    void              Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    mutable DirectX::XMFLOAT4X4 m_transform;
};

class Node
{
    friend class Model;

public:
    Node(std::vector<Mesh *> meshPtrs, const DirectX::XMMATRIX &transform) NOXND;
    void Draw(Graphics &gfx, DirectX::FXMMATRIX accumulateTransform) const NOXND;

private:
    void AddChild(std::unique_ptr<Node> pChild) NOXND;

    std::vector<std::unique_ptr<Node>> m_childPtrs;
    std::vector<Mesh*> m_meshPtrs;
    DirectX::XMFLOAT4X4 m_transform;
};

class Model
{
public:
    Model(Graphics& gfx, const std::string filename);
    void Draw(Graphics &gfx, DirectX::FXMMATRIX transform) const;

private:
    static std::unique_ptr<Mesh> ParseMesh(Graphics &gfx, const aiMesh &mesh);
    std::unique_ptr<Node> ParseNode(const aiNode &node);

    std::unique_ptr<Node> m_pRoot;
    std::vector<std::unique_ptr<Mesh>> m_meshPtrs;
};

#endif //__MESH_H