// This file is part of retro-gtk. License: GPL-3.0+.

#pragma once

#if !defined(__RETRO_GTK_INSIDE__) && !defined(RETRO_GTK_COMPILATION)
# error "Only <retro-gtk.h> can be included directly."
#endif

#include <glib.h>

G_BEGIN_DECLS

/**
 * retro_try:
 * @try: the block that can throw an error
 * @catch: the name for the variable storing the caught error
 * @code: the block that can handle the caught error
 *
 * Creates the temporary #GError pointer named @catch, executes the @try block,
 * then checks if an error has been caught.
 *
 * If an error is caught, executes the @code clock and free the caught error.
 * If you don't want the caught error to be freed, set @caught to %NULL.
 *
 * If you want to propagate the caught error, consider using
 * retro_try_propagate() and retro_try_propagate_val() instead.
 *
 * |[<!-- language="C" -->
 * void
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_if_fail (error == NULL || *error == NULL);
 *
 *   // Try sub_function_that_can_fail(), if it throws an error, catch it, let
 *   // you handle it in the second block, and frees it.
 *   retro_try ({
 *     sub_function_that_can_fail (&catch);
 *   }, catch, {
 *     g_debug ("Caught error: %s", catch->message);
 *   });
 *
 *   …
 * }
 * ]|
 *
 * Error pileups are always a bug.
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * void
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_if_fail (error == NULL || *error == NULL);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try_propagate ({
 *     sub_function_that_can_fail (&catch);
 *     other_function_that_can_fail (&catch);
 *   }, catch, {
 *     g_debug ("Caught error: %s", catch->message);
 *   });
 *
 *   …
 * }
 * ]|
 *
 * You must never return from the @try block or use statements to jump out of
 * the @try block, that would skip the error handling, defeating the purpose of
 * using retro_try().
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * gboolean
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try ({
 *     return sub_function_that_can_fail (&catch);
 *   }, catch, {
 *     g_debug ("Caught error: %s", catch->message);
 *     return FALSE;
 *   });
 *
 *   …
 * }
 * ]|
 */
#define retro_try(try, catch, code) \
  G_STMT_START { \
    g_autoptr (GError) catch = NULL; \
    {try;} \
    if (G_UNLIKELY (catch != NULL)) {code;} \
  } G_STMT_END

/**
 * retro_try_propagate:
 * @try: the block that can throw an error
 * @catch: the name for the variable storing the caught error
 * @dest: (out callee-allocates) (optional) (nullable): error return location
 *
 * Creates the temporary #GError pointer named @catch, executes the @try block,
 * then checks if an error has been caught.
 * If the function returns a value, use retro_try_propagate_val() instead.
 *
 * If an error is caught, it is propagated into @dest and the function returns.
 * Otherwise, the execution continues after this block.
 *
 * The error variable @dest points to must be %NULL.
 *
 * You can't clean up before the function returns, use auto cleanups or
 * retro_try() for that.
 *
 * |[<!-- language="C" -->
 * void
 * my_function_that_can_fail (GError **error)
 * {
 *   g_autofree gchar *string = NULL;
 *
 *   g_return_val_if_fail (error == NULL || *error == NULL);
 *
 *   string = g_strdup ("I will be freed on error propagation.");
 *
 *   // Try sub_function_that_can_fail(), if it throws an error, propagate it to
 *   // error if it isn't NULL, frees it otherwise, and return.
 *   retro_try_propagate ({
 *     sub_function_that_can_fail (&catch);
 *   }, catch, error);
 *
 *   // If no error was caught, continue.
 *   …
 * }
 * ]|
 *
 * Error pileups are always a bug.
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * void
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_if_fail (error == NULL || *error == NULL);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try_propagate ({
 *     sub_function_that_can_fail (&catch);
 *     other_function_that_can_fail (&catch);
 *   }, catch, error);
 *
 *   …
 * }
 * ]|
 *
 * You must never return from the @try block or use statements to jump out of
 * the @try block, that would skip the error handling, defeating the purpose of
 * using retro_try_propagate().
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * void
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_if_fail (error == NULL || *error == NULL);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try_propagate ({
 *     if (!sub_function_that_can_fail (&catch))
 *       return;
 *   }, catch, error);
 *
 *   …
 * }
 * ]|
 */
#define retro_try_propagate(try, catch, dest) \
  retro_try (try, catch, { g_propagate_error (dest, catch); return; })

/**
 * retro_try_propagate_val:
 * @try: the block that can throw an error
 * @catch: the name for the variable storing the caught error
 * @dest: (out callee-allocates) (optional) (nullable): error return location
 * @val: the value to return from the current function if the error is propagated
 *
 * Creates the temporary #GError pointer named @catch, executes the @try block,
 * then checks if an error has been caught.
 * If the function does not return a value, use retro_try_propagate() instead.
 *
 * If an error is caught, it is propagated into @dest and the function returns
 * @val.
 * Otherwise, the execution continues after this block.
 *
 * The error variable @dest points to must be %NULL.
 *
 * You can't clean up before the function returns, use auto cleanups or
 * retro_try() for that.
 *
 * |[<!-- language="C" -->
 * gboolean
 * my_function_that_can_fail (GError **error)
 * {
 *   g_autofree gchar *string = NULL;
 *
 *   g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
 *
 *   string = g_strdup ("I will be freed on error propagation.");
 *
 *   // Try sub_function_that_can_fail(), if it throws an error, propagate it to
 *   // error if it isn't NULL, frees it otherwise, and return.
 *   retro_try_propagate_val ({
 *     sub_function_that_can_fail (&catch);
 *   }, catch, error, FALSE);
 *
 *   // If no error was caught, continue.
 *   …
 * }
 * ]|
 *
 * Error pileups are always a bug.
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * gboolean
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try_propagate_val ({
 *     sub_function_that_can_fail (&catch);
 *     other_function_that_can_fail (&catch);
 *   }, catch, error, FALSE);
 *
 *   …
 * }
 * ]|
 *
 * You must never return from the @try block or use statements to jump out of
 * the @try block, that would skip the error handling, defeating the purpose of
 * using retro_try_propagate_val().
 * For example, this code is incorrect:
 * |[<!-- language="C" -->
 * gboolean
 * my_function_that_can_fail (GError **error)
 * {
 *   g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
 *
 *   // Errors thrown by sub_function_that_can_fail() are incorrectly unhandled.
 *   retro_try_propagate_val ({
 *     return sub_function_that_can_fail (&catch);
 *   }, catch, error, FALSE);
 *
 *   …
 * }
 * ]|
 */
#define retro_try_propagate_val(try, catch, dest, val) \
  retro_try (try, catch, { g_propagate_error (dest, catch); return (val); })

G_END_DECLS
