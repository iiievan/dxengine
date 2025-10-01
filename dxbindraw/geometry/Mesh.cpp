#include "Mesh.h"
#include "imgui.h"
#include <unordered_map>
#include <sstream>

#include "Surface.h"

namespace dx = DirectX;


ModelException::ModelException(int line, const char *file, std::string note) noexcept
: ChiliException(line, file),
  m_note(std::move(note))
{
}

const char * ModelException::what() const noexcept
{
    std::ostringstream oss;
    oss << ChiliException::what() << std::endl
        << "[Note]" << GetNote();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char * ModelException::GetType() const noexcept
{
    return "Chili Model Exception";
}

const std::string & ModelException::GetNote() const noexcept
{
    return m_note;
}

Mesh::Mesh(Graphics &gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{
    AddBind(std::make_shared<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    for (auto &pb : bindPtrs)
    {
        AddBind(std::move(pb));
    }

    AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
}

void Mesh::Draw(Graphics &gfx, dx::FXMMATRIX accumulatedTransform) const NOXND
{
    dx::XMStoreFloat4x4(&m_transform, accumulatedTransform);
    Drawable::Draw(gfx);
}

dx::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return dx::XMLoadFloat4x4(&m_transform);
}

Node::Node(int id, const std::string &name, std::vector<Mesh *> meshPtrs, const dx::XMMATRIX &transform_in) NOXND
    : m_meshPtrs(std::move(meshPtrs)),
      m_Name(name),
      m_ID(id)
{
    dx::XMStoreFloat4x4(&m_transform, transform_in);
    dx::XMStoreFloat4x4(&m_appliedTransform, dx::XMMatrixIdentity());
}

void Node::Draw(Graphics &gfx, dx::FXMMATRIX accumulateTransform) const NOXND
{
    const auto built = dx::XMLoadFloat4x4(&m_appliedTransform) *
                                 dx::XMLoadFloat4x4(&m_transform) *
                                 accumulateTransform;

    // apply that transform to all the meshes of the node and all childrens of that node
    for (const auto pm : m_meshPtrs)
        pm->Draw(gfx, built);

    for (const auto &pc : m_childPtrs)
        pc->Draw(gfx, built);
}

void Node::ShowTree( Node*& pSelectedNode) const noexcept
{
    // if there is no selected node, set selected ID to an impossible value
    const int selectedID = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
    // build up flags for current node
    // value_or : Если optional содержит значение → возвращает это значение
    //            Если optional пустой(не содержит значения) → возвращает default_value
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow | //  узел раскрывается только по клику на стрелку
                               ((GetId() == selectedID) ? ImGuiTreeNodeFlags_Selected : 0) | // узел визуально выделен, если его индекс совпадает с выбранным
                                ((m_childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0); // узел помечается как лист, если у него нет детей
    /*
     Примеры для std::optional .value_or()
    1-й пример:
    Ни один узел еще не выбран
    selectedIndex - пустой (std::nullopt)
    value_or(-1) вернет -1
    Сравнение: currentNodeIndex == -1 → всегда false (индексы начинаются с 0)
    Результат: Ни один узел не будет выделен
    -1 выбран как "невозможный" индекс, поскольку индексы узлов начинаются с 0 и увеличиваются

    2-й пример:
    Выбран узел с индексом 3
    selectedIndex содержит значение 3
    value_or(-1) вернет 3
    Сравнение: currentNodeIndex == 3 → true только для узла с индексом 3
    Результат: Только узел с индексом 3 получит флаг Selected
    */

    // Создание элемента дерева в ImGui
    // TreeNodeEx возвращает true, если узел раскрыт
    // (void *)(intptr_t)currentNodeIndex - уникальный ID для ImGui
    const auto expanded = ImGui::TreeNodeEx((void *)(intptr_t)GetId(), node_flags,m_Name.c_str());

    // Обработка клика на узле
    if (ImGui::IsItemClicked())
    {
        pSelectedNode = const_cast<Node *>(this);   // И указатель на узел
                                                    // const_cast нужен потому что метод const, но выбор требует модификации
    }

    if (expanded)
    {
        // Рекурсивный обход дочерних узлов
        // Два идентификатора выбора (индекс и указатель) дают гибкость
        for (const auto &pChild : m_childPtrs)
            pChild->ShowTree(pSelectedNode);
        ImGui::TreePop();
    }
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
    dx::XMStoreFloat4x4(&m_appliedTransform, transform);
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

        // need an ints to track node indices and select node
        if (ImGui::Begin(windowName))
        {
            int nodeIndexTracker = 0;
            ImGui::Columns(2,nullptr,true);
            root.ShowTree(m_selectedNode);

            ImGui::NextColumn();
            if (m_selectedNode != nullptr)
            {
                // если структура трансформации не существует для этого узла
                // то она будет создана именно в этот момент благодаря unordered_map<>
                auto& transform = m_transforms[m_selectedNode->GetId()];
                ImGui::Text("Orientation");
                ImGui::SliderAngle("Roll",&transform.roll, -180.0f, 180.0f);
                ImGui::SliderAngle("Pitch",&transform.pitch, -180.0f, 180.0f);
                ImGui::SliderAngle("Yaw",&transform.yaw, -180.0f, 180.0f);
                ImGui::Text("Position");
                ImGui::SliderFloat("X",&transform.x, -20.0f, 20.0f);
                ImGui::SliderFloat("Y",&transform.y, -20.0f, 20.0f);
                ImGui::SliderFloat("Z",&transform.z, -20.0f, 20.0f);
            }
        }
        ImGui::End();
    }

    dx::XMMATRIX GetTransform() const noexcept
    {
        assert(m_selectedNode != nullptr);
        const auto& transform = m_transforms.at(m_selectedNode->GetId());
        return dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
                dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
    }

    Node* GetSelectedNode() const noexcept { return m_selectedNode; }

private:
    struct m_TransformParameters
    {
        float roll = 0.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    //  Почему unordered_map? Быстрый поиск по ключу - O(1) в среднем случае
    //                        Динамическое добавление - новые трансформации создаются по мере выбора узлов
    //                        Экономия памяти - хранятся только трансформации для узлов, которые когда-либо выбирались
    std::unordered_map<int, m_TransformParameters> m_transforms;
    Node *                                         m_selectedNode;
};

Model::Model(Graphics &gfx, const std::string filename)
    : m_pWindow(std::make_unique<ModelWindow>())
{
    Assimp::Importer imp;
    const auto       pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate |
                                                                   aiProcess_JoinIdenticalVertices |
                                                                   aiProcess_ConvertToLeftHanded |
                                                                   aiProcess_GenNormals);

    if (pScene == nullptr)
        throw ModelException(__LINE__, __FILE__, imp.GetErrorString());

    for (size_t i = 0; i < pScene->mNumMeshes; i++)
        m_meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));

    int nextID = 0;
    m_pRoot = ParseNode(nextID,*pScene->mRootNode);
}

