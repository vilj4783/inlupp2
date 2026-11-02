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

void test_list_merch_sorting(void)
{
    database_t *db = create_database();
    
    // Lägg till i osorterad ordning
    backend_add_merch_execute("Orange", "Fruit", 12, db);
    backend_add_merch_execute("Apple", "Fruit", 10, db);
    backend_add_merch_execute("Banana", "Fruit", 15, db);
    backend_add_merch_execute("Avocado", "Fruit", 25, db);

    ioopm_list_t *list = backend_list_merch_execute(db);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(ioopm_list_size(list), 4);

    // Kontrollera att listan är sorterad i alfabetisk ordning: Apple -> Avocado -> Banana -> Orange
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

    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "12A", 10));      // Fel hyllformat
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A", 10));        // För kort
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A1", 10));       // Endast en siffra
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "a01", 10));      // Liten bokstav
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A01", 0));        // 0 antal ej tillåtet
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Apple", "A01", -5));       // Negativt antal ej tillåtet
    CU_ASSERT_FALSE(backend_replenish_execute(db, "NonExistent", "A01", 10)); // Varan finns inte

    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 10));
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Apple", "A01", 5));

    // Hyllan A01 är bunden till Apple, Orange ska inte få läggas på samma hylla
    CU_ASSERT_FALSE(backend_replenish_execute(db, "Orange", "A01", 8));
    CU_ASSERT_TRUE(backend_replenish_execute(db, "Orange", "B01", 8));

    backend_quit_execute(db);
}

void test_show_stock(void)
{
    database_t *db = create_database();
    backend_add_merch_execute("Apple", "Fruit", 10, db);

    // 1. Icke-existerande vara ska ge NULL
    ioopm_list_t *no_item_stock = backend_show_stock_execute(db, "GhostFruit");
    CU_ASSERT_PTR_NULL(no_item_stock);

    // 2. Existerande vara utan lager ska ge en tom lista (storlek 0)
    ioopm_list_t *empty_stock = backend_show_stock_execute(db, "Apple");
    CU_ASSERT_PTR_NOT_NULL(empty_stock);
    CU_ASSERT_EQUAL(ioopm_list_size(empty_stock), 0);
    ioopm_list_destroy(empty_stock);

    // 3. Fyll på lager på hyllor i osorterad ordning (C01, A05, B02)
    backend_replenish_execute(db, "Apple", "C01", 10);
    backend_replenish_execute(db, "Apple", "A05", 25);
    backend_replenish_execute(db, "Apple", "B02", 5);

    ioopm_list_t *stock_list = backend_show_stock_execute(db, "Apple");
    CU_ASSERT_PTR_NOT_NULL(stock_list);
    CU_ASSERT_EQUAL(ioopm_list_size(stock_list), 3);

    // Kontrollera att hyllorna sorteras alfabetiskt: A05 -> B02 -> C01
    shelf_t *s0 = ioopm_list_get(stock_list, 0).p;
    shelf_t *s1 = ioopm_list_get(stock_list, 1).p;
    shelf_t *s2 = ioopm_list_get(stock_list, 2).p;

    CU_ASSERT_STRING_EQUAL(s0->shelf_name, "A05");
    CU_ASSERT_EQUAL(s0->quantity, 25);

    CU_ASSERT_STRING_EQUAL(s1->shelf_name, "B02");
    CU_ASSERT_EQUAL(s1->quantity, 5);

    CU_ASSERT_STRING_EQUAL(s2->shelf_name, "C01");
    CU_ASSERT_EQUAL(s2->quantity, 10);

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

    // När Apple tagits bort ska hyllan A01 vara fri att användas av en ny vara
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

    // Redigera vara som inte finns
    CU_ASSERT_FALSE(backend_edit_merch_execute(db, "Pineapple", "NewPineapple", "Desc", 10));

    // Ändra beskrivning och pris under samma namn
    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "Apple", "Green fruit", 12));

    // Ändra namn till ett namn som redan är upptaget
    CU_ASSERT_FALSE(backend_edit_merch_execute(db, "Apple", "Orange", "Desc", 10));

    // Byta till ett unikt namn
    CU_ASSERT_TRUE(backend_edit_merch_execute(db, "Apple", "GreenApple", "Green fruit", 12));

    // Gamla namnet ska inte finnas kvar
    CU_ASSERT_FALSE(backend_remove_merch_execute("Apple", db));

    // Hyllan A01 ska nu vara bunden till GreenApple
    CU_ASSERT_TRUE(backend_replenish_execute(db, "GreenApple", "A01", 5));

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
        (NULL == CU_add_test(pSuite, "List Merchandise Sorting", test_list_merch_sorting)) ||
        (NULL == CU_add_test(pSuite, "Replenish Stock", test_replenish)) ||
        (NULL == CU_add_test(pSuite, "Show Stock", test_show_stock)) ||
        (NULL == CU_add_test(pSuite, "Remove Merchandise", test_remove_merch)) ||
        (NULL == CU_add_test(pSuite, "Edit Merchandise", test_edit_merch))
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