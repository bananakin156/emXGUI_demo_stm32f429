/*
 *GUI_DEMO_IconViewer.c
 *2018年3月17日下午7:41:26
 *
 */

#include "emXGUI.h"
 //#include	"rtthread.h"

#include "emXGUI_JPEG.h"
#include	"CListMenu.h"
#include "GUI_AppDef.h"


 /*============================================================================*/

#define	ID_EXIT		0x1000
//#define ICON_VIEWER_ID_PREV   0x1003
//#define	ICON_VIEWER_ID_NEXT		0x1004
#define	ID_RB1		0x1101
#define	ID_RB2		0x1102
#define	ID_RB3		0x1103
#define	ID_RB4		0x1104
#define	ID_RB5		0x1105
#define	ID_RB6		0x1106


//static int win_pos=0;
//static u8 hour,min,sec;
//static const WCHAR *pOEMText=NULL;
//static u8 List1_Col,List1_Row;

#define	ID_LIST_1 0x1000
#define ID_LIST_2 0x1001

//extern const char app_1[];

//static const void *pDefIcon = app_1;
//static const void *pIcon_app2 =app_2;

/*
*   应用程序的空回调函数
*/
static void dummy(HWND hwnd)
{

}

extern void App_LED_DIALOG(void);
extern void	GUI_App_Desktop(void);
extern void App_GUI_Tutorial_DEMO(void);
extern void	GUI_MUSICPLAYER_DIALOG(void);
extern void	GUI_VideoPlayer_DIALOG(void);
extern void GUI_AVIList_DIALOG(void);
extern void	GUI_LED_DIALOG(void);
extern void	GUI_Camera_DIALOG(void);
extern void	GUI_RES_WRITER_DIALOG(void);
extern void GUI_Boot_Interface_DIALOG(void);
int thread_ctrl = 1;
/* 视频播放器的应用 */
void GUI_VideoPlayerTest(void)
{
  while(thread_ctrl)
  {
      GUI_AVIList_DIALOG();
      GUI_VideoPlayer_DIALOG();
  }
  thread_ctrl = 1;
}


static const struct __obj_list menu_list_1[] = {
    //	L"Speed",		app_1, 		NULL,	 	RGB_WHITE,			dummy,
    //L"Hello",		app_1,		NULL, 	 	RGB_WHITE,			dummy,
    //L"Button",		app_1, 		NULL,	 	RGB_WHITE,			dummy,
    //L"Checkbox",		app_1, 		NULL,	 	RGB_WHITE,			dummy,
    //L"Radiobox",		app_1, 		NULL,	 	RGB_WHITE,			dummy,
    //L"Textbox",		app_1, 		NULL,	 	RGB_WHITE,			dummy,

      L"GUI应用",		NULL, 	L"J", 	RGB_WHITE,			GUI_App_Desktop,
      L"MP3播放器",		NULL,	  L"I", RGB_WHITE,				dummy,
      L"视频播放器",		NULL,	  L"D", RGB_WHITE,				dummy,

      L"RGB彩灯",		NULL,	  L"L", RGB_WHITE,				GUI_LED_DIALOG,
      L"摄像头",		NULL,	  L"M",RGB_WHITE, 				dummy,

      L"触摸画板",	NULL, 	L"C", RGB_WHITE,				dummy,
      L"温湿度",	NULL,   L"O", RGB_WHITE,				dummy,
      L"电压表",		NULL,	  L"W", RGB_WHITE,				dummy,  
      L"模拟U盘",	NULL,	  L"N", RGB_WHITE,				dummy, 
      L"陀螺仪",	  NULL,	  L"R", 	RGB_WHITE,			dummy,

      L"以太网",		NULL,	  L"Q", RGB_WHITE,				dummy,
      L"WiFi",		NULL,	  L"P", RGB_WHITE,				dummy,
      L"游戏",	NULL,	  L"S", RGB_WHITE,				dummy,

      L"电话",	NULL, 	L"T", RGB_WHITE,				dummy,
      L"短信",	NULL,   L"U", RGB_WHITE,				dummy,
      L"二维码",	NULL,	  L"V", RGB_WHITE,				dummy,


        L"时钟",		NULL,	  L"H", RGB_WHITE,				dummy,
        L"录音机",	  NULL,	  L"Y", 	RGB_WHITE,			dummy,
//        L"Checkbox",	NULL,	  L"J", RGB_WHITE,				dummy,
//            L"Checkbox",	NULL, 	L"D", RGB_WHITE,				dummy,
//        L"Radiobox",	NULL,   L"E", RGB_WHITE,				dummy,
//        L"Textbox",	NULL,	  L"F", RGB_WHITE,				dummy,

//        L"Speed",		NULL,	  L"G", RGB_WHITE,				dummy,
//        L"Hello",		NULL,	  L"H", RGB_WHITE,				dummy,
//        L"Button",	  NULL,	  L"I", 	RGB_WHITE,			dummy,
//        L"Checkbox",	NULL,	  L"J", RGB_WHITE,				dummy,

//        L"Checkbox",	NULL, 	L"D", RGB_WHITE,				dummy,
//        L"Radiobox",	NULL,   L"E", RGB_WHITE,				dummy,
//        L"Textbox",	NULL,	  L"F", RGB_WHITE,				dummy,

//        L"Speed",		NULL,	  L"G", RGB_WHITE,				dummy,
//        L"Hello",		NULL,	  L"H", RGB_WHITE,				dummy,
//        L"Button",	  NULL,	  L"I", 	RGB_WHITE,			dummy,
//        L"Checkbox",	NULL,	  L"J", RGB_WHITE,				dummy,
        L"FlashWriter",	  NULL,	  L"b", 	RGB_WHITE,			GUI_RES_WRITER_DIALOG,

        NULL,	NULL,	NULL,NULL, NULL,//结束标志!

};


