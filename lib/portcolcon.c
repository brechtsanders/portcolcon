#include "portcolcon.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#define CONSOLE_COLORS_TO_WINDOWS(fg,bg) (((fg) & 0x0F) + (((bg) & 0x0F) << 4))
static const unsigned char toansibaselookup[8] = {0, 4, 2, 6, 1, 5, 3, 7};
#define CONSOLE_COLORS_TO_ANSI_FOREGROUND(color) ((((color) & 0x08) == 0 ? 30 : 90) + toansibaselookup[(color) & 0x07])
#define CONSOLE_COLORS_TO_ANSI_BACKGROUND(color) ((((color) & 0x08) == 0 ? 40 : 100) + toansibaselookup[(color) & 0x07])

struct portcolcon_struct {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  HANDLE consolehandle;
  WORD windows_original_attributes;
  WORD windows_current_attributes;
#endif
  int term_is_set;
  int nocolor_is_set;
};

DLL_EXPORT_PORTCOLCON void portcolcon_get_version (int* pmajor, int* pminor, int* pmicro)
{
  if (pmajor)
    *pmajor = PORTCOLCON_VERSION_MAJOR;
  if (pminor)
    *pminor = PORTCOLCON_VERSION_MINOR;
  if (pmicro)
    *pmicro = PORTCOLCON_VERSION_MICRO;
}

DLL_EXPORT_PORTCOLCON const char* portcolcon_get_version_string ()
{
  return PORTCOLCON_VERSION_STRING;
}

DLL_EXPORT_PORTCOLCON portcolconhandle portcolcon_initialize ()
{
  struct portcolcon_struct* handle;
  char* s;
  if ((handle = (struct portcolcon_struct*)malloc(sizeof(struct portcolcon_struct))) == NULL)
    return NULL;
  handle->term_is_set = 0;
  handle->nocolor_is_set = 0;
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  CONSOLE_SCREEN_BUFFER_INFO consoleinfo;
  //get console handle and current console foreground and background color
  handle->consolehandle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (handle->consolehandle) {
    if (GetConsoleScreenBufferInfo(handle->consolehandle, &consoleinfo)) {
      //remember current console foreground and background color
      handle->windows_original_attributes = consoleinfo.wAttributes;
      handle->windows_current_attributes = consoleinfo.wAttributes;
      return handle;
    }
    handle->consolehandle = NULL;
  }
#endif
  if ((s = portcolcon_getenv("TERM")) != NULL) {
    if (*s)
      handle->term_is_set = 1;
    free(s);
  }
  if ((s = portcolcon_getenv("NO_COLOR")) != NULL) {
    if (*s)
      handle->nocolor_is_set = 1;
    free(s);
  }
  return handle;
}

DLL_EXPORT_PORTCOLCON void portcolcon_cleanup (portcolconhandle handle)
{
  if (handle) {
/*
    //restore initial console foreground and background color
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    SetConsoleTextAttribute(handle->consolehandle, handle->windows_original_attributes);
#endif
*/
    free(handle);
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_write (portcolconhandle handle, const char* data)
{
  if (!data || !*data)
    return;
/**/
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  if (handle->consolehandle) {
    WriteConsoleA(handle->consolehandle, data, strlen(data), NULL, NULL);
  } else
#endif
/**/
  {
    portcolcon_printf(handle, "%s", data);
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_color (portcolconhandle handle, unsigned char foreground_color, unsigned char background_color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS(foreground_color, background_color);
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[%i;%im", (int)CONSOLE_COLORS_TO_ANSI_FOREGROUND(foreground_color), (int)CONSOLE_COLORS_TO_ANSI_BACKGROUND(background_color));
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_foreground (portcolconhandle handle, unsigned char color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS(color, (handle->windows_current_attributes & 0x0F) >> 4);
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[%im", (int)CONSOLE_COLORS_TO_ANSI_FOREGROUND(color));
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_background (portcolconhandle handle, unsigned char color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS(handle->windows_current_attributes & 0x0F, color);
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[%im", (int)CONSOLE_COLORS_TO_ANSI_BACKGROUND(color));
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_reset_color (portcolconhandle handle)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = handle->windows_original_attributes;
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[0m");
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_title (portcolconhandle handle, const char* title)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      SetConsoleTitleA(title ? title : "");
    } else
#endif
    if (handle->term_is_set) {
      printf("\e]0;%s\a", (title ? title : ""));
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_move_cursor (portcolconhandle handle, int x, int y)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      COORD pos = {x, y};
      SetConsoleCursorPosition(handle->consolehandle, pos);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[%i;%iH", y + 1, x + 1);
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_show_cursor (portcolconhandle handle, int visible)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      CONSOLE_CURSOR_INFO cursorinfo;
      GetConsoleCursorInfo(handle->consolehandle, &cursorinfo);
      cursorinfo.bVisible = (visible ? TRUE : FALSE);
      SetConsoleCursorInfo(handle->consolehandle, &cursorinfo);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[25%c", (visible ? 'h' : 'l'));
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_clear_screen (portcolconhandle handle)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      CONSOLE_SCREEN_BUFFER_INFO consoleinfo;
      if (GetConsoleScreenBufferInfo(handle->consolehandle, &consoleinfo)) {
        DWORD n;
        COORD pos = {0, 0};
        DWORD consolesize = consoleinfo.dwSize.X * consoleinfo.dwSize.Y;
        FillConsoleOutputCharacterA(handle->consolehandle, ' ', consolesize, pos, &n);
        FillConsoleOutputAttribute(handle->consolehandle, handle->windows_original_attributes, consolesize, pos, &n);
      }
      portcolcon_move_cursor(handle, 0, 0);
    } else
#endif
    if (handle->term_is_set) {
      printf("\e[2J");
      portcolcon_move_cursor(handle, 0, 0);
    }
  }
}

DLL_EXPORT_PORTCOLCON char* portcolcon_getenv (const char* varname)
{
  char* result;
#ifdef _WIN32
  DWORD resultlen;
  if ((resultlen = GetEnvironmentVariableA(varname, NULL, 0)) == 0)
    return NULL;
  if ((result = (char*)malloc(resultlen)) == NULL)
    return NULL;
  GetEnvironmentVariableA(varname, result, resultlen);
#else
  if ((result = getenv(varname)) != NULL)
    result = strdup(result);
#endif
  return result;
}

DLL_EXPORT_PORTCOLCON char* portcolcon_getcurdir ()
{
  char* result;
#ifdef _WIN32
  DWORD resultlen;
  if ((resultlen = GetCurrentDirectoryA(0, NULL)) == 0)
    return NULL;
  if ((result = (char*)malloc(resultlen)) == NULL)
    return NULL;
  GetCurrentDirectoryA(resultlen, result);
#else
  char buf[PATH_MAX];
  if ((result = getcwd(buf, PATH_MAX)) != NULL)
    result = strdup(result);
#endif
  return result;
}

DLL_EXPORT_PORTCOLCON void portcolcon_free_string (char* data)
{
  free(data);
}



/////TO DO: detect if output is redirected (isatty() ?) in portcolcon_initialize()
