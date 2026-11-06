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
    backend_quit_execute(db);
}

void test_add_merch(void)
{
    database_t *db = create_database();

    CU_ASSERT_TRUE(backend_add_merch_execute("Apple", "Red fruit", 10, db));
    CU_ASSERT_FALSE(backend_add_merch_execute("Apple", "Different desc", 20, db));
    CU_ASSERT_TRUE(backend_add_merch_execute("Banana", "Yellow fruit", 15, db));

    backend_quit_execute(db);
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
    backend_quit_execute(db);
}

void test_list_merch_empty(void)
{
    database_t *db = create_database();

    ioopm_list_t *list = backend_list_merch_execute(db);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(ioopm_list_size(list), 0);

    ioopm_list_destroy(list);
    backend_quit_execute(db);
}

void test_list_merch_sorting(void)
{
    database_t *db = create_database();
    
    // Add items in unsorted order
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Banana", "Fruit", 15, db);
    backend_add_merch_execute("Avocado", "Fruit", 25, db);

    ioopm_list_t *list = backend_list_merch_execute(db);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(ioopm_list_size(list), 4);

    // Verify alphabetical sort order
    CU_ASSERT_STRING_EQUAL(ioopm_list_get(list, 0).s, "Apple");
    CU_ASSERT_STRING_EQUAL(ioopm_list_get(list, 1).s, "Avocado");
    CU_ASSERT_STRING_EQUAL(ioopm_list_get(list, 2).s, "Banana");
    CU_ASSERT_STRING_EQUAL(ioopm_list_get(list, 3).s, "Orange");

    ioopm_list_destroy(list);
    backend_quit_execute(db);
}

void test_replenish(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);

    // Invalid shelf formats and quantities
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "12A", 10));       // Starts with digits
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A", 10));         // Too short
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A1", 10));        // Only one digit
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "a01", 10));       // Lowercase letter
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A01", 0));        // Quantity must be > 0
    CU_ASSERT_FALSE(backend_replenish_execute(db, "NonExistent", "A01", 10)); // Merch does not exist

    // Valid replenishments
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 10));
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 5));

    // Shelf A01 is connected with Apple, Orange can't be added to same shelf
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Orange", "A01", 8));
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Orange", "B01", 8));

    backend_quit_execute(db);
}

void test_show_stock(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);

    // Non-existent merchandise should return NULL
    ioopm_list_t *no_item_stock = backend_show_stock_execute(db, "GhostFruit");
    CU_ASSERT_PTR_NULL(no_item_stock);

    // Existing merchandise with no stock should return an empty list
    ioopm_list_t *empty_stock = backend_show_stock_execute(db, "Apple");
    CU_ASSERT_PTR_NOT_NULL(empty_stock);
    CU_ASSERT_EQUAL(ioopm_list_size(empty_stock), 0);
    ioopm_list_destroy(empty_stock);

    // Replenish shelves
    backend_replenish_execute(db, "Apple", "C01", 10);
    backend_replenish_execute(db, "Apple", "A05", 25);
    backend_replenish_execute(db, "Apple", "B02", 5);
    backend_replenish_execute(db, "Apple", "Z99", 50);

    ioopm_list_t *stock_list = backend_show_stock_execute(db, "Apple");
    CU_ASSERT_PTR_NOT_NULL(stock_list);
    CU_ASSERT_EQUAL(ioopm_list_size(stock_list), 4);

    // Verify alphabetical sorting of shelves: A05 -> B02 -> C01 -> Z99
    shelf_t *s0 = ioopm_list_get(stock_list, 0).p;
    shelf_t *s1 = ioopm_list_get(stock_list, 1).p;
    shelf_t *s2 = ioopm_list_get(stock_list, 2).p;
    shelf_t *s3 = ioopm_list_get(stock_list, 3).p;

    CU_ASSERT_STRING_EQUAL(backend_get_location_name(s0), "A05");
    CU_ASSERT_EQUAL(backend_get_location_quantity(s0), 25);

    CU_ASSERT_STRING_EQUAL(backend_get_location_name(s1), "B02");
    CU_ASSERT_EQUAL(backend_get_location_quantity(s1), 5);

    CU_ASSERT_STRING_EQUAL(backend_get_location_name(s2), "C01");
    CU_ASSERT_EQUAL(backend_get_location_quantity(s2), 10);

    CU_ASSERT_STRING_EQUAL(backend_get_location_name(s2), "Z99");
    CU_ASSERT_EQUAL(backend_get_location_quantity(s2), 50);

    ioopm_list_destroy(stock_list);
    backend_quit_execute(db);
}

void test_remove_merch(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_replenish_execute(db, "Apple", "A01", 20);

    CU_ASSERT_TRUE(backend_remove_merch_execute("Apple", db));
    CU_ASSERT_FALSE(backend_remove_merch_execute("Apple", db));

    // Once Apple is removed, shelf A01 must be available for a new item
    backend_add_merch_execute("Pear", "Fruit", 14, db);
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Pear", "A01", 15));

    backend_quit_execute(db);
}