static void button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
    HWND hwnd;
    HDC hdc;
    RECT rc;
    WCHAR wbuf[128];

    hwnd = ds->hwnd; //button的窗口句柄.
    hdc = ds->hDC;   //button的绘图上下文句柄.
    rc = ds->rc;     //button的绘制矩形区.

    SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
    FillRect(hdc, &rc); //用矩形填充背景

    if (IsWindowEnabled(hwnd) == FALSE)
    {
        SetTextColor(hdc, MapRGB(hdc, COLOR_INVALID));
    }
    else if (ds->State & BST_PUSHED)
    { //按钮是按下状态
  //    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
  //		SetBrushColor(hdc,MapRGB(hdc,150,200,250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
  //		SetPenColor(hdc,MapRGB(hdc,250,0,0));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
        SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
    }
    else
    { //按钮是弹起状态
  //		SetBrushColor(hdc,MapRGB(hdc,255,255,255));
  //		SetPenColor(hdc,MapRGB(hdc,0,250,0));
        SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
    }


    //	SetBrushColor(hdc,COLOR_BACK_GROUND);

    //	FillRect(hdc,&rc); //用矩形填充背景
    //	DrawRect(hdc,&rc); //画矩形外框
    //  
    //  FillCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //用矩形填充背景FillCircle
    //	DrawCircle(hdc,rc.x+rc.w/2,rc.x+rc.w/2,rc.w/2); //画矩形外框

      /* 使用控制图标字体 */
    SetFont(hdc, controlFont_64);
    //  SetTextColor(hdc,MapRGB(hdc,255,255,255));

    GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字

    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)


    /* 恢复默认字体 */
    SetFont(hdc, defaultFont);

}


static	LRESULT	WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND wnd;
    switch (msg)
    {
    case WM_CREATE:
    {
        list_menu_cfg_t cfg;
        RECT rc;
        HWND chwnd;

        //			win_pos =0;
        //			GetTime(&hour,&min,&sec);

        GetClientRect(hwnd, &rc);

        //			rc.x =0;
        //			rc.y =0;
        //			rc.w =rc0.w;
        //			rc.h =200;

              //ListMenu控件，需要在创建时传入一个 list_menu_cfg_t 的结构体参数.
        cfg.list_objs = menu_list_1; //指定list列表.
        cfg.x_num = 4; //水平项数.
        cfg.y_num = 3; //垂直项数.
        cfg.bg_color = COLOR_DESKTOP_BACK_GROUND_HEX;

        chwnd = CreateWindow(&wcex_ListMenu,
                                L"ListMenu1",
                                WS_VISIBLE | LMS_PAGEMOVE,
                                rc.x + 60, rc.y + 20, rc.w - 120, rc.h - 10,
                                hwnd,
                                ID_LIST_1,
                                NULL,
                                &cfg);                                

        ///* 上一步按钮 */
        wnd = CreateWindow(BUTTON, L"L", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
            0, (rc.h - 30) / 2, 70, 70, hwnd, ICON_VIEWER_ID_PREV, NULL, NULL);
        SetWindowFont(wnd, controlFont_64); //设置控件窗口字体.

         /* 下一步按钮 */
        wnd = CreateWindow(BUTTON, L"K", BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
            rc.w - 65, (rc.h - 30) / 2, 70, 70, hwnd, ICON_VIEWER_ID_NEXT, NULL, NULL);
        SetWindowFont(wnd, controlFont_64); //设置控件窗口字体.

        SetTimer(hwnd, 1, 50, TMR_START, NULL);
    }
    break;
    ////

    case WM_NOTIFY:
    {
        u16 code, id;
        LM_NMHDR *nm;

        code = HIWORD(wParam);
        id = LOWORD(wParam);

        nm = (LM_NMHDR*)lParam;

        if (code == LMN_CLICKED)
        {
            switch (id)
            {
            case ID_LIST_1:
                menu_list_1[nm->idx].cbStartup(hwnd);
                break;
                ////
            case ID_LIST_2:
                //					menu_list_2[nm->idx].cbStartup(hwnd);
                break;
                ////
            }

        }
        ////
        if (code == BN_CLICKED && id == ICON_VIEWER_ID_PREV)
        {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_PREV, TRUE, 0);
        }
        ////
        if (code == BN_CLICKED && id == ICON_VIEWER_ID_NEXT)
        {
            SendMessage(GetDlgItem(hwnd, ID_LIST_1), MSG_MOVE_NEXT, TRUE, 0);
        }

    }
    break;
    ////

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;

        GetClientRect(hwnd, &rc);
        SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
        FillRect(hdc, &rc);
    }
    break;

    case WM_PAINT:
    {
        //			HDC hdc;
        PAINTSTRUCT ps;
        //			RECT rc;
        //			WCHAR wbuf[128];

        BeginPaint(hwnd, &ps);

        EndPaint(hwnd, &ps);
        ////

    }
    break;
    ////
