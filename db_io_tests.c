#include "db_io.h"
#include <CUnit/Basic.h>

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

// These are example test functions. You should replace them with
// functions of your own.
void test_ask_question_action(void) {
  CU_ASSERT_EQUAL(ask_question_action("input: A"), ADD_MERCH);
  CU_ASSERT_EQUAL(ask_question_action("input: a"), ADD_MERCH);

  CU_ASSERT_EQUAL(ask_question_action("input: L"), LIST_MERCH);
  CU_ASSERT_EQUAL(ask_question_action("input: l"), LIST_MERCH);

  CU_ASSERT_EQUAL(ask_question_action("input: D"), REMOVE_MERCH);
  CU_ASSERT_EQUAL(ask_question_action("input: d"), REMOVE_MERCH);

  CU_ASSERT_EQUAL(ask_question_action("input: E"), EDIT_MERCH);
  CU_ASSERT_EQUAL(ask_question_action("input: e"), EDIT_MERCH);

  CU_ASSERT_EQUAL(ask_question_action("input: C"), CREATE_CART);
  CU_ASSERT_EQUAL(ask_question_action("input: c"), CREATE_CART);

  CU_ASSERT_EQUAL(ask_question_action("input: R"), REMOVE_CART);
  CU_ASSERT_EQUAL(ask_question_action("input: r"), REMOVE_CART);

  CU_ASSERT_EQUAL(ask_question_action("input: +"), ADD_TO_CART);
  CU_ASSERT_EQUAL(ask_question_action("input: -"), REMOVE_FROM_CART);
  CU_ASSERT_EQUAL(ask_question_action("input: ="), CALCULATE_COST);

  CU_ASSERT_EQUAL(ask_question_action("input: O"), CHECKOUT);
  CU_ASSERT_EQUAL(ask_question_action("input: o"), CHECKOUT);

  CU_ASSERT_EQUAL(ask_question_action("input: U"), UNDO);
  CU_ASSERT_EQUAL(ask_question_action("input: u"), UNDO);

  CU_ASSERT_EQUAL(ask_question_action("input: Q"), QUIT);
  CU_ASSERT_EQUAL(ask_question_action("input: q"), QUIT);

}

void test_ask_question_confirm(void) {
  CU_ASSERT(ask_question_confirm_Y("input: Y"));
  CU_ASSERT(ask_question_confirm_Y("input: y"));

  CU_ASSERT_FALSE(ask_question_confirm_Y("input: o"));
  CU_ASSERT_FALSE(ask_question_confirm_Y("input: p"));

  CU_ASSERT_FALSE(ask_question_confirm_N("input: N"));
  CU_ASSERT_FALSE(ask_question_confirm_N("input: n"));

  CU_ASSERT(ask_question_confirm_N("input: o"));
  CU_ASSERT(ask_question_confirm_N("input: p"));
}

int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
  if (my_test_suite == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
    (CU_add_test(my_test_suite, "test ask_question_action", test_ask_question_action) == NULL) ||
    (CU_add_test(my_test_suite, "test ask_question_confirm (Y & N)", test_ask_question_confirm) == NULL) ||
    0
  )
    {
      // If adding any of the tests fails, we tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
    }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_NORMAL);

  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
}
