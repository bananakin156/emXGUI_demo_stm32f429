#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H

#define FILE_NAME_LEN           100	
#define FILE_MAX_NUM            20
#define _DF1S	                 0x81
//控件ID
#define ID_LIST_1               0x2000



//#define ICON_VIEWER_ID_PREV     0x2001
//#define ICON_VIEWER_ID_NEXT	  0x2002
extern int Play_index;
extern char playlist[FILE_MAX_NUM][FILE_NAME_LEN];//播放List
extern int sw_flag;//切换标志
#endif


