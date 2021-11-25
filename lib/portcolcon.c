#define _GNU_SOURCE     //needed for strcasestr()
#include "portcolcon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
#define CONSOLE_COLORS_TO_WINDOWS(fg,bg) (((fg) & 0x0F) + (((bg) & 0x0F) << 4))
#define WINDOWS_ATTR_TO_CONSOLE_FOREGROUND(attr) ((attr) & 0x0F)
#define WINDOWS_ATTR_TO_CONSOLE_BACKGROUND(attr) (((attr) & 0xF0) >> 4)
#endif
static const unsigned char toansibaselookup[8] = {0, 4, 2, 6, 1, 5, 3, 7};
#define CONSOLE_COLORS_TO_ANSI_FOREGROUND(color) ((((color) & 0x08) == 0 ? 30 : 90) + toansibaselookup[(color) & 0x07])
#define CONSOLE_COLORS_TO_ANSI_BACKGROUND(color) ((((color) & 0x08) == 0 ? 40 : 100) + toansibaselookup[(color) & 0x07])

struct portcolcon_struct {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  HANDLE consolehandle;
  WORD windows_original_attributes;
  WORD windows_current_attributes;
#endif
  unsigned int ansi_current_fgcolor;
  unsigned int ansi_current_bgcolor;
  int nocolor_is_set;
  int term_is_set;
};

#ifdef _WIN32
static char* strcasestr (const char* haystack, const char* needle)
{
  size_t needlelen;
  if (!needle || !*needle)
      return (char*)haystack;
  needlelen = strlen(needle);
  while (*haystack) {
    if (_strnicmp(haystack, needle, needlelen) == 0)
      return (char*)haystack;
    haystack++;
  }
  return NULL;
}
#endif

