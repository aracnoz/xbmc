/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://xbmc.org
 *
 *      Copyright (C) 2014-2015 Aracnoz
 *      http://github.com/aracnoz/xbmc
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#include "MadvrSharedRender.h"
#include "mvrInterfaces.h"
#include "Application.h"
#include "Utils/Log.h"
#include "guilib/GUIWindowManager.h"
#include "windowing/WindowingFactory.h"
#include "cores/VideoRenderers/RenderManager.h"
#include "settings/AdvancedSettings.h"

const DWORD D3DFVF_VID_FRAME_VERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;

struct VID_FRAME_VERTEX
{
  float x;
  float y;
  float z;
  float rhw;
  float u;
  float v;
};

CMadvrSharedRender::CMadvrSharedRender()
{
  color_t clearColour = (g_advancedSettings.m_videoBlackBarColour & 0xff) * 0x010101;
  CD3DHelper::XMStoreColor(m_fColor, clearColour);
  m_bUnderRender = false;
  m_bGuiVisible = false;
  m_bGuiVisibleOver = false;
}

CMadvrSharedRender::~CMadvrSharedRender()
{
  SAFE_RELEASE(m_pMadvrVertexBuffer);
  SAFE_RELEASE(m_pMadvrOverTexture);
  SAFE_RELEASE(m_pKodiOverSurface);
  SAFE_RELEASE(m_pKodiOverTexture);

  SAFE_RELEASE(m_pUnderTexture9);
  SAFE_RELEASE(m_pUnderSurface11);
  SAFE_RELEASE(m_pUnderTexture11);
  
  SAFE_RELEASE(m_pD3D11Queue);
  SAFE_RELEASE(m_pD3D9Queue);
  SAFE_RELEASE(m_pD3D11Producer);
  SAFE_RELEASE(m_pD3D11Consumer);
  SAFE_RELEASE(m_pD3D9Producer);
  SAFE_RELEASE(m_pD3D9Consumer);
}