Model::~Model() noexcept
{

}

void Model::Draw(Graphics &gfx) const
{
    if (auto node = m_pWindow->GetSelectedNode())
        node->SetAppliedTransform(m_pWindow->GetTransform());

    m_pRoot->Draw(gfx, dx::XMMatrixIdentity());
}

void Model::ShowWindow(const char *windowName) noexcept
{
    m_pWindow->Show(windowName,*m_pRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics &gfx, const aiMesh &mesh,  const aiMaterial * const *pMaterials)
{
    using Dvtx::VertexLayout;

    Dvtx::VertexBuffer vbuf(
        std::move(
            VertexLayout{}.Append(VertexLayout::Position3D)
                          .Append(VertexLayout::Normal)
                          .Append(VertexLayout::Texture2D)));

    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
        vbuf.EmplaceBack(
            *reinterpret_cast<dx::XMFLOAT3 *>(&mesh.mVertices[i]),
            *reinterpret_cast<dx::XMFLOAT3 *>(&mesh.mNormals[i]),
            *reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
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

    std::vector<std::shared_ptr<Bind::Bindable>> bindablePtrs;

    bool hasSpecularMap = false;
    float shininess = 35.0f;
    if (mesh.mMaterialIndex >= 0)
    {
        auto &material = *pMaterials[mesh.mMaterialIndex];

        using namespace std::string_literals;
        const auto base = "models\\nano_textured\\"s;
        aiString textFilename;

        material.GetTexture(aiTextureType_DIFFUSE, 0, &textFilename);
        bindablePtrs.push_back(std::make_shared<Bind::Texture>(gfx,base + textFilename.C_Str()));

        if (material.GetTexture(aiTextureType_SPECULAR, 0, &textFilename) == aiReturn_SUCCESS)
        {
            bindablePtrs.push_back(std::make_shared<Bind::Texture>(gfx,base + textFilename.C_Str(), 1));
            hasSpecularMap = true;
        }
        else
            material.Get(AI_MATKEY_SHININESS, shininess);

        bindablePtrs.push_back(std::make_shared<Bind::Sampler>(gfx));
    }

    bindablePtrs.push_back(std::make_shared<Bind::VertexBuffer>(gfx, vbuf));
    bindablePtrs.push_back(std::make_shared<Bind::IndexBuffer>(gfx, indices));

    auto pvs = std::make_shared<Bind::VertexShader>(gfx, "shaders/Phong.vs.cso");
    auto pvsbc = pvs->GetBytecode();
    bindablePtrs.push_back(std::move(pvs));

    bindablePtrs.push_back(std::make_shared<Bind::InputLayout>(gfx, vbuf.GetLayout(), pvsbc));

    if (hasSpecularMap)
    {
        bindablePtrs.push_back(std::make_shared<Bind::PixelShader>(gfx, "shaders/PhongSpecMap.ps.cso"));
    }
    else
    {
        bindablePtrs.push_back(std::make_shared<Bind::PixelShader>(gfx, "shaders/Phong.ps.cso"));
        struct PSMaterialConstant
        {
            float        specularIntensity = 0.8f;
            float        SpecularPower;
            float        padding[2];
        } pmc;
        pmc.SpecularPower = shininess;
        bindablePtrs.push_back(std::make_shared<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
    }
    return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int &nextID, const aiNode &node)
{
    const auto transform = dx::XMMatrixTranspose(
        dx::XMLoadFloat4x4(reinterpret_cast<const dx::XMFLOAT4X4 *>(&node.mTransformation)));

    std::vector<Mesh *> currMeshPtrs;
    currMeshPtrs.reserve(node.mNumMeshes);
    for (size_t i = 0; i < node.mNumMeshes; i++)
    {
        const auto meshIdx = node.mMeshes[i];
        currMeshPtrs.push_back(m_meshPtrs.at(meshIdx).get());
    }

    auto pNode = std::make_unique<Node>(nextID++, node.mName.C_Str(), std::move(currMeshPtrs), transform);
    for (size_t i = 0; i < node.mNumChildren; i++)
        pNode->AddChild(ParseNode(nextID, *node.mChildren[i]));

    return pNode;
}