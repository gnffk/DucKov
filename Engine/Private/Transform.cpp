#include "Transform.h"
#include "Shader.h"
Transform::Transform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Component{ pDevice, pContext }
{
}

Transform::~Transform()
{

}

_float3 Transform::Get_Scaled()
{
    return _float3(
        XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
        XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
        XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
    );
}

HRESULT Transform::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Transform::Initialize(void* pArg)
{
    // 여기서 구조체의 대한 정보 넘겨줌
    auto        pDesc = static_cast<TRANSFORM_DESC*>(pArg);

    m_fSpeedPerSec = pDesc->fSpeedPerSec;
    m_fRotationPerSec = pDesc->fRotationPerSec;

    Set_State(STATE::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
    Set_State(STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
    Set_State(STATE::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
    Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

    return S_OK;

}

void Transform::Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fScaleX);
    Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fScaleY);
    Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fScaleZ);
}

void Transform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * fScaleX);
    Set_State(STATE::UP, Get_State(STATE::UP) * fScaleY);
    Set_State(STATE::LOOK, Get_State(STATE::LOOK) * fScaleZ);
}

void Transform::Go_Straight(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vLook = Get_State(STATE::LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Backward(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vLook = Get_State(STATE::LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Left(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vRight = Get_State(STATE::RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Right(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vRight = Get_State(STATE::RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Up(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vUp = Get_State(STATE::UP);

    vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Down(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vDown = Get_State(STATE::UP);

    vPosition -= XMVector3Normalize(vDown) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Static_Right(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vRight = Get_State(STATE::RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

void Transform::Go_Static_Left(_float fTimeDelta)
{
    _vector         vPosition = Get_State(STATE::POSITION);
    _vector         vRight = Get_State(STATE::RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}


void Transform::Rotation(_fvector vAxis, _float fAngle)
{
    _float3     vScaled = Get_Scaled();

    _vector     vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector     vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector     vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

    _matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

    Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void Transform::Rotation(_float fAngleX, _float fAngleY, _float fAngleZ)
{
    _float3     vScaled = Get_Scaled();

    _vector     vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
    _vector     vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
    _vector     vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

    _matrix RotationMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(fAngleX), XMConvertToRadians(fAngleY), XMConvertToRadians(fAngleZ)));
    /*_matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));*/

    Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void Transform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _float3     vScaled = Get_Scaled();

    _vector     vRight = Get_State(STATE::RIGHT);
    _vector     vUp = Get_State(STATE::UP);
    _vector     vLook = Get_State(STATE::LOOK);

    _matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(m_fRotationPerSec) * fTimeDelta);

    Set_State(STATE::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE::UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}



void Transform::LookAt(_fvector vAt)
{
    _vector     vLook = vAt - Get_State(STATE::POSITION);
    _vector     vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector     vUp = XMVector3Cross(vLook, vRight);

    _float3     vScaled = Get_Scaled();

    Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
    Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void Transform::Chase(_fvector vGoal, _float fTimeDelta, _float fLimit)
{
    _vector     vPosition = Get_State(STATE::POSITION);
    _vector     vMoveDir = vGoal - vPosition;

    _float      fDistance = XMVectorGetX(XMVector3Length(vMoveDir));


    if (fDistance > fLimit)
        vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE::POSITION, vPosition);
}

shared_ptr<Transform> Transform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto		pInstance = shared_ptr<Transform>(new Transform(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Transform");
        return nullptr;
    }

    return pInstance;
}

HRESULT Transform::Bind_ShaderResource(shared_ptr<class Shader> pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

shared_ptr<Prototype> Transform::Clone(void* pArg)
{
    return nullptr;
}
