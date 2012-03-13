//---------------------------------------------------------------------------
/*#pragma hdrstop

#include "mkiddraw.h"
#include "mkiddrawsurface.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

MKIDDraw::MKIDDraw(LPDIRECTDRAW lpTADD, bool CreateFullScreen)
{
  lpDD = lpTADD;
  Windowed = CreateFullScreen;
  OutptTxt("MKIDDraw Created");

  LocalShare->DDrawClass = this;
  LocalShare->TADirectDraw = lpTADD;

  //check if version is 3.1 standar
  if((*((unsigned char*)0x4ad494))==0x00 && (*((unsigned char*)0x4ad495))==0x55 && (*((unsigned char*)0x4ad496))==0xe8)
    LocalShare->CompatibleVersion = true;
  else
    {
    LocalShare->CompatibleVersion = false;
    DataShare->ehaOff = 1; //set the ehaofvariable
    }
}

HRESULT __stdcall MKIDDraw::QueryInterface(REFIID riid , LPVOID FAR * ppvObj)
{
  OutptTxt("QueryInterface");
  return lpDD->QueryInterface(riid, ppvObj);
}

ULONG __stdcall MKIDDraw::AddRef()
{
  OutptTxt("AddRef");
  return lpDD->AddRef();
}

ULONG __stdcall MKIDDraw::Release()
{
  OutptTxt("Release");
  ULONG result = lpDD->Release();
  delete this;
  return result;
}

HRESULT __stdcall MKIDDraw::Compact()
{
  OutptTxt("Compact");
  return lpDD->Compact();
}

HRESULT __stdcall MKIDDraw::CreateClipper(DWORD arg1, LPDIRECTDRAWCLIPPER FAR* arg2, IUnknown FAR * arg3)
{
  OutptTxt("CreateClipper");
  return lpDD->CreateClipper(arg1, arg2, arg3);
}

HRESULT __stdcall MKIDDraw::CreatePalette(DWORD arg1, LPPALETTEENTRY arg2, LPDIRECTDRAWPALETTE FAR* arg3, IUnknown FAR * arg4)
{
  OutptTxt("CreatePalette");
  return lpDD->CreatePalette(arg1, arg2, arg3, arg4);
}

HRESULT __stdcall MKIDDraw::CreateSurface(LPDDSURFACEDESC arg1, LPDIRECTDRAWSURFACE FAR *arg2, IUnknown FAR *arg3)
{
  OutptTxt("CreateSurface");

  arg1->dwBackBufferCount = 2;
  HRESULT result = lpDD->CreateSurface(arg1, arg2, arg3);
  MKIDDrawSurface *SClass = new MKIDDrawSurface(*arg2, Windowed, ScreenWidth, ScreenHeight);
  //SClass->ScreenWidth = ScreenWidth;
  *arg2 = (IDirectDrawSurface*)SClass;
  return result;

}

HRESULT __stdcall MKIDDraw::DuplicateSurface(LPDIRECTDRAWSURFACE arg1, LPDIRECTDRAWSURFACE FAR * arg2)
{
  OutptTxt("DuplicateSurface");
  return lpDD->DuplicateSurface(arg1, arg2);
}

HRESULT __stdcall MKIDDraw::EnumDisplayModes(DWORD arg1, LPDDSURFACEDESC arg2, LPVOID arg3, LPDDENUMMODESCALLBACK arg4)
{
  OutptTxt("EnumDisplayModes");
  return lpDD->EnumDisplayModes(arg1, arg2, arg3, arg4);
}

HRESULT __stdcall MKIDDraw::EnumSurfaces(DWORD arg1, LPDDSURFACEDESC arg2, LPVOID arg3,LPDDENUMSURFACESCALLBACK arg4)
{
  OutptTxt("EnumSurfaces");
  return lpDD->EnumSurfaces(arg1, arg2, arg3, arg4);
}

HRESULT __stdcall MKIDDraw::FlipToGDISurface()
{
  OutptTxt("FlipToGDISurface");
  return lpDD->FlipToGDISurface();
}

HRESULT __stdcall MKIDDraw::GetCaps(LPDDCAPS arg1, LPDDCAPS arg2)
{
  OutptTxt("GetCaps");
  return lpDD->GetCaps(arg1, arg2);
}

HRESULT __stdcall MKIDDraw::GetDisplayMode(LPDDSURFACEDESC arg1)
{
  OutptTxt("GetDisplayMode");
  return lpDD->GetDisplayMode(arg1);
}

HRESULT __stdcall MKIDDraw::GetFourCCCodes(LPDWORD arg1, LPDWORD arg2)
{
  OutptTxt("GetFourCCCodes");
  return lpDD->GetFourCCCodes(arg1, arg2);
}

HRESULT __stdcall MKIDDraw::GetGDISurface(LPDIRECTDRAWSURFACE FAR *arg1)
{
  OutptTxt("GetGDISurface");
  return lpDD->GetGDISurface(arg1);
}

HRESULT __stdcall MKIDDraw::GetMonitorFrequency(LPDWORD arg1)
{
  OutptTxt("GetMonitorFrequency");
  return lpDD->GetMonitorFrequency(arg1);
}

HRESULT __stdcall MKIDDraw::GetScanLine(LPDWORD arg1)
{
  OutptTxt("GetScanLine");
  return lpDD->GetScanLine(arg1);
}

HRESULT __stdcall MKIDDraw::GetVerticalBlankStatus(LPBOOL arg1)
{
  OutptTxt("GetVerticalBlankStatus");
  return lpDD->GetVerticalBlankStatus(arg1);
}

HRESULT __stdcall MKIDDraw::Initialize(GUID FAR *arg1)
{
  OutptTxt("Initialize");
  return lpDD->Initialize(arg1);
}

HRESULT __stdcall MKIDDraw::RestoreDisplayMode()
{
  OutptTxt("RestoreDisplayMode");
  return lpDD->RestoreDisplayMode();
}

HRESULT __stdcall MKIDDraw::SetCooperativeLevel(HWND arg1, DWORD arg2)
{
  OutptTxt("SetCooperativeLevel");
  if(Windowed == false)
    return lpDD->SetCooperativeLevel(arg1, arg2);
  else
    {
    return lpDD->SetCooperativeLevel(arg1, DDSCL_NORMAL);
    }
}

HRESULT __stdcall MKIDDraw::SetDisplayMode(DWORD arg1, DWORD arg2,DWORD arg3)
{
  OutptTxt("SetDisplayMode");

  ScreenWidth = arg1;
  ScreenHeight = arg2;
  LocalShare->ScreenWidth = ScreenWidth;
  LocalShare->ScreenHeight = ScreenHeight;

  return lpDD->SetDisplayMode(arg1, arg2, arg3);

}

HRESULT __stdcall MKIDDraw::WaitForVerticalBlank(DWORD arg1, HANDLE arg2)
{
  OutptTxt("WaitForVerticalBlank");
  return lpDD->WaitForVerticalBlank(arg1, arg2);
}

void MKIDDraw::OutptTxt(char *string)
{
  if(!Log)
    return;
  //AnsiString CPath = "c:\\taddrawlog.txt";

  FILE *file;
  file = fopen("C:\\temp\\taddrawlog.txt", "a");

  fwrite(string, strlen(string), 1, file);
  fwrite("\n", 1, 1, file);

  fclose(file);
}         */
