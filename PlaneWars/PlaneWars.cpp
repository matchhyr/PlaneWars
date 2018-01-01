// PlaneWars.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PlaneWars.h"


// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
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


// 此代码模块中包含的函数的前向声明: 
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

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANEWARS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANEWARS));

    MSG msg;

    // 主消息循环: 
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
	//不需要菜单
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   int cxScreen = GetSystemMetrics(SM_CXSCREEN); //返回为屏幕的size
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle,
	                        WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_SIZEBOX),
	                        (cxScreen - WNDWIDTH) / 2,
	                        (cyScreen - WNDHEIGHT ) / 2,
	                        WNDWIDTH,
	                        WNDHEIGHT+35, /*35 here is a workaround 这里应该为标题栏的长度算入了WNDHEIGHT*/ 
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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//


/* 窗口过程函数决定了当一个窗口从外界接收到不同的信息时，所采取的不同反应，即主要用于处理发送给窗口的信息。hwnd是要处理窗口的句柄；
   message是消息ID，代表了不同的消息类型；wParam和lParam代表了消息的附加信息，附加信息会随着消息类型的不同而不同。
   当键盘消息发出时，wParam的值为按下按键的虚拟键码，lParam则存储按键的相关状态信息。因此，如果窗口对传入的键盘消息处理时，只需要判断wParam的值即可。
   当鼠标消息发出时，wParam值为鼠标按键的信息，而lParam则储存鼠标的坐标，高字节代表y坐标，低字节代表x坐标。即g_y =HIWORD(lParam), g_x = LOWORD(lParam)
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:  //当一个应用程序通过CreateWindowEx函数或者CreateWindow函数请求创建窗口时发送此消息，(此消息在函数返回之前发送)
	{
		InitGame();
	}
	break;
    case WM_PAINT://第一条WM_PAINT消息（通常发生在WinMain中调用UpdateWindow时）指示窗口消息处理程序在显示区域上画一些东西
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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
	memset(&g_tGame, 0, sizeof(g_tGame));// 将内存全部充0

	g_tGame.status = WELCOME;
	g_tGame.boardSize.cx = WNDWIDTH;
	g_tGame.boardSize.cy = WNDHEIGHT;
	g_tGame.planeNum = PLANECOUNT;
	g_tGame.planeTypeNum = PLANETYPECOUNT;

	memset(&g_tPlaneType, 0, sizeof(BITMAP));

	//将不同种类的飞机参数装入结构体
	GetObject(g_hBmp[1], sizeof(BITMAP), &bmp);
	g_tPlaneType[SMALL].type = SMALL;
	g_tPlaneType[SMALL].size.cx = bmp.bmWidth; 
	g_tPlaneType[SMALL].size.cy = bmp.bmHeight/2;// 有两个图片在一个BMP里，一次只显示一部分
	g_tPlaneType[SMALL].maxHC = 1;

	GetObject(g_hBmp[2], sizeof(BITMAP), &bmp);
	g_tPlaneType[MIDDLE].type = MIDDLE;
	g_tPlaneType[MIDDLE].size.cx = bmp.bmWidth;
	g_tPlaneType[MIDDLE].size.cy = bmp.bmHeight / 3; // 三个图片
	g_tPlaneType[MIDDLE].maxHC = 2;

	GetObject(g_hBmp[3], sizeof(BITMAP), &bmp);
	g_tPlaneType[BIG].type = BIG;
	g_tPlaneType[BIG].size.cx = bmp.bmWidth;
	g_tPlaneType[BIG].size.cy = bmp.bmHeight / 4;   // 四个图片
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
	HFONT hf;                //逻辑字体是字体的描述,存储在HFONT类型中
	LOGFONT lf;              //字体是以一个名为LOGFONT的结构来表示
	RECT       rt;           // RECT矩形 左上角和右下角坐标
	TCHAR      strScore[10];/*当没有定义_UNICODE宏时，TCHAR = char，_tcslen =strlen
	                         当定义了_UNICODE宏时，TCHAR = wchar_t ， _tcslen = wcslen*/
	int        type;

	/* Use Double Buffering method to paint */

	/* hBmpMem is a tmp bitmap, used to store all kinds of things */
	hBmpMem = CreateCompatibleBitmap(hdc, g_tGame.boardSize.cx, g_tGame.boardSize.cy);
	/* hdcMem is tmp dc in memory corresponding to hBmpMem */
	hdcMem = CreateCompatibleDC(hdc);//该函数创建一个与指定设备兼容的内存设备上下文环境（DC）。

	SelectObject(hdcMem, hBmpMem);//该函数选择一对象到指定的设备上下文环境中


	/* hdcTmp is another tmp dc, used to store widgets, like background, plane, button,etc. */
	hdcTmp = CreateCompatibleDC(hdc);


	/* Draw background */
	SelectObject(hdcTmp, g_hBmp[0]);
	BitBlt(hdcMem, 0, 0, g_tGame.boardSize.cx, g_tGame.boardSize.cy,
		hdcTmp, 0, 0, SRCCOPY);
	/* 该函数对指定的源设备环境区域中的像素进行位块（bit_block）转换，以传送到目标设备环境*/

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
				RGB(255, 255, 255));              //透明贴图

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
			/*根据Plane信息贴图*/
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
			lf.lfHeight = 25;                   //设定字体参数
			lf.lfWidth = 25;
			lf.lfEscapement = 0;
			lf.lfItalic = FALSE;
			lf.lfUnderline = FALSE;
			lf.lfStrikeOut = FALSE;
			lf.lfCharSet = CHINESEBIG5_CHARSET;

			hf = CreateFontIndirect(&lf);      //创建逻辑字体
			SelectObject(hdcMem, hf);

			rt.left = 150;                    //设定一个矩形
			rt.top = 210;
			rt.right = 300;
			rt.bottom = 260;

			_itot_s(g_tGame.score, strScore, 10); //将数字转换为字符
			/*itot可以根据当前项目是不是unicode项目从而自动变成itow或者itoa
             *后面加个s的一般要多一个参数，保证数组不越界，那个s是safe的意思*/

			SetBkMode(hdcMem, TRANSPARENT);//设置指定DC的背景混合模式,transparent or opaque

			DrawText(hdcMem, strScore, -1, &rt, DT_CENTER);//该函数在指定的矩形里写入格式化的正文，根据指定的方法对正文格式化
		}
		break;
	default:
		break;
	}

	/* Blit everything onto screen  */
	BitBlt(hdc, 0, 0, g_tGame.boardSize.cx, g_tGame.boardSize.cy, hdcMem, 0, 0, SRCCOPY);


	DeleteObject(hBmpMem); //释放空间
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
				SendMessage(hWnd, WM_CLOSE, 0, 0); //　　WM_CLOSE，调用DestroyWindow()
				                                   //DestroyWindow()又发送WM_DESTROY
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
			if (g_tPlane[i].hc > g_tPlaneType[type].maxHC) //保证plane.hc<maxHC
			{
				g_tPlane[i].hc = g_tPlaneType[type].maxHC;
			}
			/* Play crash music */
			/*PlaySound是Windows用于播放音乐的API函数(方法)。在vs2010以上版本需要加入#pragma comment(lib, "winmm.lib")才能使用PlaySound。*/
			PlaySound(
				MAKEINTRESOURCE(IDR_WAVE1),
				GetModuleHandle(NULL),
				SND_RESOURCE | SND_ASYNC);

			break; //找到唯一确定的飞机后立即退出循环
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
			newspeed = (g_tGame.score > 50) ? (g_tGame.score / 50 + 3) : 3;//每50分增加飞机的速度
			UpdatePlaneInfo(FALSE, i, newspeed);
		}
		/* Move down ，move speed every 50 ms */
		g_tPlane[i].pos.y += g_tPlane[i].speed;

		if (g_tPlane[i].pos.y + g_tPlaneType[type].size.cy >= g_tGame.boardSize.cy)//飞机到达最底部
		{
			KillTimer(hWnd, TIMER);

			g_tGame.status = GAMEOVER;

			break;
		}
	}
	/* Invalidate window rectangle region, generate WM_PAINT */
	InvalidateRect(hWnd, NULL, FALSE); //InvalidateRect是将窗口中的一块矩形区域标注为“无效”，系统会不断向窗口发送WM_PAINT消息令其重绘
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