HRESULT CMadvrSharedRender::CreateSharedResource(IDirect3DTexture9** ppTexture9, ID3D11Texture2D** ppTexture11, ID3D11RenderTargetView** ppSurface11)
{
  HRESULT hr;
  HANDLE pSharedHandle = nullptr;
  CD3D11_RENDER_TARGET_VIEW_DESC rtDesc(D3D11_RTV_DIMENSION_TEXTURE2D);

  if (FAILED(hr = m_pD3DDeviceMadVR->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, ppTexture9, &pSharedHandle)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceKodi->OpenSharedResource(pSharedHandle, __uuidof(ID3D11Texture2D), (void**)(ppTexture11))))
    return hr;

  if (FAILED(hr = m_pD3DDeviceKodi->CreateRenderTargetView(*ppTexture11, &rtDesc, ppSurface11)))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::CreateSharedQueueResource()
{
  HRESULT hr;

  // Initialize the surface queues
  SURFACE_QUEUE_DESC  desc;
  desc.Width = m_dwWidth;
  desc.Height = m_dwHeight;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.NumSurfaces = 1;
  desc.MetaDataSize = sizeof(UINT);
  desc.Flags = 0;

  if (FAILED(hr = CreateSurfaceQueue(&desc, m_pD3DDeviceMadVR, &m_pD3D11Queue)))
    return hr;

  // Clone the queue
  SURFACE_QUEUE_CLONE_DESC cloneDesc;
  cloneDesc.MetaDataSize = 0;
  cloneDesc.Flags = 0;
  if (FAILED(hr = m_pD3D11Queue->Clone(&cloneDesc, &m_pD3D9Queue)))
    return hr;

  // Open for m_pD3D9Queue
  if (FAILED(hr = m_pD3D9Queue->OpenProducer(m_pD3DDeviceMadVR, &m_pD3D9Producer)))
    return hr;
  if (FAILED(hr = m_pD3D9Queue->OpenConsumer(m_pD3DDeviceKodi, &m_pD3D11Consumer)))
    return hr;

  // Open for m_pD3D11Queue
  if (FAILED(hr = m_pD3D11Queue->OpenProducer(m_pD3DDeviceKodi, &m_pD3D11Producer)))
    return hr;
  if (FAILED(hr = m_pD3D11Queue->OpenConsumer(m_pD3DDeviceMadVR, &m_pD3D9Consumer)))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::CreateTextures(ID3D11Device* pD3DDeviceKodi, IDirect3DDevice9Ex* pD3DDeviceMadVR, int width, int height)
{
  HRESULT hr;
  m_pD3DDeviceKodi = pD3DDeviceKodi;
  m_pD3DDeviceMadVR = pD3DDeviceMadVR;
  m_dwWidth = width;
  m_dwHeight = height;

  // Create VertexBuffer
  if (FAILED(hr = m_pD3DDeviceMadVR->CreateVertexBuffer(sizeof(VID_FRAME_VERTEX) * 4, D3DUSAGE_WRITEONLY, D3DFVF_VID_FRAME_VERTEX, D3DPOOL_DEFAULT, &m_pMadvrVertexBuffer, NULL)))
    CLog::Log(LOGDEBUG, "%s Failed to create madVR vertex buffer", __FUNCTION__);

  // Create Under Surface Queue
  if (FAILED(hr = CreateSharedQueueResource()))
    CLog::Log(LOGDEBUG, "%s Failed to create under surface queue", __FUNCTION__);

  // Create Over Shared Texture
  if (FAILED(hr = CreateSharedResource(&m_pMadvrOverTexture, &m_pKodiOverTexture, &m_pKodiOverSurface)))
    CLog::Log(LOGDEBUG, "%s Failed to create over shared texture", __FUNCTION__);

  return hr;
}

HRESULT CMadvrSharedRender::Render(MADVR_RENDER_LAYER layer)
{
  if (!CMadvrCallback::Get()->GetRenderOnMadvr() || (g_graphicsContext.IsFullScreenVideo() && layer == RENDER_LAYER_UNDER))
    return CALLBACK_INFO_DISPLAY;
  
  // Dequeue GUI Textures
  DeQueue(layer);

  // Render the GUI on madVR
  RenderMadvr(layer);

  // Return to madVR if we rendered something
  return (m_bGuiVisible) ? CALLBACK_USER_INTERFACE : CALLBACK_INFO_DISPLAY;
}

void CMadvrSharedRender::DeQueue(MADVR_RENDER_LAYER layer)
{
  // Ensure to don't call the Dequeue twice with Clearbackground and RenderOSD callbacks
  if (layer == RENDER_LAYER_UNDER)
    m_bUnderRender = true;

  if (layer == RENDER_LAYER_OVER && m_bUnderRender)
  {
    m_bUnderRender = false;
    return;
  }

  CMetaData *metaData = DNew CMetaData();
  UINT size = sizeof(metaData);
  IDirect3DTexture9* pTexture9;

  HRESULT hr = m_pD3D9Consumer->Dequeue(__uuidof(IDirect3DTexture9), (void**)&pTexture9, &metaData, &size, 0);
  if (SUCCEEDED(hr))
  {
    SAFE_RELEASE(m_pUnderTexture9);

    m_pUnderTexture9 = pTexture9;
    m_bGuiVisible = metaData->bGuiVisible;
    m_bGuiVisibleOver = metaData->bGuiVisibleOver;

    m_pD3D9Producer->Enqueue(pTexture9, NULL, NULL, NULL);
    m_pD3D9Producer->Flush(NULL, NULL);
  }
}

HRESULT CMadvrSharedRender::RenderMadvr(MADVR_RENDER_LAYER layer)
{
  HRESULT hr = E_UNEXPECTED;

  // If the over layer it's empty skip the rendering of the under layer and drawn everything over madVR
  if (layer == RENDER_LAYER_UNDER && !m_bGuiVisibleOver)
    return hr;

  if (layer == RENDER_LAYER_OVER)
    m_bGuiVisibleOver ? layer = RENDER_LAYER_OVER : layer = RENDER_LAYER_UNDER;

  // Store madVR States
  if (FAILED(hr = StoreMadDeviceState()))
    return hr;

  // Setup madVR Device
  if (FAILED(hr = SetupMadDeviceState()))
    return hr;

  // Setup Vertex Buffer
  if (FAILED(hr = SetupVertex()))
    return hr;

  // Draw Kodi shared texture on madVR D3D9 device
  if (FAILED(hr = RenderTexture(layer)))
    return hr;

  // Restore madVR states
  if (FAILED(hr = RestoreMadDeviceState()))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::RenderToTexture(MADVR_RENDER_LAYER layer)
{
  HRESULT hr = S_OK;
  ID3D11DeviceContext* pContext = g_Windowing.Get3D11Context();

  CMadvrCallback::Get()->SetCurrentVideoLayer(layer);
  g_Windowing.SetVSync(false);

  if (layer == RENDER_LAYER_UNDER)
  {
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    ID3D11Texture2D*        pSurface11 = nullptr;

    CMadvrCallback::Get()->ResetRenderCount();

    hr = m_pD3D11Consumer->Dequeue(__uuidof(ID3D11Texture2D), (void**)&pSurface11, NULL, NULL, INFINITE);
    if (SUCCEEDED(hr))
    {
      m_pD3DDeviceKodi->CreateRenderTargetView(pSurface11, NULL, &pRenderTargetView);
      pContext->OMSetRenderTargets(1, &pRenderTargetView, 0);
      pContext->ClearRenderTargetView(pRenderTargetView, m_fColor);   
      
      m_pUnderTexture11 = pSurface11;
      m_pUnderSurface11 = pRenderTargetView;    
    }
  }
  else
  { 
    pContext->OMSetRenderTargets(1, &m_pKodiOverSurface, 0);
    pContext->ClearRenderTargetView(m_pKodiOverSurface, m_fColor);
  }

  return hr;
}

void CMadvrSharedRender::Flush()
{
  CMetaData *metaData = DNew CMetaData();
  metaData->bGuiVisible = CMadvrCallback::Get()->GuiVisible();
  metaData->bGuiVisibleOver = CMadvrCallback::Get()->GuiVisible(RENDER_LAYER_OVER);

  m_pD3D11Producer->Enqueue(m_pUnderTexture11, &metaData, sizeof(metaData), NULL);
  m_pD3D11Producer->Flush(NULL, NULL);
  SAFE_RELEASE(m_pUnderSurface11);
  SAFE_RELEASE(m_pUnderTexture11);
}

HRESULT CMadvrSharedRender::RenderTexture(MADVR_RENDER_LAYER layer)
{
  IDirect3DTexture9* pTexture9;

  layer == RENDER_LAYER_UNDER ? pTexture9 = m_pUnderTexture9 : pTexture9 = m_pMadvrOverTexture;

  HRESULT hr = m_pD3DDeviceMadVR->SetStreamSource(0, m_pMadvrVertexBuffer, 0, sizeof(VID_FRAME_VERTEX));
  if (FAILED(hr))
    return hr;

  hr = m_pD3DDeviceMadVR->SetTexture(0, pTexture9);
  if (FAILED(hr))
    return hr;

  hr = m_pD3DDeviceMadVR->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
  if (FAILED(hr))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::SetupVertex()
{
  VID_FRAME_VERTEX* vertices = nullptr;

  // Lock the vertex buffer
  HRESULT hr = m_pMadvrVertexBuffer->Lock(0, 0, (void**)&vertices, D3DLOCK_DISCARD);

  if (SUCCEEDED(hr))
  {
    RECT rDest;
    rDest.bottom = m_dwHeight;
    rDest.left = 0;
    rDest.right = m_dwWidth;
    rDest.top = 0;

    vertices[0].x = (float)rDest.left - 0.5f;
    vertices[0].y = (float)rDest.top - 0.5f;
    vertices[0].z = 0.0f;
    vertices[0].rhw = 1.0f;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = (float)rDest.right - 0.5f;
    vertices[1].y = (float)rDest.top - 0.5f;
    vertices[1].z = 0.0f;
    vertices[1].rhw = 1.0f;
    vertices[1].u = 1.0f;
    vertices[1].v = 0.0f;

    vertices[2].x = (float)rDest.right - 0.5f;
    vertices[2].y = (float)rDest.bottom - 0.5f;
    vertices[2].z = 0.0f;
    vertices[2].rhw = 1.0f;
    vertices[2].u = 1.0f;
    vertices[2].v = 1.0f;

    vertices[3].x = (float)rDest.left - 0.5f;
    vertices[3].y = (float)rDest.bottom - 0.5f;
    vertices[3].z = 0.0f;
    vertices[3].rhw = 1.0f;
    vertices[3].u = 0.0f;
    vertices[3].v = 1.0f;

    hr = m_pMadvrVertexBuffer->Unlock();
    if (FAILED(hr))
      return hr;
  }

  return hr;
}

HRESULT CMadvrSharedRender::StoreMadDeviceState()
{
  HRESULT hr = E_UNEXPECTED;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetScissorRect(&m_oldScissorRect)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetVertexShader(&m_pOldVS)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetFVF(&m_dwOldFVF)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetTexture(0, &m_pOldTexture)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetStreamSource(0, &m_pOldStreamData, &m_nOldOffsetInBytes, &m_nOldStride)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_CULLMODE, &m_D3DRS_CULLMODE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_LIGHTING, &m_D3DRS_LIGHTING)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_ZENABLE, &m_D3DRS_ZENABLE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_D3DRS_ALPHABLENDENABLE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_SRCBLEND, &m_D3DRS_SRCBLEND)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetRenderState(D3DRS_DESTBLEND, &m_D3DRS_DESTBLEND)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->GetPixelShader(&m_pPix)))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::SetupMadDeviceState()
{
  HRESULT hr = E_UNEXPECTED;

  RECT newScissorRect;
  newScissorRect.bottom = m_dwHeight;
  newScissorRect.top = 0;
  newScissorRect.left = 0;
  newScissorRect.right = m_dwWidth;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetScissorRect(&newScissorRect)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetVertexShader(NULL)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetFVF(D3DFVF_VID_FRAME_VERTEX)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetPixelShader(NULL)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_LIGHTING, FALSE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_ZENABLE, FALSE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)))
    return hr;

  return hr;
}