#if 1    
    case	WM_DRAWITEM:
    {
        /*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
         *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
         *  该指针成员包含了一些控件绘制相关的参数.
         */

        DRAWITEM_HDR *ds;

        ds = (DRAWITEM_HDR*)lParam;

        button_owner_draw(ds); //执行自绘制按钮

      //			if(ds->ID == ICON_VIEWER_ID_PREV)
      //			{
      //				button_owner_draw(ds); //执行自绘制按钮
      //			}
      //      else if(ds->ID == ICON_VIEWER_ID_NEXT)
      //      {
      //        button_owner_draw(ds); //执行自绘制按钮
      //      }

         /* 返回TRUE表明使用重绘操作 */
        return TRUE;
    }
    //		break;
#else


    case WM_CTLCOLOR:
    {
        u16 id;

        id = LOWORD(wParam);

        if (id == ICON_VIEWER_ID_PREV || id == ICON_VIEWER_ID_NEXT)
        {
            CTLCOLOR *cr;
            cr = (CTLCOLOR*)lParam;

            if (IsWindowEnabled(GetDlgItem(hwnd, id)) == FALSE)
            {
                cr->TextColor = RGB888_COLOR_INVALID;
                cr->BackColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
                cr->BorderColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
                GUI_DEBUG("disable");
                //          cr->ForeColor = RGB888(255,255,255);
            }
            else if (SendMessage(GetDlgItem(hwnd, id), BM_GETSTATE, 0, 0)&BST_PUSHED)
            {
                cr->TextColor = RGB888(255, 0, 0);
                cr->BackColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
                cr->BorderColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
            }
            else
            {
                cr->TextColor = RGB888(255, 255, 255);
                cr->BackColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
                cr->BorderColor = RGB888_COLOR_DESKTOP_BACK_GROUND;
            }
            return TRUE;
        }
        else
        {
            //其它按钮使用系统默认的颜色进行绘制，所以直接返回FALSE.
            return FALSE;
        }
    }
#endif
    //    break;

    case WM_LBUTTONDOWN:
    {
    }
    break;
    ////

    case WM_TIMER:
    {
    }
    break;
    ////

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);


    }
    return WM_NULL;

}


/*============================================================================*/

void	GUI_Board_App_Desktop(void)
//static void	AppMain(void)
{
    HWND	hwnd;
    WNDCLASS	wcex;
    MSG msg;

    /////
    wcex.Tag = WNDCLASS_TAG;

    wcex.Style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = NULL;//hInst;
    wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
    wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

    //创建主窗口
    hwnd = CreateWindowEx(NULL,
        &wcex,
        _T("IconViewer"),
        //								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
        /*WS_MEMSURFACE|*/WS_CLIPCHILDREN,

        0, 0, GUI_XSIZE, 400,
        GetDesktopWindow(), NULL, NULL, NULL);

    //显示主窗口
    ShowWindow(hwnd, SW_SHOW);

    //开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
    while (GetMessage(&msg, hwnd))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


/*============================================================================*/
