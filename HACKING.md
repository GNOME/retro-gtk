## Coding Style

### Identation

Use 2 spaces for indentation, use spaces for alignment.

### Parentheses

Always put a single space before an opening parenthesis.

### Function Definition

Put attributes and the return type (including its asterisks) in a single line
preceding the function's name.

Good:
```c
static void
function (FooBar *bar)
```

Bad:
```c
static void function (FooBar *bar)
```

Have a single parameter per line, the 1st parameter sitting on the same line as
the function's name.
Arrange the parameters into 4 columns:
- the 1st column has the base types and is aligned left,
- the 2nd column is a padding of a single space,
- the 3rd column has the asterisks and is aligned right,
- the last column has the parameter names.

Good:
```c
static void
function (FooBar  *bar,
          char   **strings,
          void    *data)
```

Bad:
```c
static void
function (FooBar *bar,
          char **strings,
          void *data)
```

Bad:
```c
static void
function (FooBar *bar, char **strings, void *data)
```

If the function has no parameter, explicit that with `void`.

Good:
```c
static int
function (void)
```

Bad:
```c
static int
function ()
```

Functions have their opening curly brace on a new line.

Good:
```c
static void
function (…)
{
  …
}
```

Bad:
```c
static void
function (…) {
  …
}
```

### Function Result

If a public function transfers ownership of its returned value, explicit that by
giving the function the `G_GNUC_WARN_UNUSED_RESULT` attribute.

### Function Order

Functions are defined in the following order:
- local functions
- callback functions
- overridden virtual methods
- the class init function
- the init function
- for each implemented interface:
  - interface methods
  - interface init functions
- public constructors
- private constructors
- public methods
- private methods

### Structures, Unions, and Enumerations

Structures, unions, and enumerations have their opening curly brace on a new
line.

Good:
```c
struct FooBar
{
  …
};
```

Good:
```c
typedef struct
{
  …
} FooBar;
```

Bad:
```c
struct FooBar {
  …
};
```

Function calls and macro calls have a space between the function's name and the
invocation parentheses.

Good:
```c
foo_bar_get_state (FOO_BAR (self->bar));
```

Bad:
```c
foo_bar_get_state(FOO_BAR(self->bar));
```

### Conditional Statements

Conditional statements must surround all their block with curly braces, unless
all the blocks from the statement have a single line.

Good:
```c
if (i < 0)
  i++;
else
  i--;
```

Good:
```c
if (i < 0) {
  i++;
  j++;
}
else {
  i--;
}
```

Good:
```c
if (i < 0) {
  i++;
}
else {
  i--;
  j--;
}
```

Bad:
```c
if (i < 0) {
  i++;
}
else {
  i--;
}
```

Bad:
```c
if (i < 0) {
  i++;
  j++;
}
else
  i--;
```

Bad:
```c
if (i < 0)
  i++;
else {
  i--;
  j--;
}
```

The `if`, `else`, and `else if` statements have their opening curly brace at the
end of their last line, separated by a space.

Good:
```c
if (i < 0) {
  …
}
```

Bad:
```c
if (i < 0)
{
  …
}
```

The `else` and `else if` statements must not be on the same line as the previous
closing curly brace.

Good:
```c
if (…) {
  …
}
else if (…) {
  …
}
else {
  …
}
```

Bad:
```c
if (…) {
  …
} else if (…) {
  …
} else {
  …
}
```

# Type Casts

Type casts must have a space between the closing parenthesis and the casted
value.

Good:
```c
name = (gchar *) data;
```

Bad:
```c
name = (gchar *)data;
```

### Switch Statements

Switch statements have their opening curly brace at the end of the line,
separated by a space.

Good:
```c
switch (…) {
  …
}
```

Bad:
```c
switch (…)
{
  …
}
```

The `case` statements have the same indentation as the `switch` statement, their
block is indented as usual.

Good:
```c
switch (…) {
case …:
  …
}
```

Bad:
```c
switch (…) {
  case …:
    …
}
```

### Header Inclusion Guards

Guard header inclusion with `#pragma once` rather than the traditional
`#ifndef`-`#define`-`#endif` trio.

