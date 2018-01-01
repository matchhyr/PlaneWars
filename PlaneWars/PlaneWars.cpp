// PlaneWars.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "PlaneWars.h"


// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
												/* List all bmp resoures here */
UINT              g_uiBmpNames[] = { IDB_BK,
                                     IDB_SMALL,
                                     IDB_MIDDLE,
                                     IDB_BIG,
                                     IDB_LOGO,
                                     IDB_START,
								     IDB_GAMEOVER,
                                     IDB_RESTART,
									 IDB_EXIT
                                   };

HBITMAP           g_hBmp[BMPCOUNT];                /* Bmp resource handle        */

GAME              g_tGame;                         /* Game Info                  */
PLANETYPEINFO     g_tPlaneType[PLANETYPECOUNT];    /* Plane type info            */
PLANE             g_tPlane[PLANECOUNT];            /* Plane info                 */


// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID                InitGame();
VOID                DestroyGame();
VOID                UpdatePlaneInfo(BOOL bReset, UINT uiIndex, UINT uiSpeed);
VOID                DrawWindow(HDC hdc);
VOID                TimerProc(HWND hWnd);
VOID                LButtonDownProc(HWND hWnd, LPARAM lParam);
VOID                Hit(POINT ptMouse);
BOOL                CheckGameStartButtonDown(POINT ptMouse);
BOOL                CheckGameReStartButtonDown(POINT ptMouse);
BOOL                CheckGameExitButtonDown(POINT ptMouse);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANEWARS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANEWARS));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANEWARS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//����Ҫ�˵�
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   int cxScreen = GetSystemMetrics(SM_CXSCREEN); //����Ϊ��Ļ��size
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle,
	                        WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_SIZEBOX),
	                        (cxScreen - WNDWIDTH) / 2,
	                        (cyScreen - WNDHEIGHT ) / 2,
	                        WNDWIDTH,
	                        WNDHEIGHT+35, /*35 here is a workaround ����Ӧ��Ϊ�������ĳ���������WNDHEIGHT*/ 
	                        nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//


/* ���ڹ��̺��������˵�һ�����ڴ������յ���ͬ����Ϣʱ������ȡ�Ĳ�ͬ��Ӧ������Ҫ���ڴ����͸����ڵ���Ϣ��hwnd��Ҫ�����ڵľ����
   message����ϢID�������˲�ͬ����Ϣ���ͣ�wParam��lParam��������Ϣ�ĸ�����Ϣ��������Ϣ��������Ϣ���͵Ĳ�ͬ����ͬ��
   ��������Ϣ����ʱ��wParam��ֵΪ���°�����������룬lParam��洢���������״̬��Ϣ����ˣ�������ڶԴ���ļ�����Ϣ����ʱ��ֻ��Ҫ�ж�wParam��ֵ���ɡ�
   �������Ϣ����ʱ��wParamֵΪ��갴������Ϣ����lParam�򴢴��������꣬���ֽڴ���y���꣬���ֽڴ���x���ꡣ��g_y =HIWORD(lParam), g_x = LOWORD(lParam)
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:  //��һ��Ӧ�ó���ͨ��CreateWindowEx��������CreateWindow�������󴴽�����ʱ���ʹ���Ϣ��(����Ϣ�ں�������֮ǰ����)
	{
		InitGame();
	}
	break;
    case WM_PAINT://��һ��WM_PAINT��Ϣ��ͨ��������WinMain�е���UpdateWindowʱ��ָʾ������Ϣ�����������ʾ�����ϻ�һЩ����
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
			DrawWindow(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_START:
		{
			memset(g_tPlane, 0, sizeof(g_tPlane));
			g_tGame.bigAdded = FALSE;
			g_tGame.status = RUN;
			g_tGame.score = 0;

			UpdatePlaneInfo(TRUE, 0, 3);

			/* Send WM_TIMER every 50ms */
			SetTimer(hWnd, TIMER, 50, NULL);
		}
		break;
	case WM_TIMER:
		{
			TimerProc(hWnd);
		}
	break;

	case WM_LBUTTONDOWN:
		{
			LButtonDownProc(hWnd, lParam);
		}
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		/* Analyze menu cases */
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
    case WM_DESTROY:
		{	DestroyGame();
			PostQuitMessage(0);
		}
	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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


