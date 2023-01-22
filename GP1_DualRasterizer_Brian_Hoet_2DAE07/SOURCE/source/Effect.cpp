#include "pch.h"
#include "Effect.h"
#include "Texture.h"


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
	m_pEffect{ LoadEffect(pDevice, assetFile) },
	m_pDevice{ pDevice },
	m_CullMode{ CullMode::None }
{

	m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::cout << "Technique not valid\n";
	}
	//Matrices
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::cout << "m_pMatWorldViewProjVariable not valid\n";
	}

	m_pWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldVariable->IsValid())
	{
		std::cout << "m_pWorldVariable not valid!\n";
	}

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
	{
		std::cout << "m_pViewInverseVariable not valid!\n";
	}

	//Shading
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::cout << "m_pDiffuseMapVariable not valid!\n";
	}

	m_pRasterizerStateVariable = m_pEffect->GetVariableByName("gRasterizerState")->AsRasterizer();
	if (!m_pRasterizerStateVariable->IsValid())
	{
		std::cout << "m_pRasterizerVariable not valid!\n";
	}
}

Effect::~Effect()
{
	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
}


void Effect::ToggleSampling()
{
		switch (m_SampleMethod)
		{
		case SampleMethod::Point:
			m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
			m_SampleMethod = SampleMethod::Linear;
			if (!m_pTechnique->IsValid())
				std::cout << "PointTechnique not valid\n";
			break;
		case SampleMethod::Linear:
			m_pTechnique = m_pEffect->GetTechniqueByName("LinearFilteringTechnique");
			m_SampleMethod = SampleMethod::Anisotropic;
			if (!m_pTechnique->IsValid())
				std::cout << "LinearTechnique not valid\n";
			break;
		case SampleMethod::Anisotropic:
			m_pTechnique = m_pEffect->GetTechniqueByName("AnisotropicFilteringTechnique");
			m_SampleMethod = SampleMethod::Point;
			if (!m_pTechnique->IsValid())
				std::cout << "AnisotropicTechnique not valid\n";
			break;
		default:
			break;
		}
}

void Effect::ToggleCullMode()
{
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	switch (m_CullMode)
	{
		case CullMode::Front:
		{
			rasterizerDesc.CullMode = D3D11_CULL_FRONT;
			m_CullMode = CullMode::Back;
			break;
		}
		case CullMode::Back:
		{
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			m_CullMode = CullMode::None;
			break;
		}
		case CullMode::None:
		{
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			m_CullMode = CullMode::Front;
			break;
		}
	}

	HRESULT hr{ m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState) };
	hr = m_pRasterizerStateVariable->SetRasterizerState(0, m_pRasterizerState);
}

void Effect::SetProjectionMatrix(const dae::Matrix& matrix) const
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetWorldMatrix(const Matrix& matrix) const
{
	m_pWorldVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetViewInvertMatrix(const Matrix& matrix) const 
{
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture) const
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pTechnique;
}

ID3D11InputLayout* Effect::GetInputLayout() const
{
	return m_pInputLayout;
}

int Effect::GetSampleState() const
{
	switch (m_SampleMethod)
	{
	case SampleMethod::Point:
		return 0;
		break;
	case SampleMethod::Linear:
		return 1;
		break;
	case SampleMethod::Anisotropic:
		return 2;
		break;
	default:
		return 4;
		break;
	}
}

int Effect::GetCullMode() const
{
	switch (m_CullMode)
	{
	case CullMode::Front:
		return 0;
		break;
	case CullMode::Back:
		return 1;
		break;
	case CullMode::None:
		return 2;
		break;
	default:
		return 4;
		break;
	}
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}

