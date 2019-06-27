// WindowsProject3.cpp : Definiuje punkt wejścia dla aplikacji.


#include "stdafx.h"
#include "WindowsProject3.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <queue>
using namespace std;

#define MAX_LOADSTRING 100

//definicje kolorów
#define CZERWONY 255,0,0
#define NIEBIESKI 0,0,255
#define FIOLETOWY 255,0,225

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego
ULONG_PTR m_gdiplusToken;

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct dane {
	int a;
};
dane WYSOKOSC_OKNA, SZEROKOSC_OKNA;

class rysuj_ramie {
public:

	LONG start_x = 0;
	LONG start_y = 0;
	LONG koniec_x = 0;
	LONG koniec_y = 0;
	LONG dlugosc = 250; //dlugosc ramienia


	bool poczatkowe_ramie = FALSE;
	double nachylenie = 0;
	int predkosc_zmiany = 0;
	int domyslna_predkosc = 0;

	void start(HDC hdc) {
		odswiez(poczatkowe_ramie, hdc);
	}

private:
	PAINTSTRUCT ps;
	RECT naroznik;
	LONG polozenie_x;
	LONG polozenie_y;
	void odswiez(bool poczatkowe_ramie, HDC hdc)
	{
		Graphics graphics(hdc);
		Pen obrys(Color(255, NIEBIESKI), 10);


		do {

			koniec_x = LONG(dlugosc* cos(nachylenie));  
			koniec_y = LONG(dlugosc* sin(nachylenie));

			if (poczatkowe_ramie)
			{
				start_x = LONG(SZEROKOSC_OKNA.a / 2);
				start_y = WYSOKOSC_OKNA.a;
			}

			koniec_x += start_x;
			koniec_y = start_y - koniec_y;

			if (nachylenie < 0) nachylenie += 0.01;
			else if (nachylenie > M_PI) nachylenie -= 0.01;

		} while (koniec_y > WYSOKOSC_OKNA.a);//poprawa nachylenia

		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.DrawLine(&obrys, start_x, start_y, koniec_x, koniec_y);
	}


};
class rysuj_kolo {
public:
	void start(HDC hdc, int polozenie_x, int polozenie_y, int srednica = 20) {
		rysuj(hdc, polozenie_x, polozenie_y, srednica);
	}
private:
	PAINTSTRUCT ps;
	Rect rect;
	void rysuj(HDC hdc, int polozenie_x, int polozenie_y, int srednica)
	{
		rect.X = polozenie_x - srednica / 2;
		rect.Y = polozenie_y - srednica / 2;
		rect.Height = srednica;
		rect.Width = srednica;

		Graphics graphics(hdc);
		Pen obrys(Color(255, CZERWONY), 1);
		SolidBrush wypelnienie(Color(CZERWONY));
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.DrawEllipse(&obrys, rect);
		graphics.FillEllipse(&wypelnienie, rect);
	}


};
class rysuj_blok {
public:
	int polozenie_x = 0, polozenie_y = 0;
	int wielkosc = 30;
	bool poczatek = TRUE, poruszaj = FALSE;
	int temp_x = 0, temp_y = 0;
	int nr = 0;//numer bloku konieczny do poczatkowego pozycjonowania


	void start(HDC hdc, int r, int g, int b) {
		rysuj(hdc, r, g, b);
	}
private:
	PAINTSTRUCT ps;
	Rect rect;
	void rysuj(HDC hdc, int r, int g, int b)
	{
		if (poczatek)
		{
			polozenie_x = SZEROKOSC_OKNA.a / 2 - wielkosc - 200 - nr * (wielkosc + 10);//nie zachodzenie bloków na siebie przy pierwszym uruchomieniu
			polozenie_y = WYSOKOSC_OKNA.a - wielkosc;
			poczatek = FALSE;
		}
		rect.X = polozenie_x;
		rect.Y = polozenie_y;
		rect.Height = wielkosc;
		rect.Width = wielkosc;

		if (rect.Y + rect.Height > WYSOKOSC_OKNA.a)//jesli blok wyszedl poza okno
		{
			rect.Y = WYSOKOSC_OKNA.a - rect.Height;//ustaw maksymalnie przy koncowej krawedzi
		}

		Graphics graphics(hdc);
		Pen obrys(Color(255, r, g, b), 1);
		SolidBrush wypelnienie(Color(r, g, b));
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);