DLL_EXPORT_PORTCOLCON portcolconhandle portcolcon_initialize ()
{
  struct portcolcon_struct* handle;
  char* s;
  if ((handle = (struct portcolcon_struct*)malloc(sizeof(struct portcolcon_struct))) == NULL)
    return NULL;
  handle->nocolor_is_set = 0;
  handle->term_is_set = 0;
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  CONSOLE_SCREEN_BUFFER_INFO consoleinfo;
  //get console handle and current console foreground and background color
  handle->consolehandle = GetStdHandle(STD_OUTPUT_HANDLE);
  handle->windows_original_attributes = CONSOLE_COLORS_TO_WINDOWS(PORTCOLCON_COLOR_GRAY, PORTCOLCON_COLOR_BLACK);
  if (handle->consolehandle) {
    if (GetConsoleScreenBufferInfo(handle->consolehandle, &consoleinfo)) {
      //remember current console foreground and background color
      handle->windows_original_attributes = consoleinfo.wAttributes;
    } else {
      handle->consolehandle = NULL;
    }
  }
  handle->windows_current_attributes = handle->windows_original_attributes;
#endif
  handle->ansi_current_fgcolor = CONSOLE_COLORS_TO_ANSI_FOREGROUND(PORTCOLCON_COLOR_GRAY);
  handle->ansi_current_bgcolor = CONSOLE_COLORS_TO_ANSI_BACKGROUND(PORTCOLCON_COLOR_BLACK);
  if ((s = portcolcon_getenv("NO_COLOR")) != NULL) {
    if (*s)
      handle->nocolor_is_set = 1;
    free(s);
  } else if ((s = portcolcon_getenv("TERM")) != NULL) {
    if (*s)
      handle->term_is_set = 1;
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

DLL_EXPORT_PORTCOLCON void portcolcon_set_color (portcolconhandle handle, unsigned char foreground_color, unsigned char background_color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS((foreground_color != PORTCOLCON_COLOR_IGNORE ? foreground_color : WINDOWS_ATTR_TO_CONSOLE_FOREGROUND(handle->windows_current_attributes)), (background_color != PORTCOLCON_COLOR_IGNORE ? background_color : WINDOWS_ATTR_TO_CONSOLE_BACKGROUND(handle->windows_current_attributes)));
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      if (foreground_color != PORTCOLCON_COLOR_IGNORE)
        handle->ansi_current_fgcolor = CONSOLE_COLORS_TO_ANSI_FOREGROUND(foreground_color);
      if (background_color != PORTCOLCON_COLOR_IGNORE)
        handle->ansi_current_bgcolor = CONSOLE_COLORS_TO_ANSI_BACKGROUND(background_color);
      printf("\e[%u;%um", handle->ansi_current_fgcolor, handle->ansi_current_bgcolor);
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_foreground (portcolconhandle handle, unsigned char color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS(color, WINDOWS_ATTR_TO_CONSOLE_BACKGROUND(handle->windows_current_attributes));
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      if (color != PORTCOLCON_COLOR_IGNORE) {
        handle->ansi_current_fgcolor = CONSOLE_COLORS_TO_ANSI_FOREGROUND(color);
        printf("\e[%um", handle->ansi_current_fgcolor);
      }
    }
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_set_background (portcolconhandle handle, unsigned char color)
{
  if (!handle->nocolor_is_set) {
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
    if (handle->consolehandle) {
      handle->windows_current_attributes = CONSOLE_COLORS_TO_WINDOWS(WINDOWS_ATTR_TO_CONSOLE_FOREGROUND(handle->windows_current_attributes), color);
      SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
    } else
#endif
    if (handle->term_is_set) {
      if (color != PORTCOLCON_COLOR_IGNORE) {
        handle->ansi_current_bgcolor = CONSOLE_COLORS_TO_ANSI_BACKGROUND(color);
        printf("\e[%um", handle->ansi_current_bgcolor);
      }
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

DLL_EXPORT_PORTCOLCON void portcolcon_write (portcolconhandle handle, const char* data)
{
  if (!data || !*data)
    return;
/*
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  if (handle->consolehandle) {
    WriteConsoleA(handle->consolehandle, data, strlen(data), NULL, NULL);
  } else
#endif
*/
  {
    portcolcon_printf(handle, "%s", data);
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_write_highlight (portcolconhandle handle, const char* data, const char* searchtext, int casesensitive, unsigned char foreground_color, unsigned char background_color)
{
  if (!data || !*data)
    return;
  if (handle->nocolor_is_set || !searchtext || !*searchtext) {
    portcolcon_write(handle, data);
  } else {
    const char* pos;
    const char* lastpos = data;
    char* (*selected_strstr)(const char *, const char *) = (casesensitive ? strstr : strcasestr);
    int searchtextlen = strlen(searchtext);
    while ((pos = (selected_strstr)(lastpos, searchtext)) != NULL) {
      portcolcon_printf(handle, "%.*s", (int)(pos - lastpos), lastpos);
      portcolcon_printf_in_color(handle, foreground_color, background_color, "%.*s", searchtextlen, pos);
      lastpos = pos + searchtextlen;
    }
    portcolcon_printf(handle, "%s", lastpos);
  }
}

DLL_EXPORT_PORTCOLCON void portcolcon_write_multiple_highlights (portcolconhandle handle, const char* data, const char* searchtexts[], int casesensitive, unsigned char foreground_color, unsigned char background_color)
{
  if (!data || !*data)
    return;
  if (handle->nocolor_is_set || !searchtexts || !searchtexts[0]) {
    portcolcon_write(handle, data);
  } else {
    size_t i;
    struct {
      int searchtextlen;
      const char* pos;
    } *searchtextstatus;
    size_t earliestindex;
    const char* lastpos = data;
    size_t searchtextscount = 0;
    char* (*selected_strstr)(const char *, const char *) = (casesensitive ? strstr : strcasestr);
    //count number of search texts
    searchtextscount = 0;
    while (searchtexts[searchtextscount])
      searchtextscount++;
    searchtextstatus = malloc(searchtextscount * sizeof(*searchtextstatus));
    //find earliest match
    earliestindex = searchtextscount;
    for (i = 0; i < searchtextscount; i++) {
      searchtextstatus[i].searchtextlen = strlen(searchtexts[i]);
      searchtextstatus[i].pos = (selected_strstr)(lastpos, searchtexts[i]);
      if (searchtextstatus[i].pos && (earliestindex >= searchtextscount || searchtextstatus[i].pos <= searchtextstatus[earliestindex].pos)) {
        if (earliestindex < searchtextscount && searchtextstatus[i].pos == searchtextstatus[earliestindex].pos && searchtextstatus[i].searchtextlen < searchtextstatus[earliestindex].searchtextlen) {
          //if multiple matches skip shortest one
          searchtextstatus[i].pos = (selected_strstr)(lastpos + 1, searchtexts[i]);
        } else {
          earliestindex = i;
        }
      }
    }
    //loop while match found
    while (earliestindex < searchtextscount) {
      portcolcon_printf(handle, "%.*s", (int)(searchtextstatus[earliestindex].pos - lastpos), lastpos);
      portcolcon_printf_in_color(handle, foreground_color, background_color, "%.*s", searchtextstatus[earliestindex].searchtextlen, searchtextstatus[earliestindex].pos);
      lastpos = searchtextstatus[earliestindex].pos + searchtextstatus[earliestindex].searchtextlen;
      searchtextstatus[earliestindex].pos = (selected_strstr)(lastpos, searchtexts[earliestindex]);
      //find earliest match
      earliestindex = searchtextscount;
      for (i = 0; i < searchtextscount; i++) {
        if (searchtextstatus[i].pos <= lastpos)
          searchtextstatus[i].pos = (selected_strstr)(lastpos, searchtexts[i]);
        if (searchtextstatus[i].pos && (earliestindex >= searchtextscount || searchtextstatus[i].pos <= searchtextstatus[earliestindex].pos)) {
          if (earliestindex < searchtextscount && searchtextstatus[i].pos == searchtextstatus[earliestindex].pos && searchtextstatus[i].searchtextlen < searchtextstatus[earliestindex].searchtextlen) {
            //if multiple matches skip shortest one
            searchtextstatus[i].pos = (selected_strstr)(lastpos + 1, searchtexts[i]);
          } else {
            earliestindex = i;
          }
        }
      }
    }
    portcolcon_printf(handle, "%s", lastpos);
    free(searchtextstatus);
  }
}

DLL_EXPORT_PORTCOLCON int portcolcon_printf (portcolconhandle handle, const char* format, ...)
{
  int result;
  va_list args;
  va_start(args, format);
  result = vprintf(format, args);
  va_end(args);
  return result;
}

DLL_EXPORT_PORTCOLCON int portcolcon_printf_in_color (portcolconhandle handle, unsigned char foreground_color, unsigned char background_color, const char* format, ...)
{
  int result;
  va_list args;
  va_start(args, format);
  //remember current colors and set new colors
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  if (handle->consolehandle) {
    SetConsoleTextAttribute(handle->consolehandle, CONSOLE_COLORS_TO_WINDOWS((foreground_color != PORTCOLCON_COLOR_IGNORE ? foreground_color : WINDOWS_ATTR_TO_CONSOLE_FOREGROUND(handle->windows_current_attributes)), (background_color != PORTCOLCON_COLOR_IGNORE ? background_color : WINDOWS_ATTR_TO_CONSOLE_BACKGROUND(handle->windows_current_attributes))));
  } else
#endif
  if (handle->term_is_set) {
    printf("\e[%u;%um", (unsigned int)(foreground_color != PORTCOLCON_COLOR_IGNORE ? CONSOLE_COLORS_TO_ANSI_FOREGROUND(foreground_color) : handle->ansi_current_fgcolor), (unsigned int)(background_color != PORTCOLCON_COLOR_IGNORE ? CONSOLE_COLORS_TO_ANSI_BACKGROUND(background_color) : handle->ansi_current_bgcolor));
  }
  //print match
  result = vprintf(format, args);
  //restore color
#if defined(_WIN32) && !defined(NOWINDOWSCONSOLE)
  if (handle->consolehandle) {
    SetConsoleTextAttribute(handle->consolehandle, handle->windows_current_attributes);
  } else
#endif
  if (handle->term_is_set) {
    printf("\e[%u;%um", handle->ansi_current_fgcolor, handle->ansi_current_bgcolor);
  }
  va_end(args);
  return result;
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
      printf("\e[%u;%uH", (unsigned int)(y + 1), (unsigned int)(x + 1));
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



/////TO DO: detect if output is redirected (isatty() ?) in portcolcon_initialize()
