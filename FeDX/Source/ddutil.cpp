//-----------------------------------------------------------------------------
// File: ddutil.cpp
//
// Desc: DirectDraw framewark classes. Feel free to use this class as a 
//       starting point for adding extra functionality.
//
//
// Copyright (c) 1995-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include "ddutil.h"
#include "dxutil.h"






//-----------------------------------------------------------------------------
// Name: CDisplay()
// Desc:
//-----------------------------------------------------------------------------
CDisplay::CDisplay()
{
    m_pDD                = NULL;
    m_pddsFrontBuffer    = NULL;
    m_pddsBackBuffer     = NULL;
    m_pddsBackBufferLeft = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CDisplay()
// Desc:
//-----------------------------------------------------------------------------
CDisplay::~CDisplay()
{
    DestroyObjects();
}




//-----------------------------------------------------------------------------
// Name: DestroyObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CDisplay::DestroyObjects()
{
    SAFE_RELEASE( m_pddsBackBufferLeft );
    SAFE_RELEASE( m_pddsBackBuffer );
    SAFE_RELEASE( m_pddsFrontBuffer );

    if( m_pDD )
        m_pDD->SetCooperativeLevel( m_hWnd, DDSCL_NORMAL );

    SAFE_RELEASE( m_pDD );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateFullScreenDisplay()
// Desc:
//-----------------------------------------------------------------------------
    



//-----------------------------------------------------------------------------
// Name: CreateWindowedDisplay()
// Desc:
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurface( CSurface** ppSurface,
                                 DWORD dwWidth, DWORD dwHeight )
{
    if( NULL == m_pDD )
        return E_POINTER;
    if( NULL == ppSurface )
        return E_INVALIDARG;

    HRESULT        hr;
    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize         = sizeof( ddsd );
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = dwWidth;
    ddsd.dwHeight       = dwHeight;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource or bitmap file.
//       Use MAKEINTRESOURCE() to pass a constant into strBMP.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromBitmap( CSurface** ppSurface,
                                           TCHAR* strBMP,                                            
                                           DWORD dwDesiredWidth, 
                                           DWORD dwDesiredHeight )
{
    HRESULT        hr;
    HBITMAP        hBMP = NULL;
    BITMAP         bmp;
    DDSURFACEDESC2 ddsd;

    if( m_pDD == NULL || strBMP == NULL || ppSurface == NULL ) 
        return E_INVALIDARG;

    *ppSurface = NULL;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hBMP = (HBITMAP) LoadImage( GetModuleHandle(NULL), strBMP, 
                                IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                LR_CREATEDIBSECTION );
    if( hBMP == NULL )
    {
        hBMP = (HBITMAP) LoadImage( NULL, strBMP, 
                                    IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION );
        if( hBMP == NULL )
            return E_FAIL;
    }

    // Get size of the bitmap
    GetObject( hBMP, sizeof(bmp), &bmp );

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = bmp.bmWidth;
    ddsd.dwHeight       = bmp.bmHeight;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    // Draw the bitmap on this surface
    if( FAILED( hr = (*ppSurface)->DrawBitmap( hBMP, 0, 0, 0, 0 ) ) )
    {
        DeleteObject( hBMP );
        return hr;
    }

    DeleteObject( hBMP );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CDisplay::CreateSurfaceFromText()
// Desc: Creates a DirectDrawSurface from a text string using hFont or the default 
//       GDI font if hFont is NULL.
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreateSurfaceFromText( CSurface** ppSurface,
                                         HFONT hFont, TCHAR* strText, 
                                         COLORREF crBackground, COLORREF crForeground )
{
    HDC                  hDC  = NULL;
    LPDIRECTDRAWSURFACE7 pDDS = NULL;
    HRESULT              hr;
    DDSURFACEDESC2       ddsd;
    SIZE                 sizeText;

    if( m_pDD == NULL || strText == NULL || ppSurface == NULL )
        return E_INVALIDARG;

    *ppSurface = NULL;

    hDC = GetDC( NULL );

    if( hFont )
        SelectObject( hDC, hFont );

    GetTextExtentPoint32( hDC, strText, _tcslen(strText), &sizeText );
    ReleaseDC( NULL, hDC );

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth        = sizeText.cx;
    ddsd.dwHeight       = sizeText.cy;

    (*ppSurface) = new CSurface();
    if( FAILED( hr = (*ppSurface)->Create( m_pDD, &ddsd ) ) )
    {
        delete (*ppSurface);
        return hr;
    }

    if( FAILED( hr = (*ppSurface)->DrawText( hFont, strText, 0, 0, 
                                             crBackground, crForeground ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Present()
{
    HRESULT hr;

    if( NULL == m_pddsFrontBuffer && NULL == m_pddsBackBuffer )
        return E_POINTER;

    while( 1 )
    {
        if( m_bWindowed )
            hr = m_pddsFrontBuffer->Blt( &m_rcWindow, m_pddsBackBuffer,
                                         NULL, DDBLT_WAIT, NULL );
        else
            //hr = m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT );
		hr = m_pddsFrontBuffer->Blt( &m_rcWindow, m_pddsBackBuffer,
                                         NULL, DDBLT_WAIT, NULL );

        if( hr == DDERR_SURFACELOST )
        {
            m_pddsFrontBuffer->Restore();
            m_pddsBackBuffer->Restore();
        }

        if( hr != DDERR_WASSTILLDRAWING )
            return hr;
    }
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::ShowBitmap( HBITMAP hbm, LPDIRECTDRAWPALETTE pPalette )
{
    if( NULL == m_pddsFrontBuffer ||  NULL == m_pddsBackBuffer )
        return E_POINTER;

    // Set the palette before loading the bitmap
    if( pPalette )
        m_pddsFrontBuffer->SetPalette( pPalette );

    CSurface backBuffer;
    backBuffer.Create( m_pddsBackBuffer );

    if( FAILED( backBuffer.DrawBitmap( hbm, 0, 0, 0, 0 ) ) )
        return E_FAIL;

    return Present();
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::ColorKeyBlt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds,
                               RECT* prc )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    return m_pddsBackBuffer->BltFast( x, y, pdds, prc, DDBLTFAST_SRCCOLORKEY );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Blt( DWORD x, DWORD y, LPDIRECTDRAWSURFACE7 pdds, RECT* prc,
                       DWORD dwFlags )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    return m_pddsBackBuffer->BltFast( x, y, pdds, prc, dwFlags );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Blt( DWORD x, DWORD y, CSurface* pSurface, RECT* prc )
{
    if( NULL == pSurface )
        return E_INVALIDARG;

    if( pSurface->IsColorKeyed() )
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, DDBLTFAST_SRCCOLORKEY );
    else
        return Blt( x, y, pSurface->GetDDrawSurface(), prc, 0L );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::Clear( DWORD dwColor )
{
    if( NULL == m_pddsBackBuffer )
        return E_POINTER;

    // Erase the background
    DDBLTFX ddbltfx;
    ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = dwColor;

    return m_pddsBackBuffer->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::SetPalette( LPDIRECTDRAWPALETTE pPalette )
{
    if( NULL == m_pddsFrontBuffer )
        return E_POINTER;

    return m_pddsFrontBuffer->SetPalette( pPalette );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::CreatePaletteFromBitmap( LPDIRECTDRAWPALETTE* ppPalette,
                                           const TCHAR* strBMP )
{
    HRSRC             hResource      = NULL;
    RGBQUAD*          pRGB           = NULL;
    BITMAPINFOHEADER* pbi = NULL;
    PALETTEENTRY      aPalette[256];
    HANDLE            hFile = NULL;
    DWORD             iColor;
    DWORD             dwColors;
    BITMAPFILEHEADER  bf;
    BITMAPINFOHEADER  bi;
    DWORD             dwBytesRead;

    if( m_pDD == NULL || strBMP == NULL || ppPalette == NULL )
        return E_INVALIDARG;

    *ppPalette = NULL;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hResource = FindResource( NULL, strBMP, RT_BITMAP );
    if( hResource )
    {
        pbi = (LPBITMAPINFOHEADER) LockResource( LoadResource( NULL, hResource ) );       
        if( NULL == pbi )
            return E_FAIL;

        pRGB = (RGBQUAD*) ( (BYTE*) pbi + pbi->biSize );

        // Figure out how many colors there are
        if( pbi == NULL || pbi->biSize < sizeof(BITMAPINFOHEADER) )
            dwColors = 0;
        else if( pbi->biBitCount > 8 )
            dwColors = 0;
        else if( pbi->biClrUsed == 0 )
            dwColors = 1 << pbi->biBitCount;
        else
            dwColors = pbi->biClrUsed;

        //  A DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        for( iColor = 0; iColor < dwColors; iColor++ )
        {
            aPalette[iColor].peRed   = pRGB[iColor].rgbRed;
            aPalette[iColor].peGreen = pRGB[iColor].rgbGreen;
            aPalette[iColor].peBlue  = pRGB[iColor].rgbBlue;
            aPalette[iColor].peFlags = 0;
        }

        return m_pDD->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
    }

    // Attempt to load bitmap as a file
    hFile = CreateFile( strBMP, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( NULL == hFile )
        return E_FAIL;

    // Read the BITMAPFILEHEADER
    ReadFile( hFile, &bf, sizeof(bf), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(bf) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    // Read the BITMAPINFOHEADER
    ReadFile( hFile, &bi, sizeof(bi), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(bi) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    // Read the PALETTEENTRY 
    ReadFile( hFile, aPalette, sizeof(aPalette), &dwBytesRead, NULL );
    if( dwBytesRead != sizeof(aPalette) )
    {
        CloseHandle( hFile );
        return E_FAIL;
    }

    CloseHandle( hFile );

    // Figure out how many colors there are
    if( bi.biSize != sizeof(BITMAPINFOHEADER) )
        dwColors = 0;
    else if (bi.biBitCount > 8)
        dwColors = 0;
    else if (bi.biClrUsed == 0)
        dwColors = 1 << bi.biBitCount;
    else
        dwColors = bi.biClrUsed;

    //  A DIB color table has its colors stored BGR not RGB
    //  so flip them around since DirectDraw uses RGB
    for( iColor = 0; iColor < dwColors; iColor++ )
    {
        BYTE r = aPalette[iColor].peRed;
        aPalette[iColor].peRed  = aPalette[iColor].peBlue;
        aPalette[iColor].peBlue = r;
    }

    return m_pDD->CreatePalette( DDPCAPS_8BIT, aPalette, ppPalette, NULL );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::UpdateBounds()
{
    if( m_bWindowed )
    {
        GetClientRect( m_hWnd, &m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow );
        ClientToScreen( m_hWnd, (POINT*)&m_rcWindow+1 );
    }
    else
    {
        SetRect( &m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                 GetSystemMetrics(SM_CYSCREEN) );
    }

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: CDisplay::InitClipper
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::InitClipper()
{
    LPDIRECTDRAWCLIPPER pClipper;
    HRESULT hr;

    // Create a clipper when using GDI to draw on the primary surface 
    if( FAILED( hr = m_pDD->CreateClipper( 0, &pClipper, NULL ) ) )
        return hr;

    pClipper->SetHWnd( 0, m_hWnd );

    if( FAILED( hr = m_pddsFrontBuffer->SetClipper( pClipper ) ) )
        return hr;

    // We can release the clipper now since g_pDDSPrimary 
    // now maintains a ref count on the clipper
    SAFE_RELEASE( pClipper );

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface::CSurface()
{
    m_pdds = NULL;
    m_bColorKeyed = NULL;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSurface::~CSurface()
{
    SAFE_RELEASE( m_pdds );
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create( LPDIRECTDRAWSURFACE7 pdds )
{
    m_pdds = pdds;

    if( m_pdds )
    {
        m_pdds->AddRef();

        // Get the DDSURFACEDESC structure for this surface
        m_ddsd.dwSize = sizeof(m_ddsd);
        m_pdds->GetSurfaceDesc( &m_ddsd );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Create( LPDIRECTDRAW7 pDD, DDSURFACEDESC2* pddsd )
{
    HRESULT hr;

    // Create the DDraw surface
    if( FAILED( hr = pDD->CreateSurface( pddsd, &m_pdds, NULL ) ) )
        return hr;

    // Prepare the DDSURFACEDESC structure
    m_ddsd.dwSize = sizeof(m_ddsd);

    // Get the DDSURFACEDESC structure for this surface
    m_pdds->GetSurfaceDesc( &m_ddsd );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::Destroy()
{
    SAFE_RELEASE( m_pdds );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::DrawBitmap()
// Desc: Draws a bitmap over an entire DirectDrawSurface, stretching the 
//       bitmap if nessasary
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap( HBITMAP hBMP, 
                              DWORD dwBMPOriginX, DWORD dwBMPOriginY, 
                              DWORD dwBMPWidth, DWORD dwBMPHeight )
{
    HDC            hDCImage;
    HDC            hDC;
    BITMAP         bmp;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    if( hBMP == NULL || m_pdds == NULL )
        return E_INVALIDARG;

    // Make sure this surface is restored.
    if( FAILED( hr = m_pdds->Restore() ) )
        return hr;

    // Get the surface.description
    ddsd.dwSize  = sizeof(ddsd);
    m_pdds->GetSurfaceDesc( &ddsd );

    if( ddsd.ddpfPixelFormat.dwFlags == DDPF_FOURCC )
        return E_NOTIMPL;

    // Select bitmap into a memoryDC so we can use it.
    hDCImage = CreateCompatibleDC( NULL );
    if( NULL == hDCImage )
        return E_FAIL;

    SelectObject( hDCImage, hBMP );

    // Get size of the bitmap
    GetObject( hBMP, sizeof(bmp), &bmp );

    // Use the passed size, unless zero
    dwBMPWidth  = ( dwBMPWidth  == 0 ) ? bmp.bmWidth  : dwBMPWidth;     
    dwBMPHeight = ( dwBMPHeight == 0 ) ? bmp.bmHeight : dwBMPHeight;

    // Stretch the bitmap to cover this surface
    if( FAILED( hr = m_pdds->GetDC( &hDC ) ) )
        return hr;

    StretchBlt( hDC, 0, 0, 
                ddsd.dwWidth, ddsd.dwHeight, 
                hDCImage, dwBMPOriginX, dwBMPOriginY,
                dwBMPWidth, dwBMPHeight, SRCCOPY );

    if( FAILED( hr = m_pdds->ReleaseDC( hDC ) ) )
        return hr;

    DeleteDC( hDCImage );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::DrawText()
// Desc: Draws a text string on a DirectDraw surface using hFont or the default
//       GDI font if hFont is NULL.  
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawText( HFONT hFont, TCHAR* strText, 
                            DWORD dwOriginX, DWORD dwOriginY,
                            COLORREF crBackground, COLORREF crForeground )
{
    HDC     hDC = NULL;
    HRESULT hr;

    if( m_pdds == NULL || strText == NULL )
        return E_INVALIDARG;

    // Make sure this surface is restored.
    if( FAILED( hr = m_pdds->Restore() ) )
        return hr;

    if( FAILED( hr = m_pdds->GetDC( &hDC ) ) )
        return hr;

    // Set the background and foreground color
    SetBkColor( hDC, crBackground );
    SetTextColor( hDC, crForeground );

    if( hFont )
        SelectObject( hDC, hFont );

    // Use GDI to draw the text on the surface
    TextOut( hDC, dwOriginX, dwOriginY, strText, _tcslen(strText) );

    if( FAILED( hr = m_pdds->ReleaseDC( hDC ) ) )
        return hr;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CSurface::ReDrawBitmapOnSurface()
// Desc: Load a bitmap from a file or resource into a DirectDraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT CSurface::DrawBitmap( TCHAR* strBMP, 
                              DWORD dwDesiredWidth, DWORD dwDesiredHeight  )
{
    HBITMAP hBMP;
    HRESULT hr;

    if( m_pdds == NULL || strBMP == NULL )
        return E_INVALIDARG;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hBMP = (HBITMAP) LoadImage( GetModuleHandle(NULL), strBMP, 
                                IMAGE_BITMAP, dwDesiredWidth, dwDesiredHeight, 
                                LR_CREATEDIBSECTION );
    if( hBMP == NULL )
    {
        hBMP = (HBITMAP) LoadImage( NULL, strBMP, IMAGE_BITMAP, 
                                    dwDesiredWidth, dwDesiredHeight, 
                                    LR_LOADFROMFILE | LR_CREATEDIBSECTION );
        if( hBMP == NULL )
            return E_FAIL;
    }

    // Draw the bitmap on this surface
    if( FAILED( hr = DrawBitmap( hBMP, 0, 0, 0, 0 ) ) )
    {
        DeleteObject( hBMP );
        return hr;
    }

    DeleteObject( hBMP );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CSurface::SetColorKey( DWORD dwColorKey )
{
    if( NULL == m_pdds )
        return E_POINTER;

    m_bColorKeyed = TRUE;

    DDCOLORKEY ddck;
    ddck.dwColorSpaceLowValue  = ConvertGDIColor( dwColorKey );
    ddck.dwColorSpaceHighValue = ConvertGDIColor( dwColorKey );
    
    return m_pdds->SetColorKey( DDCKEY_SRCBLT, &ddck );
}





//-----------------------------------------------------------------------------
// Name: CSurface::ConvertGDIColor()
// Desc: Converts a GDI color (0x00bbggrr) into the equivalent color on a 
//       DirectDrawSurface using its pixel format.  
//-----------------------------------------------------------------------------
DWORD CSurface::ConvertGDIColor( COLORREF dwGDIColor )
{
    if( m_pdds == NULL )
	    return 0x00000000;

    COLORREF       rgbT;
    HDC            hdc;
    DWORD          dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT        hr;

    //  Use GDI SetPixel to color match for us
    if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
        m_pdds->ReleaseDC(hdc);
    }

    // Now lock the surface so we can read back the converted color
    ddsd.dwSize = sizeof(ddsd);
    hr = m_pdds->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
    if( hr == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface; 
        if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
            dw &= ( 1 << ddsd.ddpfPixelFormat.dwRGBBitCount ) - 1;  
        m_pdds->Unlock(NULL);
    }

    //  Now put the color that was there back.
    if( dwGDIColor != CLR_INVALID && m_pdds->GetDC(&hdc) == DD_OK )
    {
        SetPixel( hdc, 0, 0, rgbT );
        m_pdds->ReleaseDC(hdc);
    }
    
    return dw;    
}




//-----------------------------------------------------------------------------
// Name: CSurface::GetBitMaskInfo()
// Desc: Returns the number of bits and the shift in the bit mask
//-----------------------------------------------------------------------------
HRESULT CSurface::GetBitMaskInfo( DWORD dwBitMask, DWORD* pdwShift, DWORD* pdwBits )
{
    DWORD dwShift = 0;
    DWORD dwBits  = 0; 

    if( pdwShift == NULL || pdwBits == NULL )
        return E_INVALIDARG;

    if( dwBitMask )
    {
        while( (dwBitMask & 1) == 0 )
        {
            dwShift++;
            dwBitMask >>= 1;
        }
    }

    while( (dwBitMask & 1) != 0 )
    {
        dwBits++;
        dwBitMask >>= 1;
    }

    *pdwShift = dwShift;
    *pdwBits  = dwBits;

    return S_OK;
}
