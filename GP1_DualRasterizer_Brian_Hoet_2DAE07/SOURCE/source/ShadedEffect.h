#pragma once
#include "Effect.h"


using namespace dae;

class ShadedEffect final : public Effect
{
public:
	ShadedEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~ShadedEffect() override;

	ShadedEffect(const ShadedEffect& other) = delete;
	ShadedEffect& operator=(const ShadedEffect& other) = delete;
	ShadedEffect(ShadedEffect&& other) = delete;
	ShadedEffect& operator=(ShadedEffect&& other) = delete;

	void SetNormalMap(Texture* pNormalTexture);
	void SetSpecularMap(Texture* pSpecularTexture);
	void SetGlossinessMap(Texture* pGlossinessTexture);

	//virtual void SetWorldMatrix(const Matrix& matrix) override;
	//virtual void SetViewInvertMatrix(const Matrix& matrix) override;

private:
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};

};



