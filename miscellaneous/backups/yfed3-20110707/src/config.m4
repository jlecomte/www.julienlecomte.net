PHP_ARG_ENABLE(yfed3,
    whether to enable YFed3 support,
    [ --enable-yfed3 Enable Yfed3 support])

if test $PHP_YAHOO_YFED3 != "no"; then
    PHP_REQUIRE_CXX()
    AC_DEFINE(HAVE_YFED3, 1, [Whether you have YFed3 support])
    PHP_NEW_EXTENSION(yfed3, php_yfed3.cpp, $ext_shared)
fi