HRESULT CMadvrSharedRender::RestoreMadDeviceState()
{
  HRESULT hr = S_FALSE;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetScissorRect(&m_oldScissorRect)))
    return hr;

  hr = m_pD3DDeviceMadVR->SetTexture(0, m_pOldTexture);

  if (m_pOldTexture)
    m_pOldTexture->Release();

  if (FAILED(hr))
    return hr;

  hr = m_pD3DDeviceMadVR->SetVertexShader(m_pOldVS);

  if (m_pOldVS)
    m_pOldVS->Release();

  if (FAILED(hr))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetFVF(m_dwOldFVF)))
    return hr;

  hr = m_pD3DDeviceMadVR->SetStreamSource(0, m_pOldStreamData, m_nOldOffsetInBytes, m_nOldStride);

  if (m_pOldStreamData)
    m_pOldStreamData->Release();

  if (FAILED(hr))
    return hr;

  hr = m_pD3DDeviceMadVR->SetPixelShader(m_pPix);
  if (m_pPix)
    m_pPix->Release();

  if (FAILED(hr))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_CULLMODE, m_D3DRS_CULLMODE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_LIGHTING, m_D3DRS_LIGHTING)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_ZENABLE, m_D3DRS_ZENABLE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_ALPHABLENDENABLE, m_D3DRS_ALPHABLENDENABLE)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_SRCBLEND, m_D3DRS_SRCBLEND)))
    return hr;

  if (FAILED(hr = m_pD3DDeviceMadVR->SetRenderState(D3DRS_DESTBLEND, m_D3DRS_DESTBLEND)))
    return hr;

  return hr;
}