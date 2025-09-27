#include "Mesh.h"
#include "imgui.h"

namespace dx = DirectX;

Mesh::Mesh(Graphics &gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
{
    if (!IsStaticInitialized())
        AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    for (auto &pb : bindPtrs)
    {
        if (auto pi = dynamic_cast<Bind::IndexBuffer *>(pb.get()))
        {
            AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{pi});
            pb.release();
        }
        else
            AddBind(std::move(pb));
    }

    AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}

void Mesh::Draw(Graphics &gfx, DirectX::FXMMATRIX accumulatedTransform) const NOXND
{
    DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
    Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&m_transform);
}

Node::Node(const std::string &name, std::vector<Mesh *> meshPtrs, const DirectX::XMMATRIX &transform) NOXND
    : m_meshPtrs(std::move(meshPtrs)),
      m_Name(name)
{
    DirectX::XMStoreFloat4x4(&m_transform, transform);
}

void Node::Draw(Graphics &gfx, DirectX::FXMMATRIX accumulateTransform) const NOXND
{
    const auto built = DirectX::XMLoadFloat4x4(&m_transform) * accumulateTransform;

    // apply that transform to all the meshes of the node and all childrens of that node
    for (const auto pm : m_meshPtrs)
        pm->Draw(gfx, built);

    for (const auto &pc : m_childPtrs)
        pc->Draw(gfx, built);
}

void Node::ShowTree() const noexcept
{
    if (ImGui::TreeNode(m_Name.c_str()))
    {
        for (const auto &pChild : m_childPtrs)
            pChild->ShowTree();
        ImGui::TreePop();
    }
}

void Node::AddChild(std::unique_ptr<Node> pChild) NOXND
{
    assert(pChild != nullptr);
    m_childPtrs.push_back(std::move(pChild));
}

// Model
class ModelWindow
{
    public:
    void Show (const char* windowName, const Node &root) noexcept
    {
        // window name defaults to "Model"
        windowName = windowName ? windowName : "Model";
        if (ImGui::Begin(windowName))
        {
            ImGui::Columns(2,nullptr,true);
            root.ShowTree();

            ImGui::NextColumn();
            ImGui::Text("Orientation");
            ImGui::SliderAngle("Roll",&m_pos.roll, -180.0f, 180.0f);
            ImGui::SliderAngle("Pitch",&m_pos.pitch, -180.0f, 180.0f);
            ImGui::SliderAngle("Yaw",&m_pos.yaw, -180.0f, 180.0f);
            ImGui::Text("Position");
            ImGui::SliderFloat("X",&m_pos.x, -20.0f, 20.0f);
            ImGui::SliderFloat("Y",&m_pos.y, -20.0f, 20.0f);
            ImGui::SliderFloat("Z",&m_pos.z, -20.0f, 20.0f);
        }
        ImGui::End();
    }

    dx::XMMATRIX GetTransform() const noexcept
    {
        return dx::XMMatrixRotationRollPitchYaw(m_pos.roll, m_pos.pitch, m_pos.yaw) *
                dx::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    }

private:
    struct
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } m_pos;

};

Model::Model(Graphics &gfx, const std::string filename)
: m_pWindow(std::make_unique<ModelWindow>())
{
    Assimp::Importer imp;
    const auto       pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    for (size_t i = 0; i < pScene->mNumMeshes; i++)
        m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));

    m_pRoot = ParseNode(*pScene->mRootNode);
}

Model::~Model() noexcept
{

}

void Model::Draw(Graphics &gfx) const
{
    m_pRoot->Draw(gfx, m_pWindow->GetTransform());
}

void Model::ShowWindow(const char *windowName) noexcept
{
    m_pWindow->Show(windowName,*m_pRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics &gfx, const aiMesh &mesh)
{
    namespace dx = DirectX;
    using Dvtx::VertexLayout;

    Dvtx::VertexBuffer vbuf(
        std::move(
            VertexLayout{}.Append(VertexLayout::Position3D)
                          .Append(VertexLayout::Normal)));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
        vbuf.EmplaceBack(
            *reinterpret_cast<dx::XMFLOAT3 *>(&mesh.mVertices[i]),
            *reinterpret_cast<dx::XMFLOAT3 *>(&mesh.mNormals[i]));
    }

    std::vector<unsigned short> indices;
    indices.reserve(mesh.mNumFaces * 3);
    for (unsigned int i = 0; i < mesh.mNumFaces; i++)
    {
        const auto &face = mesh.mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

    bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));
    bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

    auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"shaders/Phong.vs.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"shaders/Phong.ps.cso"));
    bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().Get3DLayout(), pvsbc));

    struct PSMaterialConstant
    {
        dx::XMFLOAT3 color = {0.6f, 0.6f, 0.8f};
        float        specularIntensity = 0.6f;
        float        SpecularPower = 30.0f;
        float        padding[3];
    } pmc;
    bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

    return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode &node)
{
    namespace dx = DirectX;
    const auto transform = dx::XMMatrixTranspose(
        dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4 *>(&node.mTransformation)));

    std::vector<Mesh *> currMeshPtrs;
    currMeshPtrs.reserve(node.mNumMeshes);
    for (size_t i = 0; i < node.mNumMeshes; i++)
    {
        const auto meshIdx = node.mMeshes[i];
        currMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
    }

    auto pNode = std::make_unique<Node>(node.mName.C_Str(), std::move(currMeshPtrs), transform);
    for (size_t i = 0; i < node.mNumChildren; i++)
        pNode->AddChild(ParseNode(*node.mChildren[i]));

    return pNode;
}