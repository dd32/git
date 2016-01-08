#include "../git-compat-util.h"
#include "../strbuf.h"

/* Adapted from libiberty's basename.c.  */
char *gitbasename (char *path)
{
	const char *base;

	if (path)
		skip_dos_drive_prefix(&path);

	if (!path || !*path)
		return ".";

	for (base = path; *path; path++) {
		if (!is_dir_sep(*path))
			continue;
		do {
			path++;
		} while (is_dir_sep(*path));
		if (*path)
			base = path;
		else
			while (--path != base && is_dir_sep(*path))
				*path = '\0';
	}
	return (char *)base;
}

char *gitdirname(char *path)
{
	char *p = path, *slash = NULL, c;
	int dos_drive_prefix;

	if (!p)
		return ".";

	if ((dos_drive_prefix = skip_dos_drive_prefix(&p)) && !*p) {
		static struct strbuf buf = STRBUF_INIT;

dot:
		strbuf_reset(&buf);
		strbuf_addf(&buf, "%.*s.", dos_drive_prefix, path);
		return buf.buf;
	}

	/*
	 * POSIX.1-2001 says dirname("/") should return "/", and dirname("//")
	 * should return "//", but dirname("///") should return "/" again.
	 */
	if (is_dir_sep(*p)) {
		if (!p[1] || (is_dir_sep(p[1]) && !p[2]))
			return path;
		slash = ++p;
	}
	while ((c = *(p++)))
		if (is_dir_sep(c)) {
			char *tentative = p - 1;

			/* POSIX.1-2001 says to ignore trailing slashes */
			while (is_dir_sep(*p))
				p++;
			if (*p)
				slash = tentative;
		}

	if (!slash)
		goto dot;
	*slash = '\0';
	return path;
}
