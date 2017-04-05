## Process this file with automake to produce Makefile.in

NULL =

SUBDIRS = retro-gtk po

dist_doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS \
	$(NULL)


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	$(NULL)

EXTRA_DIST = \
	$(INTLTOOL_FILES) \
	$(NULL)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	.flatpak-builder \
	$(NULL)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

-include $(top_srcdir)/git.mk
