#pragma once
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;
class MeshRepresentation;
class Texture;
struct Vertex_Out;
struct MeshRast;

using namespace dae;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render();

		void ToggleLightMode();
		void ToggleMode();
		void ToggleRot();
		void ToggleFireMesh();
		void ToggleSampling() const;
		void ToggleNor();
		void ToggleBuffer();
		void ToggleBoxVisual();
		void ToggleBackGround();

	private:
		
		bool m_UniformBackGround{ false };
		bool m_DirectXMode{ true };
		bool m_RotEnabled{ true };
		float m_Angle{};

		Vector3 Pos{ 0,0,50.f };
		
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		Camera m_Camera;

		//DIRECTX
		bool m_FireMeshEnabled{ true };

		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		std::vector<MeshRepresentation*> m_pMeshRepresentation;
		MeshRepresentation* m_pFireMesh;

		void RenderDirectX() const;
		void UpdateDirectX(const Timer* pTimer);

		//Rasterizer
		bool m_NorEnabled{ true };
		bool m_VisBuffer{ false };
		bool m_VisBox{ false };

		enum class LightMode
		{
			Combined,
			Diffuse,
			Specular,
			ObservedArea

		};

		LightMode m_CurrentLightmode{ LightMode::Combined };

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};
		std::vector<MeshRast> m_pMeshesRast;

		float* m_pDepthBufferPixels{};

		Texture* m_pDiffuseTxt;
		Texture* m_pNormalTxt;
		Texture* m_pSpecularTxt;
		Texture* m_pGlossTxt;

		void RenderRasterizer(); 
		void UpdateRasterizer(const Timer* pTimer);

		ColorRGB PixelShading(const Vertex_Out& v) const;
		void VertexTransformationFunctionW4(std::vector<MeshRast>& meshes) const;

		

	};