		graphics.DrawRectangle(&obrys, rect);
		graphics.FillRectangle(&wypelnienie, rect);
	}
};

rysuj_ramie ramie1;
rysuj_ramie ramie2;

rysuj_kolo kolo1;
rysuj_kolo kolo2;

rysuj_blok blok1;
rysuj_blok blok2;
rysuj_blok blok3;
rysuj_blok blok4;
rysuj_blok blok5;
rysuj_blok blok6;

class start_zapis
{
public:
	HWND hWnd;
	queue <double> nachylenie[2]; // 0 - ramie 1, 1 - ramie 2
	queue <int> blok[12];//przechowuje info na temat polozenia blokow : [0]-x,[1]-y (1 bloku) itd.
	char tryb = IDM_BRESET;
	int predkosc_odtwarzania = 25;

	void reset(void)
	{
		while (!nachylenie[0].empty()) //tak dlugo wyciaga z kolejek az wszytsko bedzie puste
		{
			nachylenie[0].pop();
			nachylenie[1].pop();
			blok[0].pop();
			blok[1].pop();
			blok[2].pop();
			blok[3].pop();
			blok[4].pop();
			blok[5].pop();
			blok[6].pop();
			blok[7].pop();
			blok[8].pop();
			blok[9].pop();
			blok[10].pop();
			blok[11].pop();
		}
	}
	void odtworz(void)
	{
		while (!nachylenie[0].empty())    // przypisywanie ostatniego nachylenia/polozenia
		{
			ramie1.nachylenie = nachylenie[0].front();
			ramie2.nachylenie = nachylenie[1].front();


			blok1.polozenie_x = blok[0].front();
			blok1.polozenie_y = blok[1].front();

			blok2.polozenie_x = blok[2].front();
			blok2.polozenie_y = blok[3].front();

			blok3.polozenie_x = blok[4].front();
			blok3.polozenie_y = blok[5].front();

			blok4.polozenie_x = blok[6].front();
			blok4.polozenie_y = blok[7].front();

			blok5.polozenie_x = blok[8].front();
			blok5.polozenie_y = blok[9].front();

			blok6.polozenie_x = blok[10].front();
			blok6.polozenie_y = blok[11].front();

			InvalidateRect(hWnd, 0, 1);
			SendMessage(hWnd, WM_PAINT, -1, 0);
			Sleep(1000 / predkosc_odtwarzania);
			nachylenie[0].pop();
			nachylenie[1].pop();
			blok[0].pop();					//	usuwam juz przekazane
			blok[1].pop();
			blok[2].pop();
			blok[3].pop();
			blok[4].pop();
			blok[5].pop();
			blok[6].pop();
			blok[7].pop();
			blok[8].pop();
			blok[9].pop();
			blok[10].pop();
			blok[11].pop();
		}

		SendMessage(hWnd, WM_PAINT, 0, 0);			//aby bloczki byly ponownie widoczne
		InvalidateRect(hWnd, 0, 1);
	}
	void wczytaj_dane(void)
	{
		nachylenie[0].push(ramie1.nachylenie);
		nachylenie[1].push(ramie2.nachylenie);
		blok[0].push(blok1.polozenie_x);
		blok[1].push(blok1.polozenie_y);
		blok[2].push(blok2.polozenie_x);
		blok[3].push(blok2.polozenie_y);
		blok[4].push(blok3.polozenie_x);
		blok[5].push(blok3.polozenie_y);
		blok[6].push(blok4.polozenie_x);
		blok[7].push(blok4.polozenie_y);
		blok[8].push(blok5.polozenie_x);
		blok[9].push(blok5.polozenie_y);
		blok[10].push(blok6.polozenie_x);
		blok[11].push(blok6.polozenie_y);
	}

};

start_zapis ruchy_ramienia;

