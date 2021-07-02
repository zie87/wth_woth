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
    storage_type test_storage{wge::detail::no_init};
    WTEST_ASSERT_FALSE(test_storage.has_value());
}

WTEST_CASE(WgeErrorStorage, storage_impl_construct_value) {
    using storage_type = wge::detail::storage_impl<construction_tracker, wge::i32>;
    {
        storage_type test_storage{wge::detail::no_init};

        const construction_tracker tracker;
        test_storage.construct_value(tracker);
        WTEST_ASSERT_TRUE(test_storage.value().copy_constructed);
    }

    {
        storage_type test_storage{wge::detail::no_init};

        test_storage.construct_value(construction_tracker());
        WTEST_ASSERT_TRUE(test_storage.value().move_constructed);
    }
}

WTEST_CASE(WgeErrorStorage, storage_impl_construct_error) {
    using storage_type = wge::detail::storage_impl<wge::f32, construction_tracker>;
    {
        storage_type test_storage{wge::detail::no_init};

        const construction_tracker tracker;
        test_storage.construct_error(tracker);
        WTEST_ASSERT_TRUE(test_storage.error().copy_constructed);
    }

    {
        storage_type test_storage{wge::detail::no_init};

        test_storage.construct_error(construction_tracker());
        WTEST_ASSERT_TRUE(test_storage.error().move_constructed);
    }
}

WTEST_SUITE_RUNNER(WgeErrorStorage) {
    RUN_TEST_CASE(WgeErrorStorage, storage_impl_construction);
    RUN_TEST_CASE(WgeErrorStorage, storage_impl_construct_value);
    RUN_TEST_CASE(WgeErrorStorage, storage_impl_construct_error);
}