/* Below are user defined functions */

/******************************************************
* Function name:  InitGame()
* Purpose:        Global variables initialization
* Input:          N/A
* Output:         N/A
* Return:         N/A
*****************************************************/

VOID InitGame() {
	BITMAP bmp;

	//Load bitmap resource
	for (int i = 0; i < BMPCOUNT; i++) {
		g_hBmp[i] = LoadBitmap(hInst, MAKEINTRESOURCE(g_uiBmpNames[i]));
    }

	//Init global variables
	memset(&g_tGame, 0, sizeof(g_tGame));// ���ڴ�ȫ����0

	g_tGame.status = WELCOME;
	g_tGame.boardSize.cx = WNDWIDTH;
	g_tGame.boardSize.cy = WNDHEIGHT;
	g_tGame.planeNum = PLANECOUNT;
	g_tGame.planeTypeNum = PLANETYPECOUNT;

	memset(&g_tPlaneType, 0, sizeof(BITMAP));

	//����ͬ����ķɻ�����װ��ṹ��
	GetObject(g_hBmp[1], sizeof(BITMAP), &bmp);
	g_tPlaneType[SMALL].type = SMALL;
	g_tPlaneType[SMALL].size.cx = bmp.bmWidth; 
	g_tPlaneType[SMALL].size.cy = bmp.bmHeight/2;// ������ͼƬ��һ��BMP�һ��ֻ��ʾһ����
	g_tPlaneType[SMALL].maxHC = 1;

	GetObject(g_hBmp[2], sizeof(BITMAP), &bmp);
	g_tPlaneType[MIDDLE].type = MIDDLE;
	g_tPlaneType[MIDDLE].size.cx = bmp.bmWidth;
	g_tPlaneType[MIDDLE].size.cy = bmp.bmHeight / 3; // ����ͼƬ
	g_tPlaneType[MIDDLE].maxHC = 2;

	GetObject(g_hBmp[3], sizeof(BITMAP), &bmp);
	g_tPlaneType[BIG].type = BIG;
	g_tPlaneType[BIG].size.cx = bmp.bmWidth;
	g_tPlaneType[BIG].size.cy = bmp.bmHeight / 4;   // �ĸ�ͼƬ
	g_tPlaneType[BIG].maxHC = 3;
}

/******************************************************
* Function name:  DestroyGame()
* Purpose:        Resource release
* Input:          N/A
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID DestroyGame()
{
	// Release bitmap resource
	for (int i = 0; i< BMPCOUNT; i++)
	{
		DeleteObject(g_hBmp[i]);
	}
}

/******************************************************
* Function name:  DrawWindow()
* Purpose:        Draw game window
* Input:          hdc
* Output:         N/A
* Return:         N/A
* Description:    The game window has 3 different pages.
*                 1. Welcome page
*                 Content: background + logo + start button
*                 2. Run page
*                 Content: background + planes
*                 3. Gameover page
*                 Content: background + restart button + exit button + score
*****************************************************/

