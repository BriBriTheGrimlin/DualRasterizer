#include "pch.h"
#include "Effect.h"
#include "Texture.h"


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile):
	m_pEffect { LoadEffect(pDevice, assetFile) }
{

	m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid\n";
	}
	//Matrices
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
	}

	m_pWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldVariable->IsValid())
	{
		std::wcout << L"m_pWorldVariable not valid!\n";
	}

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
	{
		std::wcout << L"m_pViewInverseVariable not valid!\n";
	}

	//Shading
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";
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


void Effect::ToggleTechniques()
{
	const Uint8* pStates{ SDL_GetKeyboardState(nullptr) };
		if (int(m_FilteringMethod) < 2) // < amount FilteringMethods - 1
			m_FilteringMethod = SampleMethod(int(m_FilteringMethod) + 1);
		else
			m_FilteringMethod = SampleMethod(0);


		switch (m_FilteringMethod)
		{
		case Effect::SampleMethod::Point:
			m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
			if (!m_pTechnique->IsValid())
				std::cout << "PointTechnique not valid\n";
			break;
		case SampleMethod::Linear:
			m_pTechnique = m_pEffect->GetTechniqueByName("LinearFilteringTechnique");
			if (!m_pTechnique->IsValid())
				std::cout << "LinearTechnique not valid\n";
			break;
		case SampleMethod::Anisotropic:
			m_pTechnique = m_pEffect->GetTechniqueByName("AnisotropicFilteringTechnique");
			if (!m_pTechnique->IsValid())
				std::cout << "AnisotropicTechnique not valid\n";
			break;
		default:
			break;
		}
}

void Effect::SetProjectionMatrix(const dae::Matrix& matrix)
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetWorldMatrix(const Matrix& matrix)
{
	m_pWorldVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetViewInvertMatrix(const Matrix& matrix)
{
	m_pViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique()
{
	return m_pTechnique;
}

ID3D11InputLayout* Effect::GetInputLayout()
{
	return m_pInputLayout;
}

int Effect::GetSampleState() const
{
	switch (m_FilteringMethod)
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

