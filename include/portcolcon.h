/**
 * @file PORTABLECONSOLE.h
 * @brief PORTABLECONSOLE library header file with main functions
 * @author Brecht Sanders
 *
 * This header file defines the functions needed for the portableconsole library
 */

#ifndef INCLUDED_PORTCOLCON_H
#define INCLUDED_PORTCOLCON_H

#include <stdio.h>

/*! \cond PRIVATE */
#if !defined(DLL_EXPORT_PORTCOLCON)
# if defined(_WIN32) && (defined(BUILD_PORTCOLCON_DLL) || defined(PORTCOLCON_EXPORTS))
#  define DLL_EXPORT_PORTCOLCON __declspec(dllexport)
# elif defined(__MINGW32__) && !defined(STATIC) && !defined(BUILD_PORTCOLCON_STATIC) && !defined(BUILD_PORTCOLCON)
#  define DLL_EXPORT_PORTCOLCON __declspec(dllimport)
# else
#  define DLL_EXPORT_PORTCOLCON
# endif
#endif
/*! \endcond */

/*! \brief version number constants
 * \sa     portcolcon_get_version()
 * \sa     portcolcon_get_version_string()
 * \name   PORTCOLCON_VERSION_*
 * \{
 */
/*! \brief major version number */
#define PORTCOLCON_VERSION_MAJOR 0
/*! \brief minor version number */
#define PORTCOLCON_VERSION_MINOR 1
/*! \brief micro version number */
#define PORTCOLCON_VERSION_MICRO 0
/*! @} */

/*! \brief packed version number */
#define PORTCOLCON_VERSION (PORTCOLCON_VERSION_MAJOR * 0x01000000 + PORTCOLCON_VERSION_MINOR * 0x00010000 + PORTCOLCON_VERSION_MICRO * 0x00000100)

/*! \cond PRIVATE */
#define PORTCOLCON_VERSION_STRINGIZE_(major, minor, micro) #major"."#minor"."#micro
#define PORTCOLCON_VERSION_STRINGIZE(major, minor, micro) PORTCOLCON_VERSION_STRINGIZE_(major, minor, micro)
/*! \endcond */

/*! \brief string with dotted version number \hideinitializer */
#define PORTCOLCON_VERSION_STRING PORTCOLCON_VERSION_STRINGIZE(PORTCOLCON_VERSION_MAJOR, PORTCOLCON_VERSION_MINOR, PORTCOLCON_VERSION_MICRO)