VOID DrawWindow(HDC hdc) 
{
	HDC hdcMem, hdcTmp;
	HBITMAP hBmpMem;
	BITMAP  bmp;
	UINT i;
	HFONT hf;                //�߼����������������,�洢��HFONT������
	LOGFONT lf;              //��������һ����ΪLOGFONT�Ľṹ����ʾ
	RECT       rt;           // RECT���� ���ϽǺ����½�����
	TCHAR      strScore[10];/*��û�ж���_UNICODE��ʱ��TCHAR = char��_tcslen =strlen
	                         ��������_UNICODE��ʱ��TCHAR = wchar_t �� _tcslen = wcslen*/
	int        type;

	/* Use Double Buffering method to paint */

	/* hBmpMem is a tmp bitmap, used to store all kinds of things */
	hBmpMem = CreateCompatibleBitmap(hdc, g_tGame.boardSize.cx, g_tGame.boardSize.cy);
	/* hdcMem is tmp dc in memory corresponding to hBmpMem */
	hdcMem = CreateCompatibleDC(hdc);//�ú�������һ����ָ���豸���ݵ��ڴ��豸�����Ļ�����DC����

	SelectObject(hdcMem, hBmpMem);//�ú���ѡ��һ����ָ�����豸�����Ļ�����


	/* hdcTmp is another tmp dc, used to store widgets, like background, plane, button,etc. */
	hdcTmp = CreateCompatibleDC(hdc);


	/* Draw background */
	SelectObject(hdcTmp, g_hBmp[0]);
	BitBlt(hdcMem, 0, 0, g_tGame.boardSize.cx, g_tGame.boardSize.cy,
		hdcTmp, 0, 0, SRCCOPY);
	/* �ú�����ָ����Դ�豸���������е����ؽ���λ�飨bit_block��ת�����Դ��͵�Ŀ���豸����*/

	switch (g_tGame.status)
	{
	case WELCOME:
		{
			/*Draw logo*/
			SelectObject(hdcTmp, g_hBmp[4]);
			GetObject(g_hBmp[4], sizeof(BITMAP), &bmp);

			TransparentBlt(
				hdcMem,
				30, 150, bmp.bmWidth, bmp.bmHeight,
				hdcTmp,
				0, 0, bmp.bmWidth, bmp.bmHeight,
				RGB(255, 255, 255));              //͸����ͼ

			/* Draw start button */
			SelectObject(hdcTmp, g_hBmp[5]);
			GetObject(g_hBmp[5], sizeof(BITMAP), &bmp);

			TransparentBlt(
				hdcMem,
				120, 350, bmp.bmWidth, bmp.bmHeight,
				hdcTmp,
				0, 0, bmp.bmWidth, bmp.bmHeight,
				RGB(255, 255, 255));
		}
		break;
	case RUN:
		{
			/* Draw planes */
			for (i = 0; i < g_tGame.planeNum; i++)	{
				type = g_tPlane[i].type;

				switch (type)
			{
				case SMALL:
				SelectObject(hdcTmp, g_hBmp[1]);
					break;
				case MIDDLE:
				SelectObject(hdcTmp, g_hBmp[2]);
					break;
				case BIG:
				SelectObject(hdcTmp, g_hBmp[3]);
					break;
				default:
					assert(0);
					break;

			}
			/*����Plane��Ϣ��ͼ*/
			TransparentBlt(
				hdcMem, g_tPlane[i].pos.x, g_tPlane[i].pos.y,
				g_tPlaneType[type].size.cx, g_tPlaneType[type].size.cy,
				hdcTmp, 0, g_tPlane[i].hc * g_tPlaneType[type].size.cy,
				g_tPlaneType[type].size.cx, g_tPlaneType[type].size.cy,
				RGB(255, 255, 255));
			if (g_tPlane[i].hc == g_tPlaneType[type].maxHC)
				{
				g_tPlane[i].hc = -1;
				}
			}
		}
		break;
	case GAMEOVER:
		{
			/* Draw game over pic */
			SelectObject(hdcTmp, g_hBmp[6]);
			GetObject(g_hBmp[6], sizeof(BITMAP), &bmp);

			TransparentBlt(
				hdcMem,
				(g_tGame.boardSize.cx - bmp.bmWidth) / 2, (g_tGame.boardSize.cy - bmp.bmHeight) / 2,
				bmp.bmWidth, bmp.bmHeight,
				hdcTmp,
				0, 0,
				bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
			/* Draw restart button */
			SelectObject(hdcTmp, g_hBmp[7]);
			GetObject(g_hBmp[7], sizeof(BITMAP), &bmp);

			TransparentBlt(
				hdcMem,
				100, 270, bmp.bmWidth, bmp.bmHeight,
				hdcTmp,
				0, 0, bmp.bmWidth, bmp.bmHeight,
				RGB(255, 255, 255));
			
			
			/* Draw exit button */
			SelectObject(hdcTmp, g_hBmp[8]);
			GetObject(g_hBmp[8], sizeof(BITMAP), &bmp);

			TransparentBlt(
				hdcMem,
				100, 310, bmp.bmWidth, bmp.bmHeight,
				hdcTmp,
				0, 0, bmp.bmWidth, bmp.bmHeight,
				RGB(255, 255, 255));

			/* Draw score */
			lf.lfHeight = 25;                   //�趨�������
			lf.lfWidth = 25;
			lf.lfEscapement = 0;
			lf.lfItalic = FALSE;
			lf.lfUnderline = FALSE;
			lf.lfStrikeOut = FALSE;
			lf.lfCharSet = CHINESEBIG5_CHARSET;

			hf = CreateFontIndirect(&lf);      //�����߼�����
			SelectObject(hdcMem, hf);

			rt.left = 150;                    //�趨һ������
			rt.top = 210;
			rt.right = 300;
			rt.bottom = 260;

			_itot_s(g_tGame.score, strScore, 10); //������ת��Ϊ�ַ�
			/*itot���Ը��ݵ�ǰ��Ŀ�ǲ���unicode��Ŀ�Ӷ��Զ����itow����itoa
             *����Ӹ�s��һ��Ҫ��һ����������֤���鲻Խ�磬�Ǹ�s��safe����˼*/

			SetBkMode(hdcMem, TRANSPARENT);//����ָ��DC�ı������ģʽ,transparent or opaque

			DrawText(hdcMem, strScore, -1, &rt, DT_CENTER);//�ú�����ָ���ľ�����д���ʽ�������ģ�����ָ���ķ��������ĸ�ʽ��
		}
		break;
	default:
		break;
	}

	/* Blit everything onto screen  */
	BitBlt(hdc, 0, 0, g_tGame.boardSize.cx, g_tGame.boardSize.cy, hdcMem, 0, 0, SRCCOPY);


	DeleteObject(hBmpMem); //�ͷſռ�
	DeleteDC(hdcMem);
	DeleteDC(hdcTmp);
}



/******************************************************
* Function name:  LButtonDownProc()
* Purpose:        Handle function to mouse left button down message
* Input:
*    hWnd
*    lParam
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID LButtonDownProc(HWND hWnd, LPARAM lParam)
{
	UINT index = 0;

	/* Mouse click coordinate */
	POINT  ptMouse;
	ptMouse.y = HIWORD(lParam);
	ptMouse.x = LOWORD(lParam);

	switch (g_tGame.status)
	{
	case WELCOME:
		{
			/* Check if game start button is clicked */
			if (CheckGameStartButtonDown(ptMouse))
			{
			SendMessage(hWnd, WM_START, 0, 0);
			}
		}
		break;
	case RUN:
		{
		/* Check if a plane is hit */
		Hit(ptMouse);
		}
		break;
	case GAMEOVER:
		{
			/* Check if game restart button is clicked */
			if (CheckGameReStartButtonDown(ptMouse))
			{
			SendMessage(hWnd, WM_START, 0, 0);
			}
			/* Check if game restart button is clicked */
			if (CheckGameExitButtonDown(ptMouse))
			{
				SendMessage(hWnd, WM_CLOSE, 0, 0); //����WM_CLOSE������DestroyWindow()
				                                   //DestroyWindow()�ַ���WM_DESTROY
			}
		}
		break;
	default:
		break;
	}
}


