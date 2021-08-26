#include "arg_parser.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  arg_parser *parser = arg_parser_make("description:");

  ARG_PARSER_ADD_BOOL(parser, "help", 'h', "print usage info", false);
  ARG_PARSER_ADD_INT(parser, "some_int", 'i', "int value", true);
  ARG_PARSER_ADD_LONG(parser, "some_long", 0, "long value", false);
  ARG_PARSER_ADD_LL(parser, "some_ll", 0, "ll value", false);
  ARG_PARSER_ADD_DOUBLE(parser, "some_double", 0, "double value", false);
  ARG_PARSER_ADD_STR(parser, "some_str", 0, "string value", false);

  ARG_PARSER_ADD_BOOLD(parser,
                       "some_bool_d",
                       0,
                       "bool value wiht default",
                       true);
  ARG_PARSER_ADD_INTD(parser, "some_int_d", 0, "int value with default", 8000.);
  ARG_PARSER_ADD_LONGD(parser, "some_long_d", 0, "long value with default", 8);
  ARG_PARSER_ADD_LLD(parser, "some_ll_d", 0, "ll value with default", 10);
  ARG_PARSER_ADD_DOUBLED(parser,
                         "some_double_d",
                         0,
                         "double value with default",
                         0.1);
  ARG_PARSER_ADD_STRD(parser,
                      "some_str_d",
                      0,
                      "string value with default",
                      "default");


  std::string err_str;
  char *      err = NULL;
  ARG_PARSER_PARSE(parser, argc, argv, false, &err);

  if (err) {
    err_str = err;
    free(err);
  }

  bool need_help = false;
  if (ARG_PARSER_GET_BOOL(parser, "help", need_help) == 1 &&
      need_help == true) {
    char *usage = arg_parser_usage(parser);
    std::cout << usage;
    free(usage);

    arg_parser_dispose(parser);
    return EXIT_FAILURE;
  }

  if (err_str.empty() == false) {
    std::cout << "fail parsing args: " << err_str << std::endl;

    arg_parser_dispose(parser);
    return EXIT_FAILURE;
  }


  double dval = 0;
  ARG_PARSER_GET_DOUBLE(parser, "some_double_d", dval);
  std::cout << dval << std::endl;


  arg_parser_dispose(parser);
  return EXIT_SUCCESS;
}