#ifdef __cplusplus
extern "C" {
#endif

/*! \brief color constants
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_set_background()
 * \name   PORTCOLCON_COLOR_*
 * \{
 */
#define PORTCOLCON_COLOR_BLACK	            0x00
#define PORTCOLCON_COLOR_BLUE	              0x01
#define PORTCOLCON_COLOR_GREEN	            0x02
#define PORTCOLCON_COLOR_CYAN	              0x03
#define PORTCOLCON_COLOR_RED	              0x04
#define PORTCOLCON_COLOR_MAGENTA            0x05
#define PORTCOLCON_COLOR_BROWN  	          0x06
#define PORTCOLCON_COLOR_GRAY               0x07
#define PORTCOLCON_COLOR_DARK_GRAY          0x08
#define PORTCOLCON_COLOR_BRIGHT_BLUE	      0x09
#define PORTCOLCON_COLOR_BRIGHT_GREEN	      0x0A
#define PORTCOLCON_COLOR_BRIGHT_CYAN	      0x0B
#define PORTCOLCON_COLOR_BRIGHT_RED	        0x0C
#define PORTCOLCON_COLOR_BRIGHT_MAGENTA     0x0D
#define PORTCOLCON_COLOR_YELLOW	            0x0E
#define PORTCOLCON_COLOR_WHITE              0x0F
#define PORTCOLCON_COLOR_BRIGHT_GRAY        PORTCOLCON_COLOR_GRAY
#define PORTCOLCON_COLOR_BRIGHT_YELLOW      PORTCOLCON_COLOR_YELLOW
#define PORTCOLCON_COLOR_DARK_YELLOW        PORTCOLCON_COLOR_BROWN
#define PORTCOLCON_COLOR_BRIGHT_WHITE       PORTCOLCON_COLOR_WHITE
/*! @} */

/*! \brief handle type used by portcolcon library
 * \sa     portcolcon_initialize()
 * \sa     portcolcon_cleanup()
 * \sa     portcolcon_write()
 * \sa     portcolcon_printf()
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_set_background()
 * \sa     portcolcon_reset_color()
 * \sa     portcolcon_set_title()
 */
typedef struct portcolcon_struct* portcolconhandle;

/*! \brief get portcolcon library version string
 * \param  pmajor        pointer to integer that will receive major version number
 * \param  pminor        pointer to integer that will receive minor version number
 * \param  pmicro        pointer to integer that will receive micro version number
 * \sa     portcolcon_get_version_string()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_get_version (int* pmajor, int* pminor, int* pmicro);

/*! \brief get portcolcon library version string
 * \return version string
 * \sa     portcolcon_get_version()
 */
DLL_EXPORT_PORTCOLCON const char* portcolcon_get_version_string ();

/*! \brief initialize portcolcon library
 * \return handle for use with other portcolcon functions, when done destroy with portcolcon_cleanup()
 * \sa     portcolcon_cleanup()
 * \sa     portcolcon_write()
 * \sa     portcolcon_printf()
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_set_background()
 * \sa     portcolcon_reset_color()
 * \sa     portcolcon_set_title()
 */
DLL_EXPORT_PORTCOLCON portcolconhandle portcolcon_initialize ();

/*! \brief initialize portcolcon library
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \return handle for use with other portcolcon functions
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_cleanup (portcolconhandle handle);

/*! \brief display text on the console
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  data          text to display in console
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_write (portcolconhandle handle, const char* data);

/*! \brief display text on the console
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  ...           arguments as used with printf()
 * \sa     portcolcon_initialize()
 */
#define portcolcon_printf(handle, ...) printf(__VA_ARGS__)

/*! \brief set the console foreground and background color
 * \param  handle            portcolcon handle returned by portcolcon_initialize()
 * \param  foreground_color  foreground color
 * \param  background_color  background color
 * \sa     portcolcon_initialize()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_set_background()
 * \sa     portcolcon_reset_color()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_set_color (portcolconhandle handle, unsigned char foreground_color, unsigned char background_color);

/*! \brief set the console foreground color
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  color         foreground color
 * \sa     portcolcon_initialize()
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_background()
 * \sa     portcolcon_reset_color()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_set_foreground (portcolconhandle handle, unsigned char color);

/*! \brief set the console background color
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  color         background color
 * \sa     portcolcon_initialize()
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_reset_color()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_set_background (portcolconhandle handle, unsigned char color);

/*! \brief reset the console foreground and background color their original colors
 * \sa     portcolcon_initialize()
 * \sa     portcolcon_set_color()
 * \sa     portcolcon_set_foreground()
 * \sa     portcolcon_set_background()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_reset_color (portcolconhandle handle);

/*! \brief set the console foreground and background color
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  title         text to set as the console title
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_set_title (portcolconhandle handle, const char* title);

/*! \brief sets the cursor at the specified position
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  x             column to position the cursor at (leftmost position = 0)
 * \param  y             row to position the cursor at (topmost position = 0)
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_move_cursor (portcolconhandle handle, int x, int y);

/*! \brief sets the cursor at the specified position
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \param  visible       0 to hide cursor or non-zero to show cursor
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_show_cursor (portcolconhandle handle, int visible);

/*! \brief clears the console and positions the cursor at the top left
 * \param  handle        portcolcon handle returned by portcolcon_initialize()
 * \sa     portcolcon_initialize()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_clear_screen (portcolconhandle handle);

/*! \brief get value of environment variable
 * \param  varname       variable name
 * \return value of environment variable or NULL if not found (caller must portcolcon_free_string() result)
 * \sa     portcolcon_getcurdir()
 * \sa     portcolcon_free_string()
 */
DLL_EXPORT_PORTCOLCON char* portcolcon_getenv (const char* varname);

/*! \brief get path of current working directory
 * \param  varname       variable name
 * \return path of current directory or NULL on error (caller must portcolcon_free_string() result)
 * \sa     portcolcon_getenv()
 * \sa     portcolcon_free_string()
 */
DLL_EXPORT_PORTCOLCON char* portcolcon_getcurdir ();

/*! \brief get current path
 * \param  data          text to be freed in memory
 * \sa     portcolcon_getenv()
 * \sa     portcolcon_getcurdir()
 */
DLL_EXPORT_PORTCOLCON void portcolcon_free_string (char* data);

#ifdef __cplusplus
}
#endif

#endif
