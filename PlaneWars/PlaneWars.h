#pragma once
#pragma once

#include <assert.h>
#include <mmsystem.h>

#include "resource.h"

#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")



#define MAX_LOADSTRING 100         /* Ĭ�ϵĳ���                       */
#define WM_START       (WM_USER+1) /* �Լ������ STRAT ��Ϣ            */
#define TIMER          1           /* Timer ID                         */

#define WNDWIDTH       380         /* Game window width                */
#define WNDHEIGHT      550         /* Game window height               */
#define PLANECOUNT     10          /* Plane number shows in the window */
#define PLANETYPECOUNT 3           /* 3 types: small, middle, big      */
#define BMPCOUNT       9           /* ��Ҫ�õ���BMP����������Ĵ�С    */




/*        GAME�ṹ�屣����Ϸ����Ϣ
 *        PLANETYPE���治ͬ���ͷɻ��Ĳ���  
 *        PLANE����ɻ�����Ϣ              
 */

/*       ������ͬ����Ϸ״̬
*    WELCOME:  show logo, start button
*    RUN:      show planes
*    GAMEOVER: show score, restart button, exit buttion
*/
typedef enum
{
	WELCOME,
	RUN,
	GAMEOVER
} GAMESTATUS;

/* Game info */
typedef struct {
	GAMESTATUS status;        /* Game status           */
	SIZE       boardSize;     /* Game window size      */
	UINT       planeTypeNum;  /* Plane type number     */
	UINT       planeNum;      /* Plane number          */
	UINT       score;         /* Game score            */
	BOOL       bigAdded;      /* If big plane is added */
} GAME;


/* 3 different types plane */
typedef enum
{
	SMALL,
	MIDDLE,
	BIG
}PLANETYPE;

/* Plane type info */
typedef struct {
	PLANETYPE type;    /* Plane type      */
	SIZE      size;    /* Plane size      */
	int       maxHC;   /* max hit counter */
} PLANETYPEINFO;

/* Plane info */
typedef struct {
	PLANETYPE type;    /* Plane type      */
	POINT     pos;     /* plane postion   */
	int       hc;      /* hit counter     */
	UINT      speed;   /* move speed      */
} PLANE;