void test_edit_merch(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_replenish_execute(db, "Apple", "A01", 20);

    // Editing a non-existent item should fail
    CU_ASSERT_FALSE(backend_edit_merch_execute(db, "Pineapple", "NewPineapple", "Desc", 10));

    // Update description and price while keeping the same name
    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "Apple", "Green fruit", 12));

    // Renaming to a name already in use should fail
    CU_ASSERT_FALSE(backend_edit_merch_execute(db, "Apple", "Orange", "Desc", 10));

    // Rename to an available unique name
    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "GreenApple", "Green fruit", 12));

    // Old name must no longer exist in the database
    CU_ASSERT_FALSE(backend_remove_merch_execute("Apple", db));

    // Shelf A01 must now be linked to GreenApple
    CU_ASSERT_TRUE(backend_replenish_execute(db, "GreenApple", "A01", 5));

    backend_quit_execute(db);
}

void test_create_remove_cart(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Pear", "Fruit", 15, db);

    backend_replenish_execute(db, "Apple", "A01", 20);
    backend_replenish_execute(db, "Orange", "B01", 20);
    backend_replenish_execute(db, "Pear", "C01", 2);

    backend_create_cart_execute(db); // cart 1
    backend_create_cart_execute(db); // cart 2

    CU_ASSERT(backend_add_to_cart_execute(1, "Apple", 1, db));

    CU_ASSERT_FALSE(backend_add_to_cart_execute(1, "Pear", 3, db)); // more than exists
    CU_ASSERT(backend_add_to_cart_execute(1, "Pear", 2, db)); // should work
    CU_ASSERT_FALSE(backend_add_to_cart_execute(1, "Pear", 2, db)); // already taken

    CU_ASSERT(backend_add_to_cart_execute(2, "Orange", 1, db));
    CU_ASSERT_FALSE(backend_add_to_cart_execute(2, "Banana", 2, db));

    CU_ASSERT_EQUAL(40, backend_calculate_cost_execute(1, db));
    CU_ASSERT_EQUAL(12, backend_calculate_cost_execute(2, db));

    CU_ASSERT(backend_remove_cart_execute(1, db));
    CU_ASSERT(backend_remove_cart_execute(2, db));

    backend_quit_execute(db);
}


void test_add_to_remove_from_cart(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Pear", "Fruit", 15, db);

    backend_replenish_execute(db, "Apple", "A01", 1);
    backend_replenish_execute(db, "Orange", "B01", 20);
    backend_replenish_execute(db, "Pear", "C01", 6);

    backend_create_cart_execute(db); // cart 1
    backend_create_cart_execute(db); // cart 2
    backend_create_cart_execute(db); // cart 3

    CU_ASSERT(backend_add_to_cart_execute(1, "Apple", 1, db));
    CU_ASSERT(backend_add_to_cart_execute(1, "Pear", 5, db));
    CU_ASSERT(backend_remove_from_cart_execute(1, "Pear", 2, db));

    CU_ASSERT(backend_add_to_cart_execute(2, "Orange", 1, db));
    CU_ASSERT_FALSE(backend_add_to_cart_execute(2, "Banana", 2, db));
    CU_ASSERT(backend_add_to_cart_execute(2, "Pear", 2, db));

    CU_ASSERT_FALSE(backend_remove_from_cart_execute(2, "Banana", 2, db));
    CU_ASSERT(backend_remove_from_cart_execute(2, "Orange", 2, db));
    CU_ASSERT_EQUAL(55, backend_calculate_cost_execute(1, db));
    CU_ASSERT_EQUAL(30, backend_calculate_cost_execute(2, db));
    CU_ASSERT_EQUAL(0, backend_calculate_cost_execute(3, db));

    CU_ASSERT(backend_checkout_execute(1, db));
    
    ioopm_list_t *pear_list = backend_show_stock_execute(db, "Pear");
    ioopm_list_t *apple_list = backend_show_stock_execute(db, "Apple");
    
    CU_ASSERT_FALSE(ioopm_list_is_empty(pear_list));
    CU_ASSERT(ioopm_list_is_empty(apple_list));

    ioopm_list_destroy(pear_list);
    ioopm_list_destroy(apple_list);

    CU_ASSERT(backend_add_to_cart_execute(2, "Pear", 1, db));    
    CU_ASSERT(backend_remove_merch_execute("Pear", db));

    CU_ASSERT(backend_remove_cart_execute(2, db));
    CU_ASSERT(backend_remove_cart_execute(3, db));
    
    CU_ASSERT_FALSE(backend_remove_cart_execute(4, db)); // nonexisting cart
    
    backend_quit_execute(db);

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
        (NULL == CU_add_test(pSuite, "Create and Destroy database", test_create_destroy_database)) ||
        (NULL == CU_add_test(pSuite, "Add Merchandise", test_add_merch)) ||
        (NULL == CU_add_test(pSuite, "List Merchandise", test_list_merch)) ||
        (NULL == CU_add_test(pSuite, "List Merchandise Empty Database", test_list_merch_empty)) ||
        (NULL == CU_add_test(pSuite, "List Merchandise Sorting", test_list_merch_sorting)) ||
        (NULL == CU_add_test(pSuite, "Replenish Stock", test_replenish)) ||
        (NULL == CU_add_test(pSuite, "Show Stock", test_show_stock)) ||
        (NULL == CU_add_test(pSuite, "Remove Merchandise", test_remove_merch)) ||
        (NULL == CU_add_test(pSuite, "Edit Merchandise", test_edit_merch)) ||
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