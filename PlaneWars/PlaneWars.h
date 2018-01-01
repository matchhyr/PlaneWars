#pragma once
#pragma once

#include <assert.h>
#include <mmsystem.h>

#include "resource.h"

#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")



#define MAX_LOADSTRING 100         /* 默认的长度                       */
#define WM_START       (WM_USER+1) /* 自己定义的 STRAT 消息            */
#define TIMER          1           /* Timer ID                         */

#define WNDWIDTH       380         /* Game window width                */
#define WNDHEIGHT      550         /* Game window height               */
#define PLANECOUNT     10          /* Plane number shows in the window */
#define PLANETYPECOUNT 3           /* 3 types: small, middle, big      */
#define BMPCOUNT       9           /* 需要用到的BMP数量，数组的大小    */




/*        GAME结构体保存游戏的信息
 *        PLANETYPE保存不同类型飞机的参数  
 *        PLANE保存飞机的信息              
 */

/*       三个不同的游戏状态
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