/******************************************************
* Function name:  Hit()
* Purpose:        Check if a plane is hit
* Input:          ptMouse
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID Hit(POINT ptMouse)
{
	RECT rc;
	int type;

	for (UINT i = 0; i < g_tGame.planeNum; i++)
	{
		type = g_tPlane[i].type;
		rc.left = g_tPlane[i].pos.x;
		rc.top = g_tPlane[i].pos.y;
		rc.right =rc.left + g_tPlaneType[type].size.cx;
		rc.bottom =rc.top + g_tPlaneType[type].size.cy;


		if (PtInRect(&rc,ptMouse))
		{
			g_tPlane[i].hc++;
			if (g_tPlane[i].hc > g_tPlaneType[type].maxHC) //��֤plane.hc<maxHC
			{
				g_tPlane[i].hc = g_tPlaneType[type].maxHC;
			}
			/* Play crash music */
			/*PlaySound��Windows���ڲ������ֵ�API����(����)����vs2010���ϰ汾��Ҫ����#pragma comment(lib, "winmm.lib")����ʹ��PlaySound��*/
			PlaySound(
				MAKEINTRESOURCE(IDR_WAVE1),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC);

			break; //�ҵ�Ψһȷ���ķɻ��������˳�ѭ��
		}
	}
}


