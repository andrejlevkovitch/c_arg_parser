#include "arg_parser.h"
#include <assert.h>
#include <string.h>


void check_arg_parser_create_and_dispose_only_with_desc(const char *desc) {
  arg_parser *parser = arg_parser_make(desc);
  char *      usage  = arg_parser_usage(parser);
  int         result = 0;
  if (desc == NULL || strlen(desc) == 0) {
    result = strcmp(usage, "");
    assert(result == 0 && "non empty usage");
  } else {
    result = strncmp(usage, desc, strlen(desc));
    assert(result == 0 && "unexpected usage string");
    assert(strlen(usage) == strlen(desc) + 1 /*new line symbol*/ &&
           "unexpected diff");
  }
  free(usage);
  arg_parser_dispose(parser);
}

void check_arg_flags() {
  arg_parser *parser = arg_parser_make("main desc:");

  ARG_PARSER_ADD_STR(parser, "string", 0, "not required string", false);
  ARG_PARSER_ADD_STR(parser, "string_req", 's', "required string", true);
  ARG_PARSER_ADD_STRD(parser,
                      "string_def",
                      0,
                      "string with default value",
                      "default");

  ARG_PARSER_ADD_INT(parser, "int", 0, "not required int", false);
  ARG_PARSER_ADD_INT(parser, "int_req", 'i', "required int", true);
  ARG_PARSER_ADD_INTD(parser, "int_def", 0, "int with default value", 10);

  ARG_PARSER_ADD_LONG(parser, "long", 0, "not required long", false);
  ARG_PARSER_ADD_LONG(parser, "long_req", 'l', "required long", true);
  ARG_PARSER_ADD_LONGD(parser, "long_def", 0, "long with default value", 9);

  ARG_PARSER_ADD_LL(parser, "long_long", 0, "not required long long", false);
  ARG_PARSER_ADD_LL(parser, "long_long_req", 't', "required long long", true);
  ARG_PARSER_ADD_LLD(parser,
                     "long_long_def",
                     0,
                     "long long with default value",
                     8);

  ARG_PARSER_ADD_DOUBLE(parser, "double", 0, "not required double", false);
  ARG_PARSER_ADD_DOUBLE(parser, "double_req", 'd', "required double", true);
  ARG_PARSER_ADD_DOUBLED(parser,
                         "double_def",
                         0,
                         "double with default value",
                         7.6);

  ARG_PARSER_ADD_BOOL(parser, "bool", 0, "not required bool", false);
  ARG_PARSER_ADD_BOOL(parser, "bool_req", 'b', "required bool", true);
  ARG_PARSER_ADD_BOOLD(parser, "bool_def", 0, "bool with default value", true);


  const char *target_usage =
      "main desc:\n"
      "      --string                 not required string\n"
      "  -s, --string-req             required string\n"
      "      --string-def (=default)  string with default value\n"
      "      --int                    not required int\n"
      "  -i, --int-req                required int\n"
      "      --int-def (=10)          int with default value\n"
      "      --long                   not required long\n"
      "  -l, --long-req               required long\n"
      "      --long-def (=9)          long with default value\n"
      "      --long-long              not required long long\n"
      "  -t, --long-long-req          required long long\n"
      "      --long-long-def (=8)     long long with default value\n"
      "      --double                 not required double\n"
      "  -d, --double-req             required double\n"
      "      --double-def (=7.600000) double with default value\n"
      "      --bool                   not required bool\n"
      "  -b, --bool-req               required bool\n"
      "      --bool-def (=true)       bool with default value\n";

  char *usage  = arg_parser_usage(parser);
  int   result = strcmp(usage, target_usage);
  assert(result == 0);
  free(usage);


  int argc = 13;
  // clang-format off
  char *argv[] = {"program_name",
                  "--string-req",     "string",
                  "--int-req",        "1",
                  "--long-req",       "2",
                  "--long-long-req",  "3",
                  "--double-req",     "10.5",
                  "--bool-req",       "false",
  };
  // clang-format on
  result = ARG_PARSER_PARSE(parser, argc, argv, false, NULL);
  assert(result == 0);


  const char *str          = NULL;
  int         int_value    = 0;
  long        long_value   = 0;
  long long   ll_value     = 0;
  double      double_value = 0;
  bool        bool_value   = 0;

  assert(ARG_PARSER_GET_STR(parser, "string", str) == 0);
  assert(ARG_PARSER_GET_INT(parser, "int", int_value) == 0);
  assert(ARG_PARSER_GET_LONG(parser, "long", long_value) == 0);
  assert(ARG_PARSER_GET_LL(parser, "long-long", ll_value) == 0);
  assert(ARG_PARSER_GET_DOUBLE(parser, "double", double_value) == 0);
  assert(ARG_PARSER_GET_BOOL(parser, "bool", bool_value) == 0);


  ARG_PARSER_GET_STR(parser, "string-req", str);
  ARG_PARSER_GET_INT(parser, "int-req", int_value);
  ARG_PARSER_GET_LONG(parser, "long-req", long_value);
  ARG_PARSER_GET_LL(parser, "long-long-req", ll_value);
  ARG_PARSER_GET_DOUBLE(parser, "double-req", double_value);
  ARG_PARSER_GET_BOOL(parser, "bool-req", bool_value);

  assert(strcmp(str, "string") == 0);
  assert(int_value == 1);
  assert(long_value == 2);
  assert(ll_value == 3);
  assert(double_value > 10.4 && double_value < 10.6);
  assert(bool_value == false);


  ARG_PARSER_GET_STR(parser, "string-def", str);
  ARG_PARSER_GET_INT(parser, "int-def", int_value);
  ARG_PARSER_GET_LONG(parser, "long-def", long_value);
  ARG_PARSER_GET_LL(parser, "long-long-def", ll_value);
  ARG_PARSER_GET_DOUBLE(parser, "double-def", double_value);
  ARG_PARSER_GET_BOOL(parser, "bool-def", bool_value);

  assert(strcmp(str, "default") == 0);
  assert(int_value == 10);
  assert(long_value == 9);
  assert(ll_value == 8);
  assert(double_value > 7.5 && double_value < 7.7);
  assert(bool_value == true);


  arg_parser_dispose(parser);
}

