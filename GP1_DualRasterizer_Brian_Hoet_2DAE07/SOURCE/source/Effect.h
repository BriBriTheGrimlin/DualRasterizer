#pragma once
class Texture;

using namespace dae;

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	//Matrices transformations
	void SetProjectionMatrix(const dae::Matrix& matrix) const;
	void SetViewInvertMatrix(const dae::Matrix& matrix) const;
	void SetWorldMatrix(const dae::Matrix& matrix) const;
	//Shading
	void SetDiffuseMap(Texture* pDiffuseTexture) const;

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	ID3D11InputLayout* GetInputLayout() const;

	int GetSampleState() const;

	void ToggleTechniques();
	void ToggleCullMode();

protected:
	enum class SampleMethod
	{
		Point, 
		Linear,
		Anisotropic
	};
	SampleMethod m_SampleMethod;

	enum class CullMode
	{
		Front,
		Back,
		None
	};
	CullMode m_CullMode;


	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

	//Transform Matrices
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{};
	ID3DX11EffectMatrixVariable* m_pWorldVariable{};

	//Shading
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};