/******************************************************
* Function name:  UpdatePlaneInfo()
* Purpose:        Update Planes detailed info in g_tPlane[]
* Input:
*    bReset:      If TRUE,update all planes info, if FALSE, update designated plane info
*    uiIndex:     Index of plane needs to update. Only valid when bReset is FALSE.
*    uiSpeed:     Move speed
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID UpdatePlaneInfo(BOOL bReset, UINT uiIndex, UINT uiSpeed)
{
	UINT i, idx, begin, end;
	assert(uiIndex < g_tGame.planeNum);

	if (bReset)
	{
		begin = 0;
		end = g_tGame.planeNum;
	}
	else
	{
		begin = uiIndex;
		end = uiIndex + 1;
	}
	srand(GetTickCount());

	for (i = begin; i < end; i++)
	{
		/* idx is a random value: 0 - small, 1 - middle, 2 - big */
		idx = rand() % 3;

		/* Ensure there is only one big plane, idx == 2 only once */
		while ((idx == 2) && g_tGame.bigAdded)
		{
			idx = rand() % 3;
		}

		if (idx == 2)
			g_tGame.bigAdded = TRUE;

		g_tPlane[i].type = (PLANETYPE)idx;
		g_tPlane[i].pos.x = rand() % (g_tGame.boardSize.cx - (g_tPlaneType[idx].size.cx+10));// 10  is a workaround
		/* Initial Y position is outside the client window */
		g_tPlane[i].pos.y = -g_tPlaneType[idx].size.cy - rand() % (g_tPlaneType[idx].size.cy*(4-idx));
		g_tPlane[i].speed = uiSpeed - idx;
	}
}
/******************************************************
* Function name:  TimerProc()
* Purpose:        Handle function to timer message
* Input:          hWnd
* Output:         N/A
* Return:         N/A
* Description:    Change plane Y postion and check if reaches to the bottom
*****************************************************/
VOID TimerProc(HWND hWnd)
{
	int type, newspeed;
	for (UINT i = 0; i < g_tGame.planeNum; i++)
	{
		type = g_tPlane[i].type;

		/* After crash picture is showed, update score and plane info in crash postion */
		if (g_tPlane[i].hc == -1)
		{
			g_tGame.score++;

			g_tPlane[i].hc = 0;

			if (type == BIG)
			{
				g_tGame.bigAdded = FALSE;
			}
			newspeed = (g_tGame.score > 50) ? (g_tGame.score / 50 + 3) : 3;//ÿ50�����ӷɻ����ٶ�
			UpdatePlaneInfo(FALSE, i, newspeed);
		}
		/* Move down ��move speed every 50 ms */
		g_tPlane[i].pos.y += g_tPlane[i].speed;

		if (g_tPlane[i].pos.y + g_tPlaneType[type].size.cy >= g_tGame.boardSize.cy)//�ɻ�������ײ�
		{
			KillTimer(hWnd, TIMER);

			g_tGame.status = GAMEOVER;

			break;
		}
	}
	/* Invalidate window rectangle region, generate WM_PAINT */
	InvalidateRect(hWnd, NULL, FALSE); //InvalidateRect�ǽ������е�һ����������עΪ����Ч����ϵͳ�᲻���򴰿ڷ���WM_PAINT��Ϣ�����ػ�
}





BOOL CheckGameStartButtonDown(POINT ptMouse)
{
	RECT   rc;
	BITMAP bmp;

	GetObject(g_hBmp[5], sizeof(BITMAP), &bmp);

	rc.left = 120;
	rc.top = 350;
	rc.right = bmp.bmWidth + rc.left;
	rc.bottom = bmp.bmHeight + rc.top;

	return PtInRect(&rc, ptMouse);
}

BOOL CheckGameReStartButtonDown(POINT ptMouse)
{
	RECT   rc;
	BITMAP bmp;

	GetObject(g_hBmp[7], sizeof(BITMAP), &bmp);

	rc.left = 100;
	rc.top = 270;
	rc.right = bmp.bmWidth + rc.left;
	rc.bottom = bmp.bmHeight + rc.top;

	return PtInRect(&rc, ptMouse);
}

BOOL CheckGameExitButtonDown(POINT ptMouse)
{
	RECT   rc;
	BITMAP bmp;

	GetObject(g_hBmp[8], sizeof(BITMAP), &bmp);

	rc.left = 100;
	rc.top = 310;
	rc.right = bmp.bmWidth + rc.left;
	rc.bottom = bmp.bmHeight + rc.top;

	return PtInRect(&rc, ptMouse);
}