struct uchwyt {
	HWND hwnd = 0;
};
uchwyt nagrywanie, start, pause, reset, odtworz, S_odtwarzanie, E_odtwarzanie, B_odtwarzanie;

char spr_pol(int poczatek_x, int poczatek_y, int wielkosc, int x_spr, int y_spr)//funkcja sprawdza czy x_spr i y_spr znajduja się w zasięgu podanego obszaru 
{
	if ((poczatek_x <= x_spr && poczatek_x + wielkosc >= x_spr)
		&& (poczatek_y <= y_spr && poczatek_y + wielkosc >= y_spr))
		return 1;
	else
		return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: W tym miejscu umieść kod.
	ramie1.poczatkowe_ramie = TRUE;
	blok1.nr = 0;
	blok2.nr = 1;
	blok3.nr = 2;
	blok4.nr = 3;
	blok5.nr = 4;
	blok6.nr = 5;

    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

    MSG msg;

    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej
   GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   0, 0, 700, 700, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_MAXIMIZE); //max rozmiar okna;
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	ruchy_ramienia.hWnd = hWnd;

	int wmId, wmEvent;

	HMODULE hModule = GetModuleHandleW(0);

	bool wymazywanie_tla = FALSE;//przy odswiezaniu ekranu

    switch (message)
    {
    case WM_COMMAND:
        {
            wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDM_BZASTOSUJ:
			{
				TCHAR tekst[5];
				HWND hwnd_szybkosc = GetDlgItem(hWnd, IDM_ESZYBKOSC);
				GetWindowText(hwnd_szybkosc, tekst, 5);
				int liczba = _ttoi(tekst);
				ramie1.predkosc_zmiany = liczba;
				ramie2.predkosc_zmiany = liczba;
				ramie1.domyslna_predkosc = liczba;
				ramie2.domyslna_predkosc = liczba;
				SetFocus(hWnd);
				break;
			}
			case IDM_BMANUALNY:
			{

				ShowWindow(nagrywanie.hwnd, SW_HIDE);
				ShowWindow(start.hwnd, SW_HIDE);
				ShowWindow(reset.hwnd, SW_HIDE);
				ShowWindow(pause.hwnd, SW_HIDE);
				ShowWindow(odtworz.hwnd, SW_HIDE);
				ShowWindow(S_odtwarzanie.hwnd, SW_HIDE);
				ShowWindow(E_odtwarzanie.hwnd, SW_HIDE);
				ShowWindow(B_odtwarzanie.hwnd, SW_HIDE);
				SetFocus(hWnd);
				break;
			}
			case IDM_BAUTOMATYCZNY:
			{
				ShowWindow(nagrywanie.hwnd, SW_SHOW);
				ShowWindow(start.hwnd, SW_SHOW);
				ShowWindow(reset.hwnd, SW_SHOW);
				ShowWindow(pause.hwnd, SW_SHOW);
				ShowWindow(odtworz.hwnd, SW_SHOW);
				ShowWindow(S_odtwarzanie.hwnd, SW_SHOW);
				ShowWindow(E_odtwarzanie.hwnd, SW_SHOW);
				ShowWindow(B_odtwarzanie.hwnd, SW_SHOW);
				SetFocus(hWnd);
				break;
			}
			case IDM_BSTART:
			{
				ruchy_ramienia.tryb = IDM_BSTART;
				ruchy_ramienia.wczytaj_dane();	//zapamietuje poczakowe ulozenie ramion i blokow

				SetFocus(hWnd);
				break;
			}
			case IDM_BPAUSE:
			{
				ruchy_ramienia.tryb = IDM_BPAUSE;
				SetFocus(hWnd);
				break;
			}
			case IDM_BRESET:
			{
				ruchy_ramienia.tryb = IDM_BRESET;
				ruchy_ramienia.reset();
				SetFocus(hWnd);
				break;
			}
			case IDM_BODTWORZ:
			{
				ruchy_ramienia.tryb = IDM_BODTWORZ;
				ruchy_ramienia.odtworz();
				SetFocus(hWnd);
				break;
			}
			case IDM_BODTWARZANIE:
			{
				TCHAR tekst[5];
				GetWindowText(E_odtwarzanie.hwnd, tekst, 5);
				int liczba = _ttoi(tekst);
				ruchy_ramienia.predkosc_odtwarzania = liczba;
				SetFocus(hWnd);
				break;
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}
        }
        
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			HDC pamiec_hdc;
			HBITMAP pamiec_bitmapa;

			int bez_ods = 100; //wielkosc w px obszaru od góry okna która nie jest aktualizowana

			//pobiera wymiary okna, w razie jakichkolwiek ich modyfikacji
			RECT *wymiary = new RECT;

			GetClientRect(hWnd, wymiary);			
			WYSOKOSC_OKNA.a = wymiary->bottom - wymiary->top - bez_ods;
			SZEROKOSC_OKNA.a = wymiary->right - wymiary->left;

			delete wymiary;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Tutaj dodaj kod rysujący używający elementu hdc...
			pamiec_hdc = CreateCompatibleDC(hdc);	//tworzymy kontekst
			pamiec_bitmapa = CreateCompatibleBitmap(hdc, SZEROKOSC_OKNA.a, WYSOKOSC_OKNA.a);

			DWORD kolor_tla = GetSysColor(COLOR_MENU);	//pobieranie koloru menu - taki sam jak kolor tla 
			BYTE skladowa_r = GetRValue(kolor_tla);
			BYTE skladowa_g = GetGValue(kolor_tla);
			BYTE skladowa_b = GetBValue(kolor_tla);

			//ustawianie identycznego tla na bitmapie
			BYTE *tlo = new BYTE[4 * SZEROKOSC_OKNA.a*(WYSOKOSC_OKNA.a)];
			LPBITMAPINFO naglowek = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];

			naglowek->bmiHeader.biBitCount = 32;
			naglowek->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			naglowek->bmiHeader.biWidth = SZEROKOSC_OKNA.a;
			naglowek->bmiHeader.biHeight = WYSOKOSC_OKNA.a;
			naglowek->bmiHeader.biPlanes = 1;
			naglowek->bmiHeader.biCompression = BI_RGB;
			naglowek->bmiHeader.biSizeImage = 0;
			naglowek->bmiHeader.biXPelsPerMeter = 0;
			naglowek->bmiHeader.biYPelsPerMeter = 0;
			naglowek->bmiHeader.biClrUsed = 0;
			naglowek->bmiHeader.biClrImportant = 0;

			for (int i = 0; i < (SZEROKOSC_OKNA.a*WYSOKOSC_OKNA.a); i++)
			{
				tlo[4 * i] = skladowa_r;//czerwony
				tlo[(4 * i) + 1] = skladowa_g; //zielony
				tlo[(4 * i) + 2] = skladowa_b; //niebieski
				tlo[(4 * i) + 3] = 0; //nieznaczacy
			}
			SetDIBits(pamiec_hdc, pamiec_bitmapa, 0, WYSOKOSC_OKNA.a, tlo, naglowek, DIB_RGB_COLORS);
			delete[] naglowek;
			delete[] tlo;
			//koniec bialego tla

			SelectObject(pamiec_hdc, pamiec_bitmapa);			//kopiujemy nasza bitmape do hdc okna

			ramie1.start(pamiec_hdc);
			ramie2.start_x = ramie1.koniec_x;
			ramie2.start_y = ramie1.koniec_y;
			ramie2.start(pamiec_hdc);
			kolo1.start(pamiec_hdc, ramie1.start_x, ramie1.start_y);
			kolo2.start(pamiec_hdc, ramie2.start_x, ramie2.start_y);


			if (blok1.poruszaj)				//jesli  wcisnieto spacje przy ktoryms z blokow to zmieniaj jego polozenie zgodnie z koncowka ramienia 2
			{
				
				blok1.polozenie_x = ramie2.koniec_x - blok1.temp_x;
				blok1.polozenie_y = ramie2.koniec_y - blok1.temp_y;
			}
			if (blok2.poruszaj)
			{
				blok2.polozenie_x = ramie2.koniec_x - blok2.temp_x;
				blok2.polozenie_y = ramie2.koniec_y - blok2.temp_y;
			}
			if (blok3.poruszaj)
			{
				blok3.polozenie_x = ramie2.koniec_x - blok3.temp_x;
				blok3.polozenie_y = ramie2.koniec_y - blok3.temp_y;
			}
			if (blok4.poruszaj)
			{
				blok4.polozenie_x = ramie2.koniec_x - blok4.temp_x;
				blok4.polozenie_y = ramie2.koniec_y - blok4.temp_y;
			}
			if (blok5.poruszaj)
			{
				blok5.polozenie_x = ramie2.koniec_x - blok5.temp_x;
				blok5.polozenie_y = ramie2.koniec_y - blok5.temp_y;
			}
			if (blok6.poruszaj)
			{
				blok6.polozenie_x = ramie2.koniec_x - blok6.temp_x;
				blok6.polozenie_y = ramie2.koniec_y - blok6.temp_y;
			}
			blok1.start(pamiec_hdc, FIOLETOWY);
			blok2.start(pamiec_hdc, FIOLETOWY);
			blok3.start(pamiec_hdc, FIOLETOWY);
			blok4.start(pamiec_hdc, FIOLETOWY);
			blok5.start(pamiec_hdc, FIOLETOWY);
			blok6.start(pamiec_hdc, FIOLETOWY);

			BitBlt(hdc, 0, bez_ods, SZEROKOSC_OKNA.a, WYSOKOSC_OKNA.a, pamiec_hdc, 0, 0, SRCCOPY);

			DeleteObject(pamiec_bitmapa);
			DeleteDC(pamiec_hdc);
			DeleteDC(hdc);
            EndPaint(hWnd, &ps);
			break;
        }
		case WM_KEYDOWN:

			switch ((int)wParam)
			{
			case VK_LEFT:
				ramie1.predkosc_zmiany += 1;
				ramie1.nachylenie += 0.01 + (ramie1.predkosc_zmiany*0.005);
				InvalidateRect(hWnd, NULL, wymazywanie_tla);
				if (ruchy_ramienia.tryb == IDM_BSTART)
				{
					ruchy_ramienia.wczytaj_dane();
				}
				break;
			case VK_RIGHT:
				ramie1.predkosc_zmiany += 1;
				ramie1.nachylenie -= 0.01 + (ramie1.predkosc_zmiany*0.005);
				InvalidateRect(hWnd, NULL, wymazywanie_tla);
				if (ruchy_ramienia.tryb == IDM_BSTART)
				{
					ruchy_ramienia.wczytaj_dane();
				}
				break;
			case VK_UP:
				ramie2.predkosc_zmiany += 1;
				ramie2.nachylenie += 0.01 + (ramie2.predkosc_zmiany*0.005);
				InvalidateRect(hWnd, NULL, wymazywanie_tla);		// "odświeża" okno
				if (ruchy_ramienia.tryb == IDM_BSTART)
				{
					ruchy_ramienia.wczytaj_dane();
				}
				break;
			case VK_DOWN:
				ramie2.predkosc_zmiany += 1;
				ramie2.nachylenie -= 0.01 + (ramie2.predkosc_zmiany*0.005);
				InvalidateRect(hWnd, NULL, wymazywanie_tla);
				if (ruchy_ramienia.tryb == IDM_BSTART)
				{
					ruchy_ramienia.wczytaj_dane();
				}
				break;
			case VK_SPACE:
				char blok_ok[6];//przechowuje info na temat zwroconych danych z funkcji spr_pol

				blok_ok[0] = spr_pol(blok1.polozenie_x, blok1.polozenie_y, blok1.wielkosc, ramie2.koniec_x, ramie2.koniec_y);
				blok_ok[1] = spr_pol(blok2.polozenie_x, blok2.polozenie_y, blok2.wielkosc, ramie2.koniec_x, ramie2.koniec_y);
				blok_ok[2] = spr_pol(blok3.polozenie_x, blok3.polozenie_y, blok3.wielkosc, ramie2.koniec_x, ramie2.koniec_y);
				blok_ok[3] = spr_pol(blok4.polozenie_x, blok4.polozenie_y, blok4.wielkosc, ramie2.koniec_x, ramie2.koniec_y);
				blok_ok[4] = spr_pol(blok5.polozenie_x, blok5.polozenie_y, blok5.wielkosc, ramie2.koniec_x, ramie2.koniec_y);
				blok_ok[5] = spr_pol(blok6.polozenie_x, blok6.polozenie_y, blok6.wielkosc, ramie2.koniec_x, ramie2.koniec_y);

				if (blok_ok[0] == 1)//blok 1
				{
					if (blok1.poruszaj == FALSE)
					{
						blok1.poruszaj = TRUE;


						blok1.temp_x = ramie2.koniec_x - blok1.polozenie_x;			//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok1.temp_y = ramie2.koniec_y - blok1.polozenie_y;

						blok2.poruszaj = FALSE;
						blok3.poruszaj = FALSE;
						blok4.poruszaj = FALSE;
						blok5.poruszaj = FALSE;
						blok6.poruszaj = FALSE;
					}
					else blok1.poruszaj = FALSE;	//mozliwosc zmiany stanu chwytania przy nacisnieciu spacji
				}
				else if (blok_ok[1] == 1)//blok 2
				{
					if (blok2.poruszaj == FALSE)
					{
						blok2.poruszaj = TRUE;


						blok2.temp_x = ramie2.koniec_x - blok2.polozenie_x;		//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok2.temp_y = ramie2.koniec_y - blok2.polozenie_y;

						blok1.poruszaj = FALSE;
						blok3.poruszaj = FALSE;
						blok4.poruszaj = FALSE;
						blok5.poruszaj = FALSE;
						blok6.poruszaj = FALSE;
					}
					else blok2.poruszaj = FALSE;
				}
				else if (blok_ok[2] == 1)//blok 3
				{
					if (blok3.poruszaj == FALSE)
					{
						blok3.poruszaj = TRUE;


						blok3.temp_x = ramie2.koniec_x - blok3.polozenie_x;			//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok3.temp_y = ramie2.koniec_y - blok3.polozenie_y;

						blok1.poruszaj = FALSE;
						blok2.poruszaj = FALSE;
						blok4.poruszaj = FALSE;
						blok5.poruszaj = FALSE;
						blok6.poruszaj = FALSE;
					}
					else blok3.poruszaj = FALSE;
				}
				else if (blok_ok[3] == 1)//blok 4
				{
					if (blok4.poruszaj == FALSE)
					{
						blok4.poruszaj = TRUE;


						blok4.temp_x = ramie2.koniec_x - blok4.polozenie_x;		//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok4.temp_y = ramie2.koniec_y - blok4.polozenie_y;

						blok1.poruszaj = FALSE;
						blok2.poruszaj = FALSE;
						blok3.poruszaj = FALSE;
						blok5.poruszaj = FALSE;
						blok6.poruszaj = FALSE;
					}
					else blok4.poruszaj = FALSE;
				}
				else if (blok_ok[4] == 1)//blok 5
				{
					if (blok5.poruszaj == FALSE)
					{
						blok5.poruszaj = TRUE;


						blok5.temp_x = ramie2.koniec_x - blok5.polozenie_x;			//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok5.temp_y = ramie2.koniec_y - blok5.polozenie_y;

						blok1.poruszaj = FALSE;
						blok2.poruszaj = FALSE;
						blok3.poruszaj = FALSE;
						blok4.poruszaj = FALSE;
						blok6.poruszaj = FALSE;
					}
					else blok5.poruszaj = FALSE;
				}
				else if (blok_ok[5] == 1)//blok 6
				{
					if (blok6.poruszaj == FALSE)
					{
						blok6.poruszaj = TRUE;


						blok6.temp_x = ramie2.koniec_x - blok6.polozenie_x;			//zapisywanie odleglosci konca ramienia od poczatku bloku
						blok6.temp_y = ramie2.koniec_y - blok6.polozenie_y;

						blok1.poruszaj = FALSE;
						blok2.poruszaj = FALSE;
						blok3.poruszaj = FALSE;
						blok4.poruszaj = FALSE;
						blok5.poruszaj = FALSE;
					}
					else blok6.poruszaj = FALSE;
				}
				else
				{
					blok1.poruszaj = FALSE;
					blok2.poruszaj = FALSE;
					blok3.poruszaj = FALSE;
					blok4.poruszaj = FALSE;
					blok5.poruszaj = FALSE;
					blok6.poruszaj = FALSE;
				}

				break;
			}
			break;
		case WM_KEYUP:
			switch ((int)wParam)
			{
			case VK_RIGHT:
				ramie1.predkosc_zmiany = ramie1.domyslna_predkosc;
				break;
			case VK_UP:
				ramie2.predkosc_zmiany = ramie1.domyslna_predkosc;
				break;
			case  VK_LEFT:
				ramie1.predkosc_zmiany = ramie2.domyslna_predkosc;
				break;
			case VK_DOWN:
				ramie2.predkosc_zmiany = ramie2.domyslna_predkosc;
				break;

			}
			break;
		case WM_CREATE:
		{

			CreateWindowEx(NULL, TEXT("STATIC"), TEXT("Szybkość poruszania:"), WS_VISIBLE | WS_CHILD, 3, 0, 150, 20, hWnd, NULL, NULL, NULL);
			CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("0"), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 150, 0, 30, 20, hWnd, (HMENU)IDM_ESZYBKOSC, hInst, NULL);
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Zastosuj"), WS_VISIBLE | WS_CHILD, 180, 0, 100, 20, hWnd, (HMENU)IDM_BZASTOSUJ, NULL, NULL);

			CreateWindowEx(NULL, TEXT("STATIC"), TEXT("Tryb:"), WS_VISIBLE | WS_CHILD, 3, 20, 50, 20, hWnd, NULL, NULL, NULL);
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("MANUALNY"), WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON, 50, 20, 130, 20, hWnd, (HMENU)IDM_BMANUALNY, NULL, NULL);
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("AUTOMATYCZNY"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 180, 20, 130, 20, hWnd, (HMENU)IDM_BAUTOMATYCZNY, NULL, NULL);
			CheckRadioButton(hWnd, IDM_BMANUALNY, IDM_BAUTOMATYCZNY, IDM_BMANUALNY);//domyslnie ustaw zaznaczenie jako manualny

			S_odtwarzanie.hwnd = CreateWindowEx(NULL, TEXT("STATIC"), TEXT("Szybkość odtwarzania:"), WS_CHILD, 320, 20, 150, 20, hWnd, NULL, NULL, NULL);
			E_odtwarzanie.hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("25"), WS_CHILD | WS_BORDER | ES_NUMBER, 470, 20, 40, 20, hWnd, NULL, hInst, NULL);
			B_odtwarzanie.hwnd = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Zastosuj"), WS_CHILD, 520, 20, 100, 20, hWnd, (HMENU)IDM_BODTWARZANIE, NULL, NULL);

			nagrywanie.hwnd = CreateWindowEx(NULL, TEXT("STATIC"), TEXT("Nagrywanie ruchów:"), WS_CHILD, 3, 40, 150, 20, hWnd, (HMENU)IDM_SNAGRYWANIE, NULL, NULL);
			start.hwnd = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("START"), WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON, 150, 40, 80, 20, hWnd, (HMENU)IDM_BSTART, NULL, NULL);
			pause.hwnd = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("PAUSE"), WS_CHILD | BS_AUTORADIOBUTTON, 230, 40, 80, 20, hWnd, (HMENU)IDM_BPAUSE, NULL, NULL);
			reset.hwnd = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("RESET"), WS_CHILD | BS_AUTORADIOBUTTON, 310, 40, 80, 20, hWnd, (HMENU)IDM_BRESET, NULL, NULL);
			odtworz.hwnd = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("ODTWÓRZ"), WS_CHILD | BS_AUTORADIOBUTTON, 390, 40, 100, 20, hWnd, (HMENU)IDM_BODTWORZ, NULL, NULL);
			CheckRadioButton(hWnd, IDM_BSTART, IDM_BODTWORZ, IDM_BRESET);
			break;

		}
    case WM_DESTROY:
		GdiplusShutdown(m_gdiplusToken);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Procedura obsługi komunikatów dla okna informacji o programie.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