void check_unknown_flag_fail() {
  arg_parser *parser = arg_parser_make(NULL);

  int   argc   = 2;
  char *argv[] = {"program", "--unknown"};
  int   result = ARG_PARSER_PARSE(parser, argc, argv, false, NULL);
  assert(result != 0);

  arg_parser_dispose(parser);
}

void check_unknown_flag_not_fail() {
  arg_parser *parser = arg_parser_make(NULL);

  int   argc   = 2;
  char *argv[] = {"program", "--unknown"};
  int   result = ARG_PARSER_PARSE(parser, argc, argv, true, NULL);
  assert(result == 0);

  arg_parser_dispose(parser);
}

void check_setting_short_flags() {
  arg_parser *parser = arg_parser_make(NULL);

  ARG_PARSER_ADD_STR(parser, "string", 's', "not required string", false);
  ARG_PARSER_ADD_INT(parser, "int", 'i', "not required int", false);
  ARG_PARSER_ADD_LONG(parser, "long", 'l', "not required long", false);
  ARG_PARSER_ADD_LL(parser, "long_long", 't', "not required long long", false);
  ARG_PARSER_ADD_DOUBLE(parser, "double", 'd', "not required double", false);
  ARG_PARSER_ADD_BOOL(parser, "bool", 'b', "not required bool", false);

  int argc = 13;
  // clang-format off
  char *argv[] = {"program_name",
                  "-s", "string",
                  "-i", "1",
                  "-l", "2",
                  "-t", "3",
                  "-d", "10.5",
                  "-b", "false",
  };
  // clang-format on
  int result = ARG_PARSER_PARSE(parser, argc, argv, false, NULL);
  assert(result == 0);

  const char *str          = NULL;
  int         int_value    = 0;
  long        long_value   = 0;
  long long   ll_value     = 0;
  double      double_value = 0;
  bool        bool_value   = 0;

  ARG_PARSER_GET_STR(parser, "string", str);
  ARG_PARSER_GET_INT(parser, "int", int_value);
  ARG_PARSER_GET_LONG(parser, "long", long_value);
  ARG_PARSER_GET_LL(parser, "long-long", ll_value);
  ARG_PARSER_GET_DOUBLE(parser, "double", double_value);
  ARG_PARSER_GET_BOOL(parser, "bool", bool_value);

  assert(strcmp(str, "string") == 0);
  assert(int_value == 1);
  assert(long_value == 2);
  assert(ll_value == 3);
  assert(double_value > 10.4 && double_value < 10.6);
  assert(bool_value == false);

  arg_parser_dispose(parser);
}

void check_several_values_for_one_flag() {
  arg_parser *parser = arg_parser_make(NULL);

  ARG_PARSER_ADD_STR(parser, "word", 'w', "list of words", true);

  // clang-format off
  int argc = 7;
  char *argv[] = {"program",
                  "--word=alpha",
                  "--word", "bravo",
                  "-w", "charlie",
                  "-w=delta"};
  // clang-format on
  int result = ARG_PARSER_PARSE(parser, argc, argv, false, NULL);
  assert(result == 0);

  int count = arg_parser_count(parser, "word");
  assert(count == 4);

  const char *rval_arr[4];
  arg_parser_get_args(parser, "word", ArgString, rval_arr, 4);

  assert(strcmp(rval_arr[0], "alpha") == 0);
  assert(strcmp(rval_arr[1], "bravo") == 0);
  assert(strcmp(rval_arr[2], "charlie") == 0);
  assert(strcmp(rval_arr[3], "delta") == 0);

  arg_parser_dispose(parser);
}

void check_bool_arg_without_val() {
  arg_parser *parser = arg_parser_make(NULL);

  ARG_PARSER_ADD_BOOL(parser, "bool-flag", 'b', NULL, true);

  // clang-format off
  int argc = 4;
  char *argv[] = {"program",
                  "--some-flag=some-val",
                  "-b", "--some-other-flag=some-other-value"};
  // clang-format on
  int result = ARG_PARSER_PARSE(parser, argc, argv, true, NULL);
  assert(result == 0);


  bool val = false;
  result   = ARG_PARSER_GET_BOOL(parser, "bool-flag", val);
  assert(result == 1);
  assert(val == true);

  arg_parser_dispose(parser);
}

int main() {
  check_arg_parser_create_and_dispose_only_with_desc(NULL);
  check_arg_parser_create_and_dispose_only_with_desc("");
  check_arg_parser_create_and_dispose_only_with_desc("some description");

  check_arg_flags();

  check_unknown_flag_fail();
  check_unknown_flag_not_fail();

  check_setting_short_flags();

  check_several_values_for_one_flag();
  check_bool_arg_without_val();

  return EXIT_SUCCESS;
}
