#include <wge/error/storage.hpp>

#include <wtest/wtest.hpp>

struct construction_tracker {
    construction_tracker() : default_constructed(true) {}
    construction_tracker(const construction_tracker&) : copy_constructed(true) {}
    construction_tracker(construction_tracker&&) : move_constructed(true) {}

    construction_tracker& operator=(const construction_tracker&) {
        copy_constructed = true;
        assignment       = true;
        return *this;
    }
    construction_tracker& operator=(construction_tracker&&) {
        move_constructed = true;
        assignment       = true;
        return *this;
    }

    bool default_constructed = false;
    bool copy_constructed    = false;
    bool move_constructed    = false;
    bool assignment          = false;
};

WTEST_SUITE(WgeErrorStorage);

WTEST_SETUP(WgeErrorStorage) {}
WTEST_TEAR_DOWN(WgeErrorStorage) {}

WTEST_CASE(WgeErrorStorage, storage_impl_construction) {
    using storage_type = wge::detail::storage_impl<construction_tracker, construction_tracker>;
    storage_type test_storage{};
    WTEST_ASSERT_FALSE(test_storage.has_value());
}

WTEST_CASE(WgeErrorStorage, storage_impl_construct_value) {}

WTEST_SUITE_RUNNER(WgeErrorStorage) {
    RUN_TEST_CASE(WgeErrorStorage, storage_impl_construction);
    RUN_TEST_CASE(WgeErrorStorage, storage_impl_construct_value);
}