Internal headers (for consistency, whether they need to be installed or not)
should contain the following guard to prevent users from directly including
them:
```c
#if !defined(_FOO_INSIDE) && !defined(FOO_COMPILATION)
#error "Only <foo.h> can be included directly."
#endif
```

Only after these should you include headers.

### Signals

Prefix signal enum names with `SIGNAL_*`, except the signal count which must be
named `N_SIGNALS`.

Good:
```c
enum {
  SIGNAL_SUBMITTED,
  SIGNAL_DELETED,
  SIGNAL_SYMBOL_CLICKED,
  N_SIGNALS,
};
```

Also note that the last element ends with a comma to reduce diff noise when
adding further signals.

### Properties

Prefix property enum names with `PROP_*`, except the properties count which must
be named `N_PROPS`.

Good:
```c
enum {
  PROP_0 = 0,
  PROP_NUMBER,
  PROP_SHOW_ACTION_BUTTONS,
  PROP_COLUMN_SPACING,
  PROP_ROW_SPACING,
  PROP_RELIEF,
  N_PROPS,
};
```

Also note that the last element ends with a comma to reduce diff noise when
adding further properties.

### Comment Style

In comments use full sentences with proper capitalization and punctuation.

Good:
```c
/* Make sure we don't overflow. */
```

Bad:
```c
/* overflow check */
```

### Callbacks

Suffix callback functions with `*_cb`, it must not have prefixes like `on_*`.

Good:
```c
g_signal_connect (self, "clicked", G_CALLBACK (button_clicked_cb), NULL);
```

Bad:
```c
g_signal_connect (self, "clicked", G_CALLBACK (on_button_clicked), NULL);
```

Bad:
```c
g_signal_connect (self, "clicked", G_CALLBACK (on_button_clicked_cb), NULL);
```

Bad:
```c
g_signal_connect (self, "clicked", G_CALLBACK (handle_button_clicked), NULL);
```

### Static Functions

Static functions don't need the class prefix.  E.g. with a type `foo_bar`.

Good:
```c
static void
button_clicked_cb (GtkButton *button)
```

Bad:
```c
static void
foo_bar_button_clicked_cb (GtkButton *button)
```

Note however that virtual methods like
`<class_name>_{init,constructed,finalize,dispose}` do take the class prefix.
These functions are usually never called directly but only assigned once in
`<class_name>_constructed` so the longer name is acceptable.
This also helps to distinguish virtual methods from regular private methods and
helper functions.

### Self Argument

The object a method is called on must be named `self`, including in callbacks.
When possible, make it the first argument.
When a method affects several objects of the same type in an equal manner, e.g.
a comparison method, feel free to name the parameters `self` and `other`, or `a`
and `b`.

Good:
```c
static gboolean
something_happened_cb (FooBar *self)
{
  g_return_val_if_fail (FOO_IS_BAR (self), FALSE);
  …
  return FALSE;
}
```

And for a public function.

Good:
```c
FooState *
foo_bar_get_state (FooBar *self)
{
  FooBarPrivate *priv;

  g_return_val_if_fail (FOO_IS_BAR (self), NULL);

  priv = foo_bar_get_instance_private (self);

  return priv->state;
}
```

### Private Fields

The private section of an object must not be retrieved before ensuring the
object is valid.

Good:
```c
FooState *
foo_bar_get_state (FooBar *self)
{
  FooBarPrivate *priv;

  g_return_val_if_fail (FOO_IS_BAR (self), NULL);

  priv = foo_bar_get_instance_private (self);

  return priv->state;
}
```

Bad:
```c
FooState *
foo_bar_get_state (FooBar *self)
{
  FooBarPrivate *priv = foo_bar_get_instance_private (self);

  g_return_val_if_fail (FOO_IS_BAR (self), NULL);

  return priv->state;
}
```

### User interface files

User interface template files should have the `.ui` extension, the same base
name as other files of their class, and be localted at the same place as other
files for that class.

E.g.:
```
foo-bar.c
foo-bar.h
foo-bar-private.h
foo-bar.ui
```

#### Properties

Use minus signs instead of underscores in property names.

