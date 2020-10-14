## Commit Message Style

Commit messages have 4 parts: a tag, a short explanation, a long explanation,
and an issue reference.

```
tag: Short explanation of the commit

Longer explanation explaining exactly what's changed and why, whether
any external or private interfaces changed, what bugs were fixed (with
bug tracker reference if applicable) and so forth. Be concise but not
too brief.

Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1.
```

### The Tag

The tag tells which part of the software is affected; it must be in kebab-case,
it shouldn't include fle extensions, and it shouldn't have the *retro-* prefix.

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
message. It must be in the following format: `Fixes $ISSUE_URL.`.

Do not simply use the issue number.

Good issue references:
```
Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1.
```

Bad issue references:
```
Fixes #1
Fix https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1.
Fixes https://gitlab.gnome.org/GNOME/retro-gtk/-/issues/1
```
