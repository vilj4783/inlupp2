#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "db_backend.h"

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

void test_create_destroy_database(void)
{
    database_t *db = create_database();
    CU_ASSERT_PTR_NOT_NULL(db);
    // destroy_database(db);
}

void test_add_merch(void)
{
    database_t *db = create_database();

    CU_ASSERT_TRUE(backend_add_merch_execute("Apple", "Red fruit", 10, db));

    CU_ASSERT_FALSE(backend_add_merch_execute("Apple", "Different desc", 20, db));

    CU_ASSERT_TRUE(backend_add_merch_execute("Banana", "Yellow fruit", 15, db));

    // destroy_database(db);
}

void test_list_merch(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Desc", 10, db);
    backend_add_merch_execute("Banana", "Desc", 15, db);

    ioopm_list_t *list = backend_list_merch_execute(db);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(ioopm_list_size(list), 2);

    ioopm_list_destroy(list);
    // destroy_database(db);
}

void test_replenish(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);

    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "12A", 10));  // Wrong shelfname
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A", 10));    // Wrong shelfname
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A01", -5));   // Negative number
    CU_ASSERT_FALSE(backend_replenish_execute(db, "NonExistent", "A01", 10)); // Item doesn't exist

    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 10));

    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 5));

    CU_ASSERT_FALSE(backend_replenish_execute(db, "Orange", "A01", 8));

    CU_ASSERT_TRUE(backend_replenish_execute(db, "Orange", "B01", 8));

    // destroy_database(db);
}

void test_remove_merch(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_replenish_execute(db, "Apple", "A01", 20);

    CU_ASSERT_TRUE(backend_remove_merch_execute("Apple", db));

    CU_ASSERT_FALSE(backend_remove_merch_execute("Apple", db));

    backend_add_merch_execute("Pear", "Fruit", 14, db);
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Pear", "A01", 15));

    // destroy_database(db);
}

void test_edit_merch(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_replenish_execute(db, "Apple", "A01", 20);

    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "Apple", "Green fruit", 12));

    CU_ASSERT_FALSE(backend_edit_merch_execute(db, "Apple", "Orange", "Desc", 10));

    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "GreenApple", "Green fruit", 12));

    CU_ASSERT_FALSE(backend_remove_merch_execute("Apple", db));

    CU_ASSERT_TRUE(backend_replenish_execute(db, "GreenApple", "A01", 5));

    // destroy_database(db);
}
void test_create_remove_cart(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Pear", "Fruit", 15, db);
    backend_replenish_execute(db, "Apple", "A01", 20);
    backend_create_cart_execute(db); // cart 0
    backend_create_cart_execute(db); // cart 1

    backend_add_to_cart_execute(0, "Apple", 1, db);
    backend_add_to_cart_execute(0, "Pear", 2, db);

    backend_add_to_cart_execute(1, "Orange", 1, db);
    backend_add_to_cart_execute(1, "Banana", 2, db);

    CU_ASSERT_EQUAL(40, backend_calculate_cost_execute(0, db));
    CU_ASSERT_EQUAL(12, backend_calculate_cost_execute(1, db));

    CU_ASSERT(backend_remove_cart_execute(0, db));
    CU_ASSERT(backend_remove_cart_execute(1, db));
}


void test_add_to_remove_from_cart(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Pear", "Fruit", 15, db);
    backend_replenish_execute(db, "Apple", "A01", 20);

    backend_create_cart_execute(db); // cart 0
    backend_create_cart_execute(db); // cart 1
    backend_create_cart_execute(db); // cart 2

    backend_add_to_cart_execute(0, "Apple", 1, db);
    backend_add_to_cart_execute(0, "Pear", 3, db);

    backend_add_to_cart_execute(1, "Orange", 1, db);
    backend_add_to_cart_execute(1, "Banana", 2, db);

    CU_ASSERT_EQUAL(55, backend_calculate_cost_execute(0, db));
    CU_ASSERT_EQUAL(12, backend_calculate_cost_execute(1, db));
    CU_ASSERT_EQUAL(0, backend_calculate_cost_execute(2, db));

    CU_ASSERT(backend_remove_cart_execute(0, db));
    CU_ASSERT(backend_remove_cart_execute(1, db));
    CU_ASSERT(backend_remove_cart_execute(2, db));
    CU_ASSERT_FALSE(backend_remove_cart_execute(3, db)); // nonexisting cart
    

}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    CU_pSuite pSuite = CU_add_suite("Database Backend Suite", init_suite, clean_suite);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        // (NULL == CU_add_test(pSuite, "Create and Destroy database", test_create_destroy_database)) ||
        // (NULL == CU_add_test(pSuite, "Add Merchandise", test_add_merch)) ||
        // (NULL == CU_add_test(pSuite, "List Merchandise", test_list_merch)) ||
        // (NULL == CU_add_test(pSuite, "Replenish Stock", test_replenish)) ||
        // (NULL == CU_add_test(pSuite, "Remove Merchandise", test_remove_merch)) ||
        // (NULL == CU_add_test(pSuite, "Edit Merchandise", test_edit_merch)) ||
        (NULL == CU_add_test(pSuite, "add and remove a cart", test_create_remove_cart)) ||
        (NULL == CU_add_test(pSuite, "add to and remove from cart", test_add_to_remove_from_cart))
    )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}