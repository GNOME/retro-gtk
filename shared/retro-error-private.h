// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef GError RetroError;

void retro_error_ensure_free (RetroError *error);

/**
 * retro_propagate_if_error:
 * @dest: (out callee-allocates) (optional) (nullable): error return location
 * @src: (transfer full): error to checkmove into the return location
 *
 * Verifies that the source error @src has not been set, which imlplies it is
 * %NULL.
 * If the function returns a value, use retro_propagate_val_if_error() instead.
 *
 * If @src is non-%NULL and @dest is %NULL, free @src; otherwise, moves @src
 * into *@dest and return.
 * The error variable @dest points to must be %NULL.
 */
#define retro_propagate_if_error(dest, src) \
  G_STMT_START { \
    if (G_UNLIKELY (src != NULL)) { \
      g_propagate_error (dest, src); \
      src = NULL; \
      return; \
    } \
  } G_STMT_END

/**
 * retro_propagate_val_if_error:
 * @dest: (out callee-allocates) (optional) (nullable): error return location
 * @src: (transfer full): error to checkmove into the return location
 * @val: the value to return from the current function
 *       if the error is propagated
 *
 * Verifies that the source error @src has not been set, which imlplies it is
 * %NULL.
 * If the function does not return a value, use retro_propagate_if_error() instead.
 *
 * If @src is non-%NULL and @dest is %NULL, free @src; otherwise, moves @src
 * into *@dest and return @val.
 * The error variable @dest points to must be %NULL.
 */
#define retro_propagate_val_if_error(dest, src, val) \
  G_STMT_START { \
    if (G_UNLIKELY (src != NULL)) { \
      g_propagate_error (dest, src); \
      src = NULL; \
      return (val); \
    } \
  } G_STMT_END

/**
 * retro_throw_if_fail:
 * @err: (out callee-allocates) (optional): a return location for a #GError
 * @expr: the expression to check
 * @domain: error domain
 * @code: error code
 * @format: printf()-style format
 * @...: args for @format
 *
 * Verifies that the expression @expr, usually representing an error condition,
 * evaluates to %TRUE. If the function returns a value, use
 * retro_throw_val_if_fail() instead.
 *
 * If @expr evaluates to %FALSE, an error condition has been found and the
 * current function must throw it and return immediately.
 *
 * To throw this error, if @expr evaluates to %FALSE, does nothing if @err is
 * %NULL; if @err is non-%NULL, then *@err must be %NULL. A new #GError is
 * created and assigned to *@err. Then, the current function returns.
 */
#define retro_throw_if_fail(err, expr, domain, code, format, ...) \
  G_STMT_START { \
    if (G_LIKELY (expr)) { } \
    else { \
      g_set_error (err, domain, code, format __VA_OPT__(,) __VA_ARGS__); \
      return; \
    } \
  } G_STMT_END

/**
 * retro_throw_val_if_fail:
 * @err: (out callee-allocates) (optional): a return location for a #GError
 * @expr: the expression to check
 * @val: the value to return from the current function
 *       if the expression is not true
 * @domain: error domain
 * @code: error code
 * @format: printf()-style format
 * @...: args for @format
 *
 * Verifies that the expression @expr, usually representing an error condition,
 * evaluates to %TRUE. If the function does not return a value, use
 * retro_throw_if_fail() instead.
 *
 * If @expr evaluates to %FALSE, an error condition has been found and the
 * current function must throw it and return immediately.
 *
 * To throw this error, if @expr evaluates to %FALSE, does nothing if @err is
 * %NULL; if @err is non-%NULL, then *@err must be %NULL. A new #GError is
 * created and assigned to *@err. Then, @val is returned from the current
 * function.
 */
#define retro_throw_val_if_fail(err, expr, val, domain, code, format, ...) \
  G_STMT_START { \
    if (G_LIKELY (expr)) { } \
    else { \
      g_set_error (err, domain, code, format __VA_OPT__(,) __VA_ARGS__); \
      return (val); \
    } \
  } G_STMT_END

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RetroError, retro_error_ensure_free)

G_END_DECLS
