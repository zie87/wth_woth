#include <JRenderer.h>

#include <wtest/wtest.hpp>

WTEST_SUITE(JgeJRenderer);

WTEST_SETUP(JgeJRenderer) {}
WTEST_TEAR_DOWN(JgeJRenderer) {}

struct TestRenderer : JRenderer {};

WTEST_CASE(JgeJRenderer, image_processing) {
    { TestRenderer renderer; }
}

WTEST_SUITE_RUNNER(JgeJRenderer) { RUN_TEST_CASE(JgeJRenderer, image_processing); }
