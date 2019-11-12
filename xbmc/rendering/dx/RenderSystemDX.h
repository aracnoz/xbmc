/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "DeviceResources.h"
#include "rendering/RenderSystem.h"
#include "threads/Condition.h"
#include "threads/CriticalSection.h"
#include "utils/Color.h"

#include <wrl/client.h>

enum PCI_Vendors
{
  PCIV_ATI    = 0x1002,
  PCIV_nVidia = 0x10DE,
  PCIV_Intel  = 0x8086
};

class ID3DResource;
class CGUIShaderDX;
enum AVPixelFormat;
enum AVPixelFormat;

class CRenderSystemDX : public CRenderSystemBase, DX::IDeviceNotify
{
public:
  CRenderSystemDX();
  virtual ~CRenderSystemDX();

  // CRenderBase overrides
  bool InitRenderSystem() override;
  bool DestroyRenderSystem() override;
  bool BeginRender() override;
  bool EndRender() override;
  void PresentRender(bool rendered, bool videoLayer) override;
  bool ClearBuffers(UTILS::Color color) override;
  void SetViewPort(const CRect& viewPort) override;
  void GetViewPort(CRect& viewPort) override;
  void RestoreViewPort() override;
  CRect ClipRectToScissorRect(const CRect &rect) override;
  bool ScissorsCanEffectClipping() override;
  void SetScissors(const CRect &rect) override;
  void ResetScissors() override;
  void CaptureStateBlock() override;
  void ApplyStateBlock() override;
  void SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor = 0.f) override;
  void SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view) override;
  bool SupportsStereo(RENDER_STEREO_MODE mode) const override;
  void Project(float &x, float &y, float &z) override;
  bool SupportsNPOT(bool dxt) const override;

/* KRYPTON
  IDXGIOutput* GetCurrentOutput() const { return m_pOutput; }
  void GetDisplayMode(DXGI_MODE_DESC *mode, bool useCached = false);
  void FinishCommandList(bool bExecute = true) const;
  void FlushGPU() const;
  void RequestDecodingTime();
  void ReleaseDecodingTime();

  ID3D11Device*           Get3D11Device() const       { return m_pD3DDev; }
  ID3D11DeviceContext*    Get3D11Context() const      { return m_pContext; }
  ID3D11DeviceContext*    GetImmediateContext() const { return m_pImdContext; }
  CGUIShaderDX*           GetGUIShader() const        { return m_pGUIShader; }
  unsigned                GetFeatureLevel() const     { return m_featureLevel; }
  D3D11_USAGE             DefaultD3DUsage() const     { return m_defaultD3DUsage; }
  DXGI_ADAPTER_DESC       GetAIdentifier() const      { return m_adapterDesc; }
  bool                    Interlaced() const          { return m_interlaced; }
  int                     GetBackbufferCount() const  { return 2; }
  void                    SetAlphaBlendEnable(bool enable); */
#ifdef HAS_DS_PLAYER
  void                    SetWindowedForMadvr();
  void                    GetParamsForDSPlayer(bool &useWindowedDX, unsigned int &nBackBufferWidth, unsigned int &nBackBufferHeight, bool &bVSync, float &refreshRate, bool &interlaced);
  bool                    m_UseWindowedDX_DSPlayer;
#endif


  // IDeviceNotify overrides
  void OnDXDeviceLost() override;
  void OnDXDeviceRestored() override;

  // CRenderSystemDX methods
  CGUIShaderDX* GetGUIShader() const { return m_pGUIShader; }
  bool Interlaced() const { return m_interlaced; }
  bool IsFormatSupport(DXGI_FORMAT format, unsigned int usage) const;
  CRect GetBackBufferRect();
  CD3DTexture& GetBackBuffer();

  void FlushGPU() const;
  void RequestDecodingTime();
  void ReleaseDecodingTime();
  void SetAlphaBlendEnable(bool enable);

  // empty overrides
  bool IsExtSupported(const char* extension) const override { return false; };
  bool ResetRenderSystem(int width, int height) override { return true; };

protected:
  virtual void PresentRenderImpl(bool rendered) = 0;

  bool CreateStates();
  bool InitGUIShader();
  void OnResize();
  void CheckInterlacedStereoView(void);
  void CheckDeviceCaps();

  CCriticalSection m_resourceSection;
  CCriticalSection m_decoderSection;

  // our adapter could change as we go
  bool m_interlaced;
  bool m_inScene{ false }; ///< True if we're in a BeginScene()/EndScene() block
  bool m_BlendEnabled{ false };
  bool m_ScissorsEnabled{ false };
  D3D11_VIEWPORT m_viewPort;
  CRect m_scissor;
  CGUIShaderDX* m_pGUIShader{ nullptr };
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
  Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendEnableState;
  Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendDisableState;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RSScissorDisable;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RSScissorEnable;
  // stereo interlaced/checkerboard intermediate target
  CD3DTexture m_rightEyeTex;

  XbmcThreads::EndTime m_decodingTimer;
  XbmcThreads::ConditionVariable m_decodingEvent;

  std::shared_ptr<DX::DeviceResources> m_deviceResources;
};
