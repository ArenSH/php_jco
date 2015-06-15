PHP_ARG_ENABLE(jco, Enable jco extension,[ --enable-jco  Enable jco support])

if test "$PHP_JCO" = "yes"; then
  AC_DEFINE(HAVE_JCO, 1, [Whether you have JCollections])
  PHP_NEW_EXTENSION(jco, jco.c, $ext_shared)
fi