#include <JRenderer.h>

#include <wtest/wtest.hpp>

WTEST_SUITE(JgeJRenderer);

WTEST_SETUP(JgeJRenderer) {}
WTEST_TEAR_DOWN(JgeJRenderer) {}

WTEST_CASE(JgeJRenderer, image_processing) {}

WTEST_SUITE_RUNNER(JgeJRenderer) { RUN_TEST_CASE(JgeJRenderer, image_processing); }