Good:
```xml
<property name="margin-start">12</property>
```

Bad:
```xml
<property name="margin_start">12</property>
```

### Automatic cleanup

It's recommended to use `g_auto()`, `g_autoptr()`, `g_autofree` for
automatic resource cleanup when possible.
Do not put a space between the attribute and its opening parenthesis.

Good:
```c
g_autoptr(FooBaz) baz = foo_bar_generate_baz (bar);
```

Bad:
```c
g_autoptr (FooBaz) baz = foo_bar_generate_baz (bar);
```

Bad:
```c
FooBaz *baz = foo_bar_generate_baz (bar);
…
g_object_unref (baz);
```

## Documentation Style

### XML Documentation

This describes how to write XML documentation like the reference manual.

Indent with two spaces.

Start paragraphs on a new line, indented.
Break lines in paragraphs after 80 characters when possible, including the
indentation.
Start sentences on a new line so they can be changed without having to reflow
the whole paragraph.

Don't break lines in the middle of inlined elements like links, only before or
after.
It is acceptable to let the element exceeds the 80 characters limit if you have
no other choice, e.g. if a URL you are linking to is too long.

### MarkDown Documentation

This describes how to write MarkDown documentation like README.md or HACKING.md.

Break lines in paragraphs after 80 characters when possible.
Start sentences on a new line so they can be changed without having to reflow
the whole paragraph.

Don't break lines in the middle of inlined elements like links, only before or
after.
It is acceptable to let the element exceeds the 80 characters limit if you have
no other choice, e.g. if a URL you are linking to is too long.

## Commit Message Style

Commit messages have 4 parts: a tag, a short explanation, a long explanation,
and an issue reference.

```
tag: Short explanation of the commit

Longer explanation explaining exactly what's changed and why, whether
any external or private interfaces changed, what bugs were fixed (with
bug tracker reference if applicable) and so forth. Be concise but not
too brief.

Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1
```

### The Tag

The tag tells which part of the software is affected; it must be in kebab-case,
it shouldn't include fle extensions, and it shouldn't have the `retro-` prefix.

If the change is too global to be relevant to any specific part, you can omit
the tag completely.

Good tags:
```
core-view
gtk
runner
meson
ci
```

Bad tags:
```
retro-core-view
retro-gtk
retro-runner
meson.build
CI
```

### The Short Explanation

The short explanation must be brief and describe the change as shortly as
possible. You should describe the change and not the intended effect, this is
left to the long explanation. Use the imperative form, start with a capital
letter (if relevant) and don't use terminal punctuation.

No need to prefix the full namespace to methods you mention if the type is in
the tag already; you can refer to them via `*_set_property()` or via
`the set_property() method`. No need to specify the accessors of a property,
just mention the property's name.

The short explanation is the only part of a commit that can't be omitted.

Try to keep the first line (hence, the tag and the short explanation) under 50
characters, but longer lines are accepted if agreed they can't be shortened
without losing useful information.

Good short explanations:
```
Add HACKING.md
gtk: Add RetroMyType
my-type: Add the my_method() method
```

Bad short explanations:
```
Add RetroMyType
gtk: Add RetroMyType.
gtk: add RetroMyType
gtk: This adds RetroMyType
my-type: Add retro_my_type_my_method()
```

### The Long Explanation

The long explanations lets you describe anything that you deem important and
can't be explained by the short explanation only. Use as many lines as you need,
and leave an empty line between each paragraph. Try to limit the lines to 75
characters, but try to fill lines as much as possible withinh that limit. Do not
break function names or URLs, give them their own line if they are too large to
fit within the a single one.

Use proper prose with proper punctuation.

Good long explanations:
```
This gives retro_my_type_my_method() to RetroMyType to allow…
```

Bad long explanations:
```
Give retro_my_type_my_method()
to RetroMyType to allow…
```

### The Issue Reference

If there is an issue fixed by your commit, specify it at the end of your commit
message. It must be in the following format: `Fixes $ISSUE_URL`.

Do not simply use the issue number.

Good issue references:
```
Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1
```

Bad issue references:
```
Fixes #1
Fix https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1
Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1.
```
